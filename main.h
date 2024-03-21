#include "list.h"
#include <string.h>

// Process control block
typedef struct {
    int pid;        // Process ID
    int priority;   // Priority (0, 1 or 2)
    char state[10];   // State of process (running, ready, blocked, deadlocked)
    char proc_message[100]; // Message that another process "send"s or "reply"s to this process
} PCB;

typedef struct {
    int sid;         // Semaphore ID (from 0 to 4)
    int value;       // Value of semaphore
    List *proc_list; // List of processes waiting on the semaphore
} semaphore;

// Queues
List* high_priority; // priority = 0
List* medium_priority; // priority = 1
List* low_priority; // priority = 2
List* send_queue;
List* receive_queue;
List* all_jobs;

// List of semaphore
List* all_semaphores; 

// Init process
PCB* init_process;
