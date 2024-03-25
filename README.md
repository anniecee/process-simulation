"# Process Simulation" 

1. Create:
- Create new process and put the new process in appropriate priority queues.
- If it is the first process created, set current running process and init process to ready.

2. Fork:
- Create a new process with the current running process properties (copy only priority) and return a new id.

3. Kill:
- Kill the user input process id and remove it from all the waiting and blocking queues.
- If the current running proess is init process then it cannot be killed and return fail.

4. Exit:
- Kill the current running process.
- If the current running proess is init process and there is no process waiting or blocking then the program terminated.

5. Quantum:
- Pre-empt current running process and put it back to ready queue with lower priority.

6. Send:
- Send before receive:
    + Append the message and the rcv_id in packet_list (list of message).
    + Block current send process in send wait queue.

- Receive before send:
    + Unblock receive process, assign the message to it and put it back to ready queue.
    + Block current send process in send wait queue.

- Notes:
    + Init process cannot operate Send command.
    + Cannot send to init process.
    + Multiple processes can send to one process but the message will be queued in message list (FCFS).

7. Receive:
- Send before receive:
    + Print the first message found in message list with the same receive ID.
    + Block current receive process in receive wait queue if there is no more message in message list.

- Receive before send:
    + Block current receive process in receive wait queue.

- Notes:
    + Init process cannot operate Receive command.
    + One process can be received from multiple processes.

8. Reply:
- If reply to a process haven't sent before, then return fail.

- Reply to a process will unblock it from send wait queue and assign the message to it. The next time that process is running, it will print the message.

- Notes:
    + Init process can operate Reply command but cannot be replied by another process.
    + One process can reply to multiple processes.

9. New semaphore:
- When create new semaphore, the program checks if semaphore ID is in range from 0 - 4; if semaphore with the same ID has already been created; and if initial value is > 0.
- Keep all semaphores in a list called all_semaphores for later reference.

10. Semaphore P:
- After decrementing value & checking if value < 0, running process will be blocked unless it's an init process or there's no process running.
- Besides changing running process' state to 'blocked', we also replace running process with a new process from ready queue.

11. Semaphore V:
- After incrementing value and checking if value <= 0, we will get a process from block list of semaphore and wake it up by setting state as 'ready' and put the process into appropriate priority queue.