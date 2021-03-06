#include "proc.h"

#include "stdint.h"
#include "syscall.h"
#include "syscall_handler.h"
#include "kmalloc.h"
#include "page.h"
#include "asm.h"
#include "printk.h"
#include "list.h"
#include "user.h"
#include "string.h"
#include "frame.h"

#define INTERRUPT_ENABLE_BIT (1 << 9)

// iretq, interrupt stack frame holds:
// ss, rsp, rflags, cs, rip
// https://users.csc.calpoly.edu/~bellardo/courses/2174/454/notes/CPE454-Week02-2.pdf

static int is_proc_running = 0;
static uint64_t new_proc_id = 1;
struct ProcContext* current_proc = 0; // global for snakes
static struct ProcContext* next_proc = 0; // set by ProcReschedule()
static struct ProcContext* main_proc = 0; // context of thread that called ProcRun()

static List<struct ProcContext> proc_list = List<struct ProcContext>();

// starts system, returns when all threads are complete
void ProcRun() {
  if (proc_list.IsEmpty()) {
    // there are no procs to run
    printk("ProcRun() proc_lsit.IsEmpty(): %p\n", proc_list.IsEmpty());
    return;
  }

  current_proc = proc_list.GetHead(); // current_proc will be run first
  next_proc = 0;
  Syscall(SYSCALL_PROC_RUN);
}

ProcContext* ProcCreateKthread(KthreadFunction entry_point, void* arg) {
  struct ProcContext* new_proc = (struct ProcContext*) kcalloc(sizeof(struct ProcContext));
  new_proc->rip = (uint64_t) entry_point;
  new_proc->cs = 0x8; // kernel or user, for privilege level
  new_proc->rsp = (uint64_t) StackAllocate(); // TODO consider stack overflow, its only 2MB virt
                                              // TODO free this when changing proc to user?
  new_proc->ss = 0; // for kernel
  new_proc->rflags = INTERRUPT_ENABLE_BIT;
  new_proc->pid = new_proc_id++;
  new_proc->cr3 = (uint64_t) Getcr3();

  // set first C argument to new proc function to void* arg
  new_proc->rdi = (uint64_t) arg;

  // push ProcExit() onto stack
  uint64_t* stack_pointer = (uint64_t*) new_proc->rsp;
  *stack_pointer = (uint64_t) &ProcExit;

  // add new_proc to linked list
  proc_list.Add(new_proc);

  return new_proc;
}

// this is intended for user processes for fork()
ProcContext* ProcCreateCopy(uint64_t new_rip) {
  // TODO check to make sure procs are running first?
  ProcContext* new_proc = (ProcContext*) kcalloc(sizeof(ProcContext));

  SaveState(current_proc); // update current_proc registers

  // this copies the instruction pointer from right before the syscall?
  memcpy(new_proc, current_proc, sizeof(ProcContext));
  new_proc->rip = new_rip;
  printk("new_proc->rip: %p\n", new_proc->rip);
  printk("new_proc->rsp: %p\n", new_proc->rsp);
  printk("new_proc->rbp: %p\n", new_proc->rbp);

  // set new page table for new proc
  //new_proc->cr3 = (uint64_t) CopyCurrentPageTable();
  new_proc->pid = new_proc_id++;

  proc_list.Add(new_proc);

  return new_proc;
}

// this functionality is replaced with usermode setting in exec()
// TODO usermode setting in exec() seems wrong, maybe should be done somewhere else
/*ProcContext* ProcCreateUserThread(KthreadFunction entry_point, void* arg) {
  struct ProcContext* new_proc = (struct ProcContext*) kcalloc(sizeof(struct ProcContext));
  new_proc->rip = (uint64_t) entry_point;
  new_proc->cs = GDT_USER_CS;
  new_proc->rsp = USER_STACK_BOTTOM; // TODO where should user stack go and how should it be allocated?
  new_proc->ss = GDT_USER_DS;
  new_proc->rflags = INTERRUPT_ENABLE_BIT | (3 << 12); // user mode flags
  new_proc->pid = new_proc_id++;

  // set first C argument to new proc function to void* arg
  new_proc->rdi = (uint64_t) arg;

  // push ProcExit() onto stack
  // this will be done in exec handler for user procs instead
  // uint64_t* stack_pointer = (uint64_t*) new_proc->rsp;
  // *stack_pointer = (uint64_t) &ProcExit;

  // add new_proc to linked list
  proc_list.Add(new_proc);

  return new_proc;
}*/

