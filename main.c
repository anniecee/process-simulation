#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"

// Process information
static int id = 0;
static PCB* curr_running = NULL;

// Semaphore information
static int total_sem = 0;

// using for List_search function for Process ID
bool searchPid(void* pItem, void* pComparisonArg) {
    // Return true if pid matches comparision arg
    if(((PCB*)pItem)->pid == *(int*)pComparisonArg){
        return true;
    }
    else{
        return false;
    }
}

// using for List_search function for Semaphore ID
bool searchSid(void* sItem, void* pComparisonArg) {
    // Return true if sid matches comparision arg
    if(((semaphore*)sItem)->sid == *(int*)pComparisonArg){
        return true;
    }
    else {
        return false;
    }
}

// Function to run the next ready process!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
PCB* getProcess() {
    PCB* running_process = NULL;
    if (List_count(high_priority) != 0) {
        running_process = List_trim(high_priority);
    }
    else if (List_count(medium_priority) != 0) {
        running_process = List_trim(medium_priority);
    }
    else if (List_count(low_priority) != 0) {
        running_process = List_trim(low_priority);
    }
    else {
        running_process = init_process;
    }

    strcpy(running_process->state, "running");
    curr_running = running_process;

    return curr_running;
}

void checkInit() {
    // Init process set to "running" if all priority queues are empty
    if (List_count(high_priority) == 0 && List_count(medium_priority) == 0 && List_count(low_priority) == 0) {
        strcpy(init_process->state, "running");
    }
    // Init process set to "ready" otherwise
    else {
        strcpy(init_process->state, "ready");
    }
}

