# Authors
- Hongyu Ding (20838222)
- Renhang Liu (21049351)

# project

# Build Instructions
## download
```
git clone https://git.uwaterloo.ca/h44ding/cs452_k2.git --branch renhang
```
## build
```bash
cd cs452_k2
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
- Multiple Producers Single Consumer (MPSC)
Both name server and RPS server use MPSC since MPSC suits their server-client pattern. 

For the name server, it acts like a single consumer that keeps consuming things sent from producers (clients) and process them from there. The name clients then acts like multiple producers that produces name queries to the server.

Similarly, the RPS server acts like a single consumer that keeps consuming queries sent from producers, and the RPS clients are producers that produce game actions and send those actions to the server.

- Hash Table
The name server uses a hash table to map names to Tids. From the reference http://www.cse.yorku.ca/~oz/hash.html a djb2 hash function is used by our hash table, which hashes a string to an unsigned word.

- Memory Pool
For simplicity, we have a simple memory pool for simple dynamic memory allocation. It preallocates a fixed-size array as the data pool, and it defines a fixed data chunk size. Every time we apply for some new memory, we find a non-used memory chunk in the data pool and give it to the user. Then set that chunk as used. When freeing an address, we simply set that corresponding chunk as non-used. The drawback of this method is that we cannot allocate a memory size greater than the predefined chunk size, and it takes linear time to find a free chunk in the pool. However, it gets things done easily.

- Queues
  - RPS queue
  We use a queue for storing signing up players. Whenever there are more than 2 players, we pop two as a pair.
  - message passing
  For every receiver there is a queue dedicated for it to get data from the senders. Basically all the data sent from the senders will be pushed into the corresponding receiver's queue, and the receiver will then be able to get the data from the queue. The queue is designed to be as a circular queue backed up by a fix-sized array. The fix-sized array makes the queue access faster than a queue implemented as a linked list.

## Limitations
One round of the send-receive-reply takes a relatively long time. The most time consuming part is the scheduler. Since we haven't implemented a proper blocked queue, whenever we schedule one task, the scheduler need to scan through some blocked tasks, which makes the scheduling slow. We plan to re-implement the scheduler on the next week to make it faster.
## System Parameters
- Our kernel can have 64 tasks with 10 priority levels at maximum, which in our opinion is sufficent.
- Therefore, the message queue for each task has the maximum length of 63
## Bugs
In the rps server, after calling RegisterAs the control flow will not be able to get back to where it should resume. The return address x30 becomes an invalid value which causes an unsupported exception. A simple Send Receive Reply can be correctly executed without this issue but a complicated scenario involving the name server and rps server will cause such issue. We might require more external assistance for resolving this issue. It is difficult to keep track of the x30 value as printing the register in assembly caused other bugs. We tried to store the registers onto a certain part of the memory but when we tried to access it later those registers do not seem to be the actual ones we need to know.
# Output Descriptions
RPS game output:
The output log is put in rps_log.txt in the root directory.
In the log file, everything before line 82 is executed correctly. 
In the properly executed code, the following things happens in right order
  - the initial program creates the name server 
  - the name server is scheduled
  - the name server begins to wait to receive message, thus blocked
  - the initial program creates the rps server
  - the rps server is scheduled
  - the rps begin to send message (aka. CALL RegisterAs) to name server, thus blocked
  - the name server is resumed, and received the message from rps server corretly
  - the name server replies the rps server, and data are correctly passed to rps server
  - the name server begins to wait to receive message again, thus blocked
  - the rps server is resumed, and received the reply from name server
  - the rps is going to finish its RegisterAs CALL
  
After the prevois step, the program goes wrong as explained below:
  - the link address(x30) of the rps server in RegisterAs got wrong
  - the rps cannot return from the RegisterAs call, but it returns to an invalid address instead
  - the invalid address exception cannot be handled by the current exception handlers
  - the rps task is put into task queue and resumed again
  
  - the rps cannot return from the RegisterAs call, but it returns to an invalid address instead
  - the invalid address exception cannot be handled by the current exception handlers
  - the rps task is put into task queue and resumed again
  - ... infinite loop between the prevois 3 steps


# Performance Measurement Methodologies
In the receiver first case, we take the begin time as the time the receiver calls Receive, and the end time as the time the sender receives the reply.
In the sender first case, we take the begin time as the time the sender calls Send, and the end time as the time the sender receives the reply.

To remove the time that is taken by the get_time function itself, we call the get_time functions twice in a row, and compute the difference between the two time. We then substract the orinially measured time by this difference. This result is the final time used for one send-receive-reply.

And we take multiple (up to 5) experiments from each case, and compute the average time.