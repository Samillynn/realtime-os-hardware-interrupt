# Authors
- Hongyu Ding (20838222)
- Renhang Liu (21049351)

# project
cs452 K3

# Build Instructions
## download
```
git clone https://git.uwaterloo.ca/h44ding/cs452_k3.git
```
## build
```bash
cd cs452_k3
make clean && make
```
## run
put Pi number as the parameter for the ./upload.sh

for example, CS017542 (Pi No.2):
```bash
./upload.sh 2
```

# Kernel Descriptions
## Algorithms & Data Structures
- memory pool
In order to utilize some queue features, we implemented a memory pool for dynamic memory allocation. The pool is a data array preallocated on the kernel stack. We have a stack for keeping track of all the available (free) data chunks, which are initialized in the beginning based on the chunk size. Whenever we want to allocate some new data, we first check if the size of the data exceeds the memory pool chunk size. We proceed only if the size is within the range of the chunk size. Then we pop a free data chunk from the stack as the allocated memory. When we want to free the data we simply then push it back to the stack.
- clock server
The clock server uses a priority queue for storing all the delayed tasks' information. Every time we call a valid delay query, we will push that information with the task's corresponding delay ticks and number of delays to the priority queue. The priority is based on the delay ticks of the task. The smaller delay ticks means it will happen earlier so it will be put near the front of the queue. We use a clock notifier called `clock_server_tick()` for notifying the ticks. It calls `AwaitEvent` for getting the tick interrupts, and when it gets the interrupt, it notifies the server to add a tick. For every tick, the clock server will check if there is any task in the priority queue that reaches its desired delay tick, and if so it pop that information from the queue and wakes up that task.
- interrupts 
We use a priority queue for the interrupts. Since there are 1024 different IRQs are supported by the raspberry Pi, we have 1024 queues for different types of interrupts. Although this assignment only uses one interrupt (clock event), it is extensible for other interrupts.
Moreover, the interrupt handler uses a function pointer array to hold different interrupt handlers, which is similar to the exception handler.
- idle task
  - The main part of idle is an infinite while loop. When the idle task is running and an interrupt happens, the idle context switches to kernel at the end of the while loop. When it is scheduled again, it resumes its execution at the beginning of the while loop. 
After enter the while loop, the idle program:
    1. set DAIF bits to disable interrupt
    2. put the core into power-standby mode
    3. the whole program stops execution after entering power-standby mode,
    4. when any interrupt is triggered, the program starts its execution again. 
    5. But since the DAIF bits are set (interrupt disabled), the program will not jump to the exception table. Instead it continues running and goes to the next instruction of the idle task.
    6. the idle starts running again, 
    7. idle immediately clears the DAIF bits to enable the interrupt
    8. once the interrupt is enabled, the program will jump to the vector table, which allows the kernel to do its job.
  - To measure the idle time, some code are put between 1 and 2 (`idle_after_enter`),  and between 6 and 7 (`idle_before_exit`)
We try to put most code in `idle_after_enter`, and put as few as possible in `idle_before_exit`. To put few code in `idle_before_exit`, we make the system be able to respond to interrupt as fast as it can be.

## Limitations
- memory pool
The chunk size is fixed, and so we cannot allocate a super large memory. At the same time, since different size of data uses a fixed sized chunk, there will be a lot of memory wasted.
- prints
All the prints use busy waiting so if the kernel wants to print things it might take a long time, which might make the kernel miss ticks. All the prints in the user space could be interrupted, and so some prints might look corrupted if it takes too much time. Without compiler optimization, this issue will occur more frequently.
## Bugs
# Output Descriptions
## Output
```
clock_server successfully registered in name_server
task[4]: (delay: 10, num: 20)
task[5]: (delay: 23, num: 9)
task[6]: (delay: 33, num: 6)
task[7]: (delay: 71, num: 3)
Performance before the 0th IDLE: 100%
task[4]: (delay: 10, num: 19)
task[4]: (delay: 10, num: 18)
task[5]: (delay: 23, num: 8)
task[4]: (delay: 10, num: 17)
task[6]: (delay: 33, num: 5)
task[4]: (delay: 10, num: 16)
task[5]: (delay: 23, num: 7)
task[4]: (delay: 10, num: 15)
Performance before the 50th IDLE: 3%
task[4]: (delay: 10, num: 14)
task[6]: (delay: 33, num: 4)
task[5]: (delay: 23, num: 6)
task[4]: (delay: 10, num: 13)
task[7]: (delay: 71, num: 2)
task[4]: (delay: 10, num: 12)
task[4]: (delay: 10, num: 11)
task[5]: (delay: 23, num: 5)
task[6]: (delay: 33, num: 3)
task[4]: (delay: 10, num: 10)
Performance before the 100th IDLE: 3%
task[4]: (delay: 10, num: 9)
task[5]: (delay: 23, num: 4)
task[4]: (delay: 10, num: 8)
task[4]: (delay: 10, num: 7)
task[6]: (delay: 33, num: 2)
task[5]: (delay: 23, num: 3)
task[4]: (delay: 10, num: 6)
task[7]: (delay: 71, num: 1)
task[4]: (delay: 10, num: 5)
Performance before the 150th IDLE: 3%
task[4]: (delay: 10, num: 4)
task[5]: (delay: 23, num: 2)
task[6]: (delay: 33, num: 1)
task[4]: (delay: 10, num: 3)
task[4]: (delay: 10, num: 2)
task[5]: (delay: 23, num: 1)
task[4]: (delay: 10, num: 1)
Performance before the 200th IDLE: 4%
Performance before the 250th IDLE: 2%
Performance before the 300th IDLE: 1%
Performance before the 350th IDLE: 1%
Performance before the 400th IDLE: 1%
Performance before the 450th IDLE: 1%
Performance before the 500th IDLE: 1%
Performance before the 550th IDLE: 1%
Performance before the 600th IDLE: 1%

```
## Description
The name server will be started in the beginning of the first user task, and we don't include it here in the output. The first line states that the clock server is initialized after the name server's initialization. It then registers itself onto the name server. we first create four clients with tids 4, 5, 6, and 7. Then next four lines are the corresponding clients. Since all of them now call the delay operation, they are blocked. Then we drop to the idle task. We will print a line "Performance before the xth IDLE: x%" every 50 times we drop to the idle task. The percentage means the ratio of non-idle time to total time. The first time we drop to idle time thus will have 100% usage of non-idle tasks. All the clients will be waked up if delay finishes. Thus for example task with id 4 (task[4]) will delay for 10 ticks for 20 times. Then every time it prints its tid and its delay ticks and how many times it needs to delay for, and so "num" will be a countdown from 20 to 1. After num reaches 1 the client will exit so no more lines will be printed by the client.

It can be seen that the non-idle task usage is dropping after all the clients exit. This makes sense because fewer tasks are running and so idle task will run more frequently.


