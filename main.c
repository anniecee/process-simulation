#include <stdio.h>
#include "main.h"

static int id = 0;

// Get input from user
char getInput() {
    char input;
    return input;
}

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

int main() {

    // startUp() called
    startUp();
    // Print direction to screen:
    printf("%s", init_process.state);
    printf("Process Scheduling Simulation\n");
    printf("Please input your command and then enter. Input '!' to end program.\n");

    return 0; 
}
