"# Process Simulation" 

1. Create:
2. Fork:
3. Kill:
4. Exit:
5. Quantum:
6. Send:
7. Receive:
8. Reply:
9. New semaphore:
- When create new semaphore, the program checks if semaphore ID is in range from 0 - 4; if semaphore with the same ID has already been created; and if initial value is > 0
- Keep all semaphores in a list called all_semaphores for later reference

10. Semaphore P:
- After decrementing value & checking if value < 0, running process will be blocked unless it's an init process or there's no process running
- Besides changing running process' state to 'blocked', we also replace running process with a new process from ready queue

11. Semaphore V:
- After incrementing value and checking if value <= 0, we will get a process from block list of semaphore and wake it up by setting state as 'ready' and put the process into appropriate priority queue.