#ifndef PROC_H_
#define PROC_H_

#include "stdint.h"

typedef void (*KthreadFunction)(void*);

void ProcInit(); // initializes proc system, only call once
bool ProcIsKernel();

void ProcRun(); // starts threading system, runs all threads to completion and returns
struct ProcContext* ProcCreateKthread(KthreadFunction entry_point, void* arg);
void ProcReschedule();
void ProcYield();
void ProcExit();
int ProcIsRunning(); // returns 1 if threading system is running, else 0

void ProcPrint();
ProcContext* ProcCreateCopy(uint64_t);

struct ProcQueue {
  struct ProcContext* head;
};

// Initializes a ProcQueue structure (mainly sets head to NULL).
// Called once for each ProcQueue during driver initialization.
void ProcInitQueue(struct ProcQueue* queue);

// Unblocks one process from the ProcQueue,
// moving it back to the scheduler.
// Called by interrupt handler?
// Returns whether or not a proc was unblocked
int ProcUnblockHead(struct ProcQueue* queue);

// Unblocks all processes from the ProcQueue,
// moving them all back to the scheduler.
// Called by interrupt handler?
void ProcUnblockAll(struct ProcQueue* queue);

// Blocks the current process.
// Called by system call handler.
//void ProcBlockOn(struct ProcQueue* queue, int enable_ints);
void ProcBlockOn(struct ProcQueue* queue);

// global for snakes
struct ProcContext {
  // hardware context
  // TODO add floating point state
  uint64_t rax; // 000
  uint64_t rbx; // 008
  uint64_t rcx; // 016
  uint64_t rdx; // 024
  uint64_t rdi; // 036
  uint64_t rsi; // 040
  uint64_t r8;  // 048
  uint64_t r9;  // 056
  uint64_t r10; // 064
  uint64_t r11; // 072
  uint64_t r12; // 080
  uint64_t r13; // 088
  uint64_t r14; // 096
  uint64_t r15; // 104
  uint64_t rbp; // 112
  uint64_t rsp; // 120 interrupt stack frame rsp+24
  uint64_t rip; // 128 interrupt stack frame rsp+0
  uint64_t rflags; // 136 interrupt stack frame rsp+16
  // TODO should _s registers be uint16_t instead?
  //      with endianness i feel like they might not work correctly
  uint64_t cs;  // 144 interrupt stack frame rsp+8
  uint64_t ss;  // 152 interrupt stack frame rsp+32
  // can i ignore these segment registers? what are they even for? how do i access them?
  uint64_t ds;  // 160
  uint64_t es;  // 168
  uint64_t fs;  // 176
  uint64_t gs;  // 184

  uint64_t cr3;

  // software context
  struct ProcContext* blocked_next;
  int is_blocked;
  uint64_t pid;
} __attribute__((packed)) __attribute__((aligned(16)));

// global for snakes
extern struct ProcContext* current_proc;

uint64_t* GetStackSaveState();
void RestoreState(ProcContext* proc);
void SaveState(ProcContext* proc);

#endif  // PROC_H_