// For testing purpose; might adjust into Totalinfo()
void printList(List* list){
    printf("Current running process is: %d - %d - %s\n", curr_running->pid, curr_running->priority, curr_running->state);
    List_first(list);
    while(List_curr(list) != NULL){
        PCB* item = List_curr(list);
        printf("%d - %d - %s\n", item->pid, item->priority, item->state);
        List_next(list);
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
    all_jobs = List_create();
    all_semaphores = List_create();

    // Create init_process
    init_process = (PCB*)malloc(sizeof(PCB));
    strcpy(init_process->state, "running");
    init_process->pid = id++; // pid of init process = 0
    curr_running = init_process;
}

// function for create PCB (C command)
int createPCB(int priority){
    PCB* new_process = (PCB*)malloc(sizeof(PCB));

    new_process->pid = id++;
    new_process->priority = priority;
    strcpy(new_process->state, "ready");

    // If it is the first process created, set current running process and init process to ready
    if(id == 2){
        strcpy(init_process->state, "ready");
        strcpy(new_process->state, "running");
        curr_running = new_process;
    }
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
    if (List_count(all_jobs) == 0){
        printf("Error, there is no process to kill");
        return 0;
    }
    else if (curr_running->pid == 0 && List_count(all_jobs) != 0){
        printf("Error, cannot kill init process");
        return 0;
    }

    List* queues = List_create();
    List_prepend(queues, high_priority);
    List_prepend(queues, medium_priority);
    List_prepend(queues, low_priority);
    List_prepend(queues, send_queue);
    List_prepend(queues, receive_queue);

    while(List_curr(queues) != NULL){
        List* list = List_curr(queues);
        List_first(list);
        void* pcb_searched = List_search(list, searchPid, &pid);
        if (pcb_searched != NULL){
            List_remove(list);
            // REMEMBER TO FREE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
        }
        List_next(queues);
    }

    return 1;
}

int newSemaphore(int sid, int value) {
    // Check if semaphore ID is from 0 - 4
    if (sid < 0 || sid > 4) { return 1; }
    
    // Check if 5 semaphores are already available
    if (total_sem >= 5) { return 1; }

    // Check if initial value is > 0
    if (value < 0) { return 1; }

    // Otherwise, create semaphore
    semaphore* new_sem = (semaphore*)malloc(sizeof(semaphore));
    if (new_sem == NULL) { return 1; }

    new_sem->sid = sid;
    new_sem->value = value;
    new_sem->proc_list = List_create();

    // Add semaphore to list & increment total_sem
    List_prepend(all_semaphores, new_sem);
    total_sem++;

    return 0;
}

int semaphoreP(int sid) {
    // Search for targeted semaphore
    semaphore* searched_semaphore = List_search(all_semaphores, searchSid, &sid);
    if (searched_semaphore == NULL) { return 1; }

    // Decrement value of semaphore
    searched_semaphore->value--;

    // Add process to proc_list and block process if value < 0
    if (searched_semaphore->value < 0) {
        if (curr_running != NULL || curr_running->pid != 0) {
            // Add process to proc_list
            List_prepend(searched_semaphore->proc_list, curr_running);

            // Block running process
            strcpy(curr_running->state, "block");

            // Replace running process with a new proccess
            PCB* running_process = getProcess();
            
            // fail if running process is init
            if (running_process->pid == 0) { return 1; }
        }
        // fail if no process is running or running process is init
        else { 
            return 1; 
        }
    }

    return 0;
}

int semaphoreV(int sid) {
    // Search for targeted semaphore
    semaphore* searched_semaphore = List_search(all_semaphores, searchSid, &sid);
    if (searched_semaphore == NULL) { return 1; }

    // Increment value of semaphore
    searched_semaphore->value++;
    printf("Value: %d", searched_semaphore->value);

    // Get a process from proc_list and wake it up
    if (searched_semaphore->value <= 0) {
        // Fail if proc_list is empty
        if (List_count(searched_semaphore->proc_list) == 0) { return 1; }

        // Remove a process from proc_list & set it to ready
        PCB* waken_proc = List_trim(searched_semaphore->proc_list);
        strcpy(waken_proc->state, "ready");
        printf("State (ready): %s", waken_proc->state);
        printf("Priority: %d", waken_proc->priority);

        // Put the process on appropriate ready queue
        if (waken_proc->priority == 0){
            List_prepend(high_priority, waken_proc);
        }
        else if (waken_proc->priority == 1){
            List_prepend(medium_priority, waken_proc);
        }
        else {
            List_prepend(low_priority, waken_proc);
        }
    }

    return 0;
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
        if(command == 'F'){
            fork();
        }
        if(command == 'K'){
            kill(3);
        }
        if(command == 'O'){
            printList(high_priority);
        }
        if(command == 'A'){
            List* queues = List_create();
            List_prepend(queues, high_priority);
            List_prepend(queues, medium_priority);
            List_prepend(queues, low_priority);
            List_prepend(queues, send_queue);
            List_prepend(queues, receive_queue);
            while(List_curr(queues) != NULL){
                List* list = List_curr(queues);
                printList(list);
                List_next(queues);
            }
        }
        if (command == 'E') {
            if (curr_running->pid == 0 && List_count(all_jobs) == 0){
                // terminate the program
                printf("Program terminated!");
                break;
            }
            
            if (curr_running->pid != 0 && List_count(all_jobs) == 0){
                strcpy(init_process->state, "running");
                curr_running = init_process;
            }

            if (curr_running->pid != 0){
                // free current process
                // current running = getProcess()
            }
        }
        if (command == 'Q') {
            
        }
        if (command == 'S') {

        }
        if (command == 'R') {

        }
        if (command == 'Y') {

        }
        if (command == 'N') {
            printf("You chose New Semaphore command. Please input semaphore ID (from 0 to 4): ");
            
            // Get parameter and convert char to int
            char sid[3];
            fgets(sid, 3, stdin);
            int sid_num = atoi(sid);

            printf("Please input initial value for semaphore: ");
            
            // Get parameter and convert char to int
            char val[3];
            fgets(val, 3, stdin);
            int val_num = atoi(val);

            // Create new semaphore
            int result = newSemaphore(sid_num, val_num);
            if (result == 0) {
                printf("Success \n");
            } else {
                printf("Failure \n");
            }
        }
        if (command == 'P') {
            printf("You chose Semaphore P command. Please input semaphore ID (from 0 to 4): ");
            
            // Get parameter and convert char to int
            char sid[3];
            fgets(sid, 3, stdin);
            int sid_num = atoi(sid);

            // Call semaphoreP
            int result = semaphoreP(sid_num);
            if (result == 0) {
                printf("Success \n");
            } else {
                printf("Failure \n");
            }
        }
        if (command == 'V') {
            printf("You chose Semaphore V command. Please input semaphore ID (from 0 to 4): ");
            
            // Get parameter and convert char to int
            char sid[3];
            fgets(sid, 3, stdin);
            int sid_num = atoi(sid);

            // Call semaphoreV
            int result = semaphoreV(sid_num);
            if (result == 0) {
                printf("Success \n");
            } else {
                printf("Failure \n");
            }
        }
        if (command == 'I') {

        }
        if (command == 'T') {

        }
        if (command == '!') {

        }
    }

    return 0; 
}
