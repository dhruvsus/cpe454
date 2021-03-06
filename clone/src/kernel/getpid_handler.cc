#include "getpid_handler.h"

#include "syscall_handler.h"
#include "syscall.h"
#include "proc.h"

static void HandleSyscallGetpid(uint64_t interrupt_number, uint64_t param_1, uint64_t param_2, uint64_t param_3) {
  // TODO security check userspace pointer
  uint64_t* pid_pointer = (uint64_t*) param_1;
  *pid_pointer = current_proc->pid;
}

void InitGetpid() {
  SetSyscallHandler(SYSCALL_GETPID, HandleSyscallGetpid);
}