// returns 0 if all procs are blocked
// uses round robin from current_proc
static struct ProcContext* GetNextUnblockedProc() {
  if (!current_proc || proc_list.IsEmpty()) {
    printk("GetNextUnblockedProc() current_proc: %p, proc_list.IsEmpty(): %p\n", current_proc, proc_list.IsEmpty());
    return 0;
  }

  BEGIN_CS(); // interrupts can change blocking

  struct ProcContext* proc = current_proc;
  do {
    proc = proc_list.GetNext(proc);

    if (!proc->is_blocked) {
      END_CS();
      return proc;
    }

    if (proc == current_proc) {
      // we have looped all the way around and have found no unblocked procs
      END_CS();
      return 0;
    }
  } while (proc != current_proc);

  END_CS();
  return 0;
}

// sets next_proc to the next process to switch contexts to during yield or exit
// if there is only one proc left, then sets next_proc = current_proc
void ProcReschedule() {
  if (!current_proc || proc_list.IsEmpty()) {
    // there are no processes to schedule. this shouldn't happen, right?
    printk("ProcReschedule() current_proc: %p, proc_list.IsEmpty(): %p\n", current_proc, proc_list.IsEmpty());
    return;
  }

  int interrupts_were_enabled = are_interrupts_enabled();

  do {
    sti(); // force disable interrupts
    // hope that interrupts are handled in between these lines.
    // try to find an unblocked proc multiple times to
    // reduce chance of interrupt handling before halting
    //for (int i = 0; !next_proc && i < 30; i++) {
    for (int i = 0; i < 30; i++) {
      next_proc = GetNextUnblockedProc();
    }
    // TODO reduce number of instructions between block checking and hlt()
    if (!next_proc) {
      hlt();
    }
  } while (!next_proc);

  if (interrupts_were_enabled) {
    cli();
  }

  /*
  // round robin scheduler, just pick the next proc in the linked list
  //next_proc = current_proc->all_procs_next;
  next_proc = current_proc;

  BEGIN_CS(); // interrupts can change process blocking

  //while (next_proc->is_blocked) {
  while (1) {
    if (!next_proc->all_procs_next) {
      next_proc = all_procs;
    } else {
      next_proc = next_proc->all_procs_next;
    }

    if (next_proc == current_proc && current_proc->is_blocked) {
      // we have wrapped around all procs, there are no
      // unblocked procs to run

      // TODO figure out how to atomically halt and enable interrupts at the same time.
      // If i enable interrupts then halt, then an interrupt to unblock a process could be
      // handled before calling halt, in which case there should be no halting and
      // execution would not resume until another interrupt is fired
      
      // there this a chance this could stay blocked for two interrupts instead of one
      //END_CS();
      sti();
      // in between these lines an interrupt could be handled, in which case we don't want to hlt
      asm volatile ("hlt");
      //BEGIN_CS();
      cli();

    } else {
      // consider running next_proc
      if (next_proc->is_blocked) {
        // cant run next_proc
      } else {
        // can run next_proc
        break;
      }
    }
  }

  END_CS();*/
}

void ProcYield() {
  ProcReschedule();
  Syscall(SYSCALL_YIELD);
}

void ProcExit() {
  ProcReschedule();
  Syscall(SYSCALL_EXIT);
}

