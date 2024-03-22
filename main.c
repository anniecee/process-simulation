#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

void procFree(void* item){
    if (item != NULL){
        // free(((PCB*)item)->proc_message);
        free(item);
    }
}

void terminateProgram(){
    // Free queue lists
    List_free(high_priority, procFree);
    List_free(medium_priority, procFree);
    List_free(low_priority, procFree);
    List_free(send_queue, procFree);
    List_free(receive_queue, procFree);
    List_free(all_jobs, procFree);
    // Free semaphore list

    // Free init process

}

// Function to run the next ready process!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
PCB* getProcess() {
    PCB* running_process = NULL;

    // Get new running process from appropriate priority queue
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

    // Remove the running process from list of all ready jobs
    List_first(all_jobs);
    if (running_process->pid != 0){
        List_search(all_jobs, searchSid, &(running_process->pid));
        List_remove(all_jobs);
    }

    // Set state to running
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

// Bring process back to ready queue
void toReadyQueue(PCB* process){
    int priority = process->priority;
    
    // TO-DO: Change state of process to 'ready'

    if(priority == 0){
        List_prepend(high_priority, process);
    }
    else if(priority == 1){
        List_prepend(medium_priority, process);
    }
    else {
        List_prepend(low_priority, process);
    }

    // Put into list of all jobs
    List_prepend(all_jobs, process);
}

// Startup
void startUp() {
    // Create all queues 
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

// Function for create PCB (C command)
int createPCB(int priority){
    // Check if process priority is from 0 - 2
    if (priority < 0 || priority > 2) { return FAIL; }

    // Create new process
    PCB* new_process = (PCB*)malloc(sizeof(PCB));
    if (new_process == NULL ) { return FAIL; }

    new_process->pid = id++;
    new_process->priority = priority;
    strcpy(new_process->state, "ready");
    strcpy(new_process->proc_message, "");

    // If it is the first process created, set current running process and init process to ready
    if(curr_running->pid == 0){
        strcpy(init_process->state, "ready");
        strcpy(new_process->state, "running");
        curr_running = new_process;
    }
    else {
        toReadyQueue(new_process);
    }

    return new_process->pid;
}

// fork (F command)
int fork() {
    // If no process is running or the current process is init process, fork fails
    if(curr_running == NULL){
        return FAIL;
    }
    else if(curr_running->pid == 0){
        return FAIL;
    }

    // Otherwise, create new process and return pid
    int new_id = createPCB(curr_running->priority);

    return new_id;
}

int kill(int pid) {
    // Search for process
    List_first(all_jobs);
    void* pcb_searched = List_search(all_jobs, searchPid, &pid);

    // If running process is init OR process can't be found, cannot kill
    if (curr_running->pid == 0 && List_count(all_jobs) == 0){
        printf("Error, cannot kill init process!\n");
        return FAIL;
    }
    else if (pcb_searched == NULL){
        printf("Error, there is no process of given id to kill!\n");
        return FAIL;
    }

    // TO-DO: What if the current running is the one need to be killed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    // Create all_queues to check for pid of process
    List* all_queues = List_create();
    List_prepend(all_queues, high_priority);
    List_prepend(all_queues, medium_priority);
    List_prepend(all_queues, low_priority);
    List_prepend(all_queues, send_queue);
    List_prepend(all_queues, receive_queue);
    // TO-DO: Check semaphores lists to kill

    // Go through all queues to kill the targeted process
    while(List_curr(all_queues) != NULL){
        List* list = List_curr(all_queues);
        List_first(list);
        pcb_searched = List_search(list, searchPid, &pid);
        if (pcb_searched != NULL){
            // Remove in the waiting and ready lists
            List_remove(list);
            // Remove in all jobs lists
            List_first(all_jobs);
            List_search(all_jobs, searchPid, &pid);
            List_remove(all_jobs);
            // REMEMBER TO FREE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
        }
        List_next(all_queues);
    }

    return SUCCESS;
}

int newSemaphore(int sid, int value) {
    // Check if semaphore ID is from 0 - 4
    if (sid < 0 || sid > 4) { return FAIL; }
    
    // Check if 5 semaphores are already available
    if (total_sem >= 5) { return FAIL; }

    // Check if the semaphore has already been created
    List_first(all_semaphores);
    semaphore* searched_sem = List_search(all_semaphores, searchSid, &sid);
    if (searched_sem != NULL) { return FAIL; }

    // Check if initial value is < 0
    if (value < 0) { return FAIL; }

    // Otherwise, create semaphore
    semaphore* new_sem = (semaphore*)malloc(sizeof(semaphore));
    if (new_sem == NULL) { return FAIL; }

    new_sem->sid = sid;
    new_sem->value = value;
    new_sem->proc_list = List_create();

    printf("Created semaphore with ID: %d and initial value %d.\n", sid, value);

    // Add semaphore to list & increment total_sem
    List_prepend(all_semaphores, new_sem);
    total_sem++;

    return SUCCESS;
}

int semaphoreP(int sid) {
    // Search for targeted semaphore
    List_first(all_semaphores);
    semaphore* searched_sem = List_search(all_semaphores, searchSid, &sid);
    if (searched_sem == NULL) { return FAIL; }

    // Decrement value of semaphore
    searched_sem->value--;
    printf("Decremented value of semaphore to: %d.\n", searched_sem->value);

    // Add process to proc_list and block process if value < 0
    if (searched_sem->value < 0) {
        if (curr_running != NULL || curr_running->pid != 0) {
            // Add process to proc_list
            PCB* temp_process = curr_running;
            List_prepend(searched_sem->proc_list, curr_running);

            // Block running process
            strcpy(curr_running->state, "blocked");

            // Replace running process with a new proccess
            PCB* running_process = getProcess();
            
            // fail if running process is init
            if (running_process->pid == 0) { return FAIL; }

            printf("Blocked process with ID: %d.\n", temp_process->pid);
        }
        // fail if no process is running or running process is init
        else { 
            return FAIL; 
        }
    } else {
        printf("No process was blocked.\n");
    }

    return SUCCESS;
}

int semaphoreV(int sid) {
    // Search for targeted semaphore
    List_first(all_semaphores);
    semaphore* searched_sem = List_search(all_semaphores, searchSid, &sid);
    if (searched_sem == NULL) { return FAIL; }

    // Increment value of semaphore
    searched_sem->value++;
    printf("Incremented value of semaphore to: %d.\n", searched_sem->value);

    // Get a process from proc_list and wake it up
    if (searched_sem->value <= 0) {
        // Fail if proc_list is empty
        if (List_count(searched_sem->proc_list) == 0) { return FAIL; }

        // Remove a process from proc_list & set it to ready
        PCB* waken_proc = List_trim(searched_sem->proc_list);
        strcpy(waken_proc->state, "ready");

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

        printf("Set process with ID: %d to ready.\n", waken_proc->pid);
    } else {
        printf("No process was waken up.\n");
    }

    return SUCCESS;
}

int procInfo(int pid) {
    // Search for process
    List_first(all_jobs);
    PCB* pcb_searched = List_search(all_jobs, searchPid, &pid);

    if (pcb_searched == NULL && curr_running->pid != pid) { 
        printf("Error. Input process ID is not available.\n");
        return FAIL; 
    }
    // Set pcb_searched to currently running process if the requested pid is running process
    if (pcb_searched == NULL && curr_running->pid == pid) {
        pcb_searched = curr_running;
    }

    // Dump information
    printf("Process Information:\n");
    printf("Process ID: %d\n", pcb_searched->pid);
    printf("Priority: %d (0 - High, 1 - Medium, 2 - Low)\n", pcb_searched->priority);
    printf("State: %s\n", pcb_searched->state);
    printf("Message: %s\n", pcb_searched->proc_message);

    return SUCCESS;
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
            printf("You chose Create command. Please input priority (0 - High, 1 - Medium, 2 - Low): ");
            
            // Get parameter and convert char to int
            char priority[3];
            fgets(priority, 3, stdin);
            int priority_num = atoi(priority);

            // Create new process
            int result = createPCB(priority_num);;
            if (result == FAIL) {
                printf("Fail \n");
            } else {
                printf("New process with ID: %d created! \n", result);
                printf("Success \n");
            }
            
        }
        if(command == 'F'){
            printf("You chose Fork command.\n");
            int result = fork();
            if (result == FAIL) {
                printf("Fail \n");
            } else {
                printf("Forked a new process with ID: %d.\n", result);
                printf("Success\n");
            }
        }
        if(command == 'K'){
            printf("You chose Kill command. Please input process ID you want to kill: ");
            
            // Get parameter and convert char to int
            char pid[3];
            fgets(pid, 3, stdin);

            int result = kill(atoi(pid));
            if (result == FAIL) {
                printf("Fail \n");
            } else {
                printf("Killed process %s", pid);
                printf("Success\n");
            }
        }
        if(command == 'A'){
            printList(all_jobs);
        }
        if(command == 'L'){
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
                // Terminate the program
                terminateProgram();
                // Free
                printf("Program terminated!\n");
                break;
            }
            // current runnning is not init and there is no jobs left
            if (curr_running->pid != 0 && List_count(all_jobs) == 0){
                strcpy(init_process->state, "running");
                free(curr_running);
                curr_running = init_process;
            }
            else if (curr_running->pid != 0){
                // free current process
                free(curr_running);
                curr_running = getProcess();
            }
            printf("Exit successful\n");
        }
        if (command == 'Q') {
            printf("You called Quantum!\n");
            if (curr_running->pid != 0){
                // Update new priority if priority is not low
                if (curr_running->priority != 2){
                    curr_running->priority += 1;
                }
                strcpy(curr_running->state, "ready");
                toReadyQueue(curr_running);
                getProcess();
            }
            printf("Quantum successful\n");
        }
        if (command == 'S') {
            printf("You chose Send command, please input process ID and message with a space in between ('id' 'message'): ");
            // Get receive id with message and separate those two
            char input_message[1000];
            fgets(input_message, 1000, stdin);
            int rcv_id = atoi(&input_message[0]);
            char message[1000];
            strcpy(message, &input_message[2]);

            // Search for the recv process
            List_first(receive_queue);
            PCB* rcv_search = List_search(receive_queue, searchPid, &rcv_id);
            if (rcv_search != NULL){
                // // Unblock recv process and put back to ready queue
                // strcpy(rcv_search->proc_message, message);
                // strcpy(rcv_search->state, "ready");
                // toReadyQueue(rcv_search);

                // // Remove process from receive wait
                // List_remove(receive_queue);
            }
            else{
                // If no process found in recv wait, block the sender until there is a recv
                strcpy(curr_running->proc_message, message);
                strcpy(curr_running->state, "blocked");
                List_prepend(send_queue, curr_running);
                getProcess();
            }



        }
        if (command == 'R') {
            printf("You chose Receive command!");

            // Search for the send process
            List_first(receive_queue);
            PCB* send_search = List_search(send_queue, searchPid, &(curr_running->pid));
            if (send_search != NULL){
                // Take message from sender
                printf("Message received: %s", send_search->proc_message);
            }
            else{
                // If no process found in send wait, block the recv until there is a send
                strcpy(curr_running->state, "blocked");
                List_prepend(receive_queue, curr_running);
                getProcess();
            }
            
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
            if (result == SUCCESS) {
                printf("Success \n");
            } else {
                printf("Fail \n");
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
            if (result == SUCCESS) {
                printf("Success \n");
            } else {
                printf("Fail \n");
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
            if (result == SUCCESS) {
                printf("Success \n");
            } else {
                printf("Fail \n");
            }
        }
        if (command == 'I') {
            printf("You chose Process Information command. Please input process ID: ");
            
            // Get parameter and convert char to int
            char pid[3];
            fgets(pid, 3, stdin);
            int pid_num = atoi(pid);

            // Call procInfo()
            int result = procInfo(pid_num);
            if (result == FAIL) {
                printf("Fail \n");
            } else {
                printf("Success \n");
            }
        }
        if (command == 'T') {

        }
        if (command == '!') {

        }
    }

    return 0; 
}
