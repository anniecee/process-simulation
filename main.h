#include "list.h"
#include <string.h>

#define SUCCESS 0
#define FAIL -1

#define MAX_LENGTH 40

// Process control block struct
typedef struct {
    int pid;        // Process ID
    int priority;   // Priority (0, 1 or 2)
    char state[10];   // State of process (running, ready, blocked, deadlocked)
    char proc_message[MAX_LENGTH]; // Message that another process "send"s or "reply"s to this process
} PCB;

// Semaphore struct
typedef struct {
    int sid;         // Semaphore ID (from 0 to 4)
    int value;       // Value of semaphore
    List *proc_list; // List of processes waiting on the semaphore
} semaphore;

typedef struct {
    int rcv_id;
    char message[MAX_LENGTH];
} packet;

// Queues
List* high_priority; // priority = 0
List* medium_priority; // priority = 1
List* low_priority; // priority = 2
List* send_queue;
List* receive_queue;
List* all_jobs;
List* packet_list;

// Semaphore list
List* all_semaphores; 

// Init process
PCB* init_process;

int kill(int pid);