extern uint64_t stack_save_state_address[];
// TODO make static?
uint64_t* GetStackSaveState() {
  return (uint64_t*) stack_save_state_address[0];
}

// TODO make static?
void SaveState(struct ProcContext* proc) {
  uint64_t* stack_save_state = GetStackSaveState();
  proc->rbp = stack_save_state[1];
  proc->r15 = stack_save_state[2];
  proc->r14 = stack_save_state[3];
  proc->r13 = stack_save_state[4];
  proc->r12 = stack_save_state[5];
  proc->r11 = stack_save_state[6];
  proc->r10 = stack_save_state[7];
  proc->r9 = stack_save_state[8];
  proc->r8 = stack_save_state[9];
  proc->rsi = stack_save_state[10];
  proc->rdx = stack_save_state[11];
  proc->rcx = stack_save_state[12];
  proc->rbx = stack_save_state[13];
  proc->rax = stack_save_state[14];
  proc->rdi = stack_save_state[15];
  proc->rip = stack_save_state[16];
  proc->cs = stack_save_state[17];
  proc->rflags = stack_save_state[18];
  proc->rsp = stack_save_state[19];
  proc->ss = stack_save_state[20];
  proc->cr3 = (uint64_t) Getcr3();
}

// TODO make static
void RestoreState(struct ProcContext* proc) {
  //printk("RestoreState() restoring rip %p\n", proc->rip);
  uint64_t* stack_save_state = GetStackSaveState();
  //printk("RestoreState() stack_save_state: %p\n", stack_save_state);
  stack_save_state[1] = proc->rbp;
  stack_save_state[2] = proc->r15;
  stack_save_state[3] = proc->r14;
  stack_save_state[4] = proc->r13;
  stack_save_state[5] = proc->r12;
  stack_save_state[6] = proc->r11;
  stack_save_state[7] = proc->r10;
  stack_save_state[8] = proc->r9;
  stack_save_state[9] = proc->r8;
  stack_save_state[10] = proc->rsi;
  stack_save_state[11] = proc->rdx;
  stack_save_state[12] = proc->rcx;
  stack_save_state[13] = proc->rbx;
  stack_save_state[14] = proc->rax;
  stack_save_state[15] = proc->rdi;
  stack_save_state[16] = proc->rip;
  stack_save_state[17] = proc->cs;
  stack_save_state[18] = proc->rflags;
  stack_save_state[19] = proc->rsp;
  stack_save_state[20] = proc->ss;
  Setcr3(proc->cr3);
}

static void HandleSyscallProcRun(uint64_t syscall_number, uint64_t param_1, uint64_t param_2, uint64_t param_3) {
  // save "real" state into main_proc, and load first proc in current_proc
  if (!current_proc) {
    printk("HandleSyscallProcRun() current_proc: %p\n", current_proc);
    return;
  }

  is_proc_running = 1;

  main_proc = (struct ProcContext*) kcalloc(sizeof(struct ProcContext));
  SaveState(main_proc);
  RestoreState(current_proc);
}

static void HandleSyscallYield(uint64_t syscall_number, uint64_t param_1, uint64_t param_2, uint64_t param_3) {
  //TODO investigate more   printk("HandleSyscallYield() are_interrupts_enabled: %d\n", are_interrupts_enabled());
  // switch contexts
  if (!current_proc || !next_proc) {
    printk("HandleSyscallYield() current_proc: %p, next_proc: %p\n", current_proc, next_proc);
    return;
  }

  // save context put on stack by irq_syscall
  SaveState(current_proc);

  current_proc = next_proc;
  next_proc = 0;

  // put new context onto stack, to be restored by irq_syscall
  RestoreState(current_proc);
}

