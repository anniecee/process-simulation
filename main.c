#include <stdio.h>
#include <stdlib.h>
#include "main.h"

static int id = 0;

// Startup
void startUp() {
    // Create 5 queues 
    high_priority = List_create();
    medium_priority = List_create();
    low_priority = List_create();
    send_queue = List_create();
    receive_queue = List_create();

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

int main() {

    // startUp() called
    startUp();
    
    // Print direction to screen:
    printf("Process Scheduling Simulation\n");
    printf("Please input your command and then enter. Input '!' to end program.\n");

    while(1){
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
    }

    return 0; 
}
