#include "list.h"
#include <string.h>

// Process control block
typedef struct {
    int pid;        // Process ID
    int priority;   // Priority (0, 1 or 2)
    string state;   // State of process (running, ready, blocked, deadlocked)
    string proc_message; // Message that another process "send"s or "reply"s to this process
} PCB;

typedef struct {
    int value;       // Value of semaphore
    List *proc_list; // List of processes waiting on the semaphore
} semaphore;
