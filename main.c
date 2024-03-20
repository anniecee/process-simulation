#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"

static int id = 0;
static PCB* curr_running = NULL;

// using for List_search function
bool searchPid(void* pItem, void* pComparisonArg) {
    // Return true if pid matches comparision arg
    if(((PCB*)pItem)->pid == *(int*)pComparisonArg){
        return true;
    }
    else{
        return false;
    }
}

void checkInit() {
    // Init process set to "running" if all priority queues are empty
    if (List_count(high_priority) == 0 && List_count(medium_priority) == 0 && List_count(low_priority) == 0) {
        strcpy(init_process.state, "running");
    }
    // Init process set to "ready" otherwise
    else {
        strcpy(init_process.state, "ready");
    }
}

// Startup
void startUp() {
    // Create 5 queues 
    high_priority = List_create();
    medium_priority = List_create();
    low_priority = List_create();
    send_queue = List_create();
    receive_queue = List_create();
    all_pcb = List_create();

    // Create init_process
    strcpy(init_process.state, "running");
    init_process.pid = id++;
}

// function for create PCB (C command)
int createPCB(int priority){
    PCB* new_process = (PCB*)malloc(sizeof(PCB));

    new_process->pid = id++;
    new_process->priority = priority;
    strcpy(new_process->state, "ready");

    // If it is the first process created, set current running process and init process to ready
    if(id == 2 && curr_running == NULL){
        strcpy(init_process.state, "ready");
        strcpy(new_process->state, "running");
        curr_running = new_process;
    }
    // If not the first process, put into appropriate priority queue
    else {
        if(priority == 0){
            List_prepend(high_priority, new_process);
        }
        else if(priority == 1){
            List_prepend(medium_priority, new_process);
        }
        else {
            List_prepend(low_priority, new_process);
        }
    }
    // Put into list of all jobs
    List_prepend(all_jobs, new_process);
}

// fork (F command)
int fork() {
    // If no process is running or the current process is init process, fork fails
    if(curr_running == NULL){
        return 0;
    }
    else if(curr_running->pid == 0){
        return 0;
    }

    // Otherwise, create new process and return pid
    int new_id = createPCB(curr_running->priority);

    return new_id;
}

int kill(int pid) {
    List_first(high_priority);
    void* pcb_searched = List_search(high_priority, searchPid, &pid);
    if (pcb_searched != NULL){
        List_remove(high_priority);
    }
}

// For testing purpose; might adjust into Totalinfo()
void print(List* list){
    List_first(list);
    while(List_curr(list) != NULL){
        PCB* item = List_curr(list);
        printf("%d - %d - %s\n", item->pid, item->priority, item->state);
        List_next(list);
    }
}

int main() {

    // startUp() called
    startUp();
    
    // Print direction to screen:
    printf("Process Scheduling Simulation\n");
    printf("Please input your command and then enter. Input '!' to end program.\n");

    while(1){
        // Check init process
        checkInit();

        char command = getchar();
        while(getchar() != '\n'); // Clear input buffer

        if(command == 'C'){
            printf("You chose Create command, please input priority (0 - High, 1 - Medium, 2 - Low): ");
            
            // Get parameter and convert char to int
            char priority[3];
            fgets(priority, 3, stdin);
            int priority_num = atoi(priority);

            createPCB(priority_num);
            
        }
        else if(command == 'F'){
            fork();
        }
        else if(command == 'K'){
            kill(3);
        }
        else if(command = 'O'){
            print(high_priority);
        }
        else if (command == 'E') {

        }
        else if (command == 'Q') {
            
        }
        else if (command = 'S') {

        }
        else if (command == 'R') {

        }
        else if (command == 'Y') {

        }
        else if (command == 'N') {

        }
        else if (command == 'P') {

        }
        else if (command == 'V') {

        }
        else if (command == 'I') {

        }
        else if (command == 'T') {

        }
        else if (command == '!') {

        }
    }

    return 0; 
}