// TODO make this get handled on a different stack to stop the GP faults?
static void HandleSyscallExit(uint64_t syscall_number, uint64_t param_1, uint64_t param_2, uint64_t param_3) {
  // destroy current process and switch context to next process
  
  if (!current_proc || !next_proc || proc_list.IsEmpty()) {
    printk("HandleSyscallExit() current_proc: %p, next_proc: %p, proc_list.IsEmpty(): %p\n",
        current_proc, next_proc, proc_list.IsEmpty());
    return;
  }

  // TODO in order to round robin correctly,
  // set current_proc to the proc prior to current_proc in the list
  // remove current_proc
  // call ProcReschedule()
  // current_proc = next_proc
  // restore current_proc

  struct ProcContext* current_proc_prev = proc_list.GetPrevious(current_proc);

  // remove current_proc from linked list
  proc_list.Remove(current_proc);

  // free current_proc resources
  // TODO free page table
  StackFree((void*) current_proc->rsp); // StackFree() takes any address within the stack
  kfree(current_proc);

  // set current_proc to the proc prior to current_proc in the list
  current_proc = current_proc_prev;

  if (proc_list.IsEmpty()) {
    // no more procs to run, restore main context
    is_proc_running = 0;
    RestoreState(main_proc);
    kfree(main_proc);
  } else {
    ProcReschedule();
    current_proc = next_proc;
    next_proc = 0;
    RestoreState(current_proc);
  }
}

// Initializes a ProcQueue structure (mainly sets head to NULL).
// Called once for each ProcQueue during driver initialization
void ProcInitQueue(struct ProcQueue* queue) {
  queue->head = 0;
}

// Unblocks one process from the ProcQueue,
// moving it back to the scheduler.
// Called by interrupt handler?
// Returns whether or not a proc was unblocked
int ProcUnblockHead(struct ProcQueue* queue) {
  BEGIN_CS();

  int removed_proc = 0;
  struct ProcContext* unblocked = queue->head;
  if (unblocked) {
    queue->head = unblocked->blocked_next;
    unblocked->blocked_next = 0;
    unblocked->is_blocked = 0;
    removed_proc = 1;
  } else {
    // this queue is empty
  }

  END_CS();
  return removed_proc;
}

// Unblocks all processes from the ProcQueue,
// moving them all back to the scheduler
void ProcUnblockAll(struct ProcQueue* queue) {
  BEGIN_CS();
  while (ProcUnblockHead(queue));
  END_CS();
}

// Blocks the current process.
// Called by system call handler.
//void ProcBlockOn(struct ProcQueue* queue, int enable_ints) {
void ProcBlockOn(struct ProcQueue* queue) {
  BEGIN_CS(); // appending to the queue must be atomic, it can be edited by interrupt handlers
  /*int interrupts_were_enabled = are_interrupts_enabled();
  cli();*/

  if (queue->head) {
    struct ProcContext* last_in_queue = queue->head;
    while (last_in_queue->blocked_next) {
      last_in_queue = last_in_queue->blocked_next;
    }
    last_in_queue->blocked_next = current_proc;
  } else {
    queue->head = current_proc;
  }

  current_proc->is_blocked = 1;

  END_CS();

  ProcYield(); // i can do nested syscalls, right?
}

void ProcPrint() {
  if (proc_list.IsEmpty()) {
    printk("ProcPrint() no processes\n");
  } else {
    struct ProcContext* proc = proc_list.GetHead();
    do {
      printk("pid %d is_blocked %d\n", proc->pid, proc->is_blocked);
      proc = proc_list.GetNext(proc);
    } while (proc != proc_list.GetHead());
  }
}

int ProcIsRunning() {
  return is_proc_running;
}

void ProcInit() {
  SetSyscallHandler(SYSCALL_YIELD, HandleSyscallYield);
  SetSyscallHandler(SYSCALL_EXIT, HandleSyscallExit);
  SetSyscallHandler(SYSCALL_PROC_RUN, HandleSyscallProcRun);
}

bool ProcIsKernel() {
  uint64_t privilege_level = (current_proc->rflags >> 12) & 3;
  return privilege_level == 0; // zero is kernel, 3 is user
}
