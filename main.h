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
    int value;       // Value of semaphore
    List *proc_list; // List of processes waiting on the semaphore
} semaphore;

// Queues
List* high_priority;
List* medium_priority;
List* low_priority;
List* send_queue;
List* receive_queue;


PCB init_process;


// typedef struct {
//     int priority;       // Value of priority (0, 1, 2)
//     List *queue_list; // Queue structure
// } queue;