- [operating systems](#operating-systems)
  - [links](#links)
  - [introduction](#introduction)
  - [processes](#processes)
  - [memory](#memory)
  - [concurrency](#concurrency)
  - [I/O and filesystems](#io-and-filesystems)

# operating systems

## links

- [homepage](https://www.cse.iitb.ac.in/~mythili/os/)
- [lectures](https://www.youtube.com/playlist?list=PLDW872573QAb4bj0URobvQTD41IV6gRkx)

## introduction

**process:** running program

**operating system:** middleware between user programs & system hardware
1. **manage CPU:** process abstraction, each process has illusion of having complete CPU (timesharing, CPU virtualization), coordination between processes
2. **manage memory:** each process thinks it has dedicated memory space (code, data, stack, heap) for itself, translates virtual addresses to actual physical addresses
3. **manage devices:** device driver talks the language of hardware devices, issues instruction to device (data fetch), responds to device interrupts (key press), persistent data organized as filesystem on disk

**design goals:**
1. convenience, abstraction of hardware resources
2. efficient use of CPU, memory, etc
3. isolation between multiple processes

**system call:** function call that runs at higher privilege level of CPU (procedure call, normal function call), sensitive operations allowed only at higher privilege level

## processes

**CPU scheduler:** schedule CPU requests of processes
1. **policy:** which process to run next
2. **mechanism:** how to context-switch between processes

**process constituents:**
1. unique identifier (PID)
2. memory: static (code, data) & dynamic (stack, heap)
3. CPU context: program counter (PC), current operands, stack pointer (SP)
4. file descriptors: pointers to open files & devices

**process creation:**
1. allocate memory and create memory image (load code & data, create runtime stack & heap)
2. opens basic files (stdin, stdout, stderr)
3. initialize CPU registers (PC pointers to first instruction)

**process states:**
1. **running:** currently exxecuting on CPU
2. **ready:** waiting to be scheduled
3. **blocked:** suspended, not ready to run (waiting for some event)
4. **new:** being created, yet to run
5. **dead:** terminated

![](media/operating_systems/process_states.png)

OS maintains a data structure (list) of all active processes

**process control block:** stores information about a process
1. process identifier
2. process state
3. pointer to related processes (parent, child)
4. CPU context (saved when process is suspended)
5. pointers to memory locations
6. pointers to open files

**process application programming interface (API):** set of system calls provided by OS, blocking system calls (disk read) can cause process to be blocked

**Portable Operating System Interface (POSIX) API:** standard set of system calls that OS must implement, ensures program portability, programming language libraries hide details of invoking system calls
1. **`fork()`:** create new child process, all processes created by forking from a parent (`init()` process is ancestor of all processes)
2. **`exec()`:** make process execute a given executable
3. **`exit()`:** terminate a process
4. **`wait()`:** causes parent to block until child terminates

**`fork()` working:**
1. new process created by making a copy of parent's memory image
2. new process is added to OS process list and scheduled
3. parent & child start execution just after fork (with different return values)
4. parent & child execute and modify the memory data independently
```cpp
int main(int argc, char *argv[])
{
    int ret = fork();

    if (ret < 0) { /* fork failed */ }

    else if (ret == 0) { /* child process */ }

    else { /* parent process, ret is child process PID */ }
}
```

**child process handling:**
1. process terminates
   1. by calling `exit()` (automatically called at the end of main)
   2. OS terminates a misbehaving process
2. terminated process exists as a zombie
3. when parent called `wait()`, zombie child is cleaned up (reaped), `wait()` blocks in parent until child terminates (non-blocking wait also exist)
4. if parent terminates before child, init process adopts orphans and reaps them

**`exec()` working:**
1. load another executable into child memory image, so child can run different program from parent
2. variants of exec where arguments to new executable passed

**example: shell working:**
1. init process created after initialization of hardware
2. init process spawns a shell (like bash)
3. shell reads user command ⟶ forks a child ⟶ execs command executable ⟶ waits for it to finish ⟶ reads next command
4. common commands like ls are all executables that are exec'ed by the shell
5. `ls > foo.txt` shell rewires stdout to file before calling exec

**function call working:**
1. translates to jump instruction
2. new stack frame pushed and SP updated, stack frame contains arguments, return value, etc
3. function instructions executed
4. stack frame popped
5. PC now holding return address

**system call difference:**
1. CPU hardware has multiple priviledge levels - user mode (user code) & kernel mode (system calls), some instructions execute only in kernel mode
2. kernel does not trust user stack, uses seperate kernel stack in kernel mode
3. kernel does not trust user provided addresses to jump to, interrupt descriptor table (IDT) has addresses of kernel functions to run for system calls & other events (set up at boot time)

**system call working:**
1. when system call is made, special trap instruction is run (hidden from user by libc)
2. this will move CPU to higher priviledge level, switch to kernel stack, save user context on kernel stack, look up IDT and jump to trap handler function in OS code
3. trap usecases: system call (program needs OS service), program fault (program does something illegal) & interrupt (external device needs attention of OS)
4. number stored in a CPU register before calling trap to identify which IDT entry to use
5. when OS is done, it calls special return-from-trap instruction
6. this will restore user context on kernel stack, change CPU priviledge from kernel mode to user mode, restore PC and jump to user code after trap
7. before returning to user mode, OS checks if it must switch to different process (context switch), why - process exited or terminated (program fault), process made blocking system call (waiting for disk data), process has run for too long (CPU timesharing)

**CPU scheduler mechanism types:**
1. **non-preemptive (cooperative):** switch only if process blocked or terminated
2. **preemptive (non-cooperative):** switch even when process is ready to continue, CPU generated periodic timer interrupt, after servicing interrupt OS checks if current process has run for too long

**CPU scheduler mechanism working:**
1. process A has moved from use to kernel mode, OS has decided it must switch from process A to B
2. save kernel context of A on kernel stack
3. swicth SP to kernel stack of B
4. restore kernel context from B's kernel stack (stored when B was switched out)
5. now CPU running B in kernel mode, return-from-trap to user mode of B

**user context:** when going from user mode to kernel mode, saved on kernel stack by trap instruction  
**kernel context:** during context switch, saved on kernel stack by context switching code

**CPU scheduler policy goals:**
1. maximize utilization: fraction of time CPU is used
2. minimize average turnaround time: time from process arrival to completion
3. minimize average response time: time from process arrival to first scheduling
4. fairness: all processes must be treated equally
5. minimize overhead: run process long enough to reduce cost of context switch (~1ms)

**CPU scheduler policies:** on context switch which process to run next, based on CPU time used by a process in a continuous stretch (CPU burst)
1. **first come first serve (FCFS):** process job in the order they are received, convoy effect: stuck behind long process so high turnaround time
2. **shortest job first (SJF):** process job with shorted execution time, non-preemptive so can stil get stuck if long process executing, bad response time
3. **shortest remaining time first (SRTF):** preemptive version of SJF (preempts running job), process job closest to completion
4. **round robin (RR):** each job processed for a fixed time slice, preemptive, slice big enough to reduce context switch cost, good for response time & fairness, bad for turnaround time
5. **multi lebel feedback queue (MLFQ):** used in linux, many queues in order of priority, process from highest prioriy queue, within same priority any algorithm like RR

**inter-process communication (IPC):** mechanisms to share information between processes, processes do not share any memory with each other
1. **shared memory:** both processes can access same region of memory, via `int shmget(key,size, shmflg)` system call, by providing same key two processes can get same segment of memory, can read/write memory to communicate
2. **signals:** can be sent to a process by OS or another process, some signals have fixed meaning (`ctrl + C` send `SIGINT` signal), every process has a default code to execute for each signal (signal handler) like exit on terminate signal, some signal handlers can be overriden to do other things
3. **sockets:** can be used for two processes on same machine (Unix sockets) or different machine (TCP/UDP sockets) to communicate, processes open sockets and connect them to each other, messages written into one socket can be read from another, OS transfers data across socket buffers
4. **pipes:** one-way communication (half-duplex), pipe system call returns write handle & read handle file descriptors, data written in one file descriptor can be read through another file descriptor, pipe data buffered in OS buffers between read & write
   1. **regular pipes:** both file descriptor are in same process, parent & child share file descriptor after fork, parent uses one end and child uses other end
   2. **named pipes:** two endpoints in different processes
5. **message queue:** process can open mailbox at a specified location, processes can send/receive messages from mailbox, OS buffers messages between send & receive

**blocking vs non-blocking communication:** system calls to read/write have versions that block or can return with error code in case of failure, like reading from empty socket/pipe/message queue or writing to full socket/pipe/message queue

## memory

![](media/operating_systems/actual_memory.png)

**virtual address space:** every process assumes it has access to large contiguous space of memory from address 0 to MAX, to hide complexity of multiple processes non-contiguously sharing memory, contains program code, heap & stack (heap & stack grow runtime), setup during process creation

![](media/operating_systems/virtual_memory.png)

**address translation:** CPU issues load/store to virtual addresses (VA) but memory hardware accesses physical addresses (PA), OS allocates memory and tracks location of processes, translation (VA to PA) done by memory management unit (MMU) with necessary information from OS

**paging:** OS divides virtual address space into fixed size pages and physical memory into frames, to allocate memory a page is mapped to free physical frame, page table stores mapping from virtual page number to physical frame number for a process

**memory virtualization goals:**
1. **transparency:** user should not be aware of messy details
2. **efficiency:** minimize overhead & wastage in terms of memory space & access time
3. **isolation & protection:** user should not be able to access anything outside its address space

**memory allocation system calls:**
1. `malloc()` implemented by libc using `brk()`/`sbrk()` system call
2. program can use `mmap()` to allocate page sized memory, gets anonymous page from OS

**OS address space:** OS is not a seperate process with its own address space, instead OS code is part of the address space of every process, page table maps OS addresses to OS code

**address translation in simplified OS:** places entire memory image in one chunk, OS tells MMU the base (starting address) & bound (total size of process) values (needs priviledged mode), MMU calculates PA from VA, MMU also checks if address is beyond bound, generates faults and traps to OS if access illegal (VA out of bound), OS updates translation information upon context switch
```
PA = VA + base
assert (PA < VA + base + bound)
```

**segmentation:** generalized base & bounds, each segment of memory image placed seperately, multiple base & bound values stored in MMU, good for sparse address space, but variable sized allocation leads to external framentation due to small holes in memory left between segments

![](media/operating_systems/segmentation.png)

**internal fragmentation:** unused part of memory block assigned to a process cannot be used by other processes, can be solved using dynamic partitioning to allocate space to process

**external fragmentation:** total memory space is enough to satisfy a request or to reside a process in it but it is not contiguous so it cannot be used

![](media/operating_systems/fragmentation.png)

**paging:** allocate memory in fixed size chunks (pages), avoids external fragmentation but has internal fragmentation

**page table:** per process data structure to help VA-PA translation, array stores mappings from virtual page number (VPN) to physical frame number (PFN), part of OS memory (in PCB), MMU has access to page table and uses it for address translation, OS updates page table upon context switch

**page table entry (PTE):** simplest page table is linear page table, page table is array of page table entries (one per virtual page), VPN is index in the array, each PTE contains PFN & few other bits like 
- **valid bit:** is this page used by process?
- **protection bits:** read/write permissions
- **present bit:** is this page in memory?
- **dirty bit:** has this page been modified?
- **accessed bit:** has this page been recently accessed?

**address translation in hardware:** most significant bits of VA give the VPN, page table maps VPN to PFN, PA is obtained from PFN and offset within a page, MMU stores physical address of start of page table then walks the page table to get relevant PTE

![](media/operating_systems/address_translation.png)

**translation lookaside buffer (TLB):** cache of VA-PA mappings, MMU first looks up TLB, if TLB miss then MMU performs additional memory accesses to walk page table, TLB misses are expensive (multiple memory accesses), TLB entries may become invalid on context switch and change of page tables

**page tables in memory:** with 32 bit VA × 4KB pages × 4 bytes PTE each page table is 4MB, one page table per process, so page table is itself split into smaller chunks

**multilevel page tables:** page table is spread over many pages, an outer page table (page directory) tracks the PFNs of the page table pages, depending on how large the page table is we may need more than 2 levels also, for address translation first few bits of VA to identify outer page table entry, next few bits to index next level of PTEs, in case of TLB miss multiple accesses to memory required to access all levels of page tables

![](media/operating_systems/linear_vs_multilevel_pagetable.png)

![](media/operating_systems/multilevel_pagetable_address.png)

**demand paging:** main memory not always enough to store all the pages of all active processes, OS uses a part of disk (swap space) to store pages that are not in active use

![](media/operating_systems/swap_space.png)

**page fault:** if page not in main memory (present bit) during VA-PA translation MMU raises trap to OS, MMU cannot access the disk

**page fault handling:** moves CPU to kernel mode, OS fetches disk address of page and issues read to disk, disk fetch is slow so OS context switches to another process, once disk read completes OS updates page table of process and marks process as ready, when process scheduled again OS restarts the instruction that caused page fault

**page replacement policies:** when servicing page fault if OS finds no free page then OS must swap out an existing page and then swap in faulting page, to prevent this much work OS proactively swap out pages to keep list of free pages handy
1. **optimal:** replace page not needed for longest time in future, just theoretical, cannot calculate when page is needed
2. **first in first out (FIFO):** replace page that was brought into memory earliest (maybe a popular page)
3. **least recently/frequently used (LRU/LFU):** replace the page that was least recently (or frequently) used in the past, works well due to locality of references, OS periodically looks at accessed bit in PTE (set by MMU) to estimate pages that are active and inactive

**cold miss:** miss occur when the first access to a page happens

**belady's anomaly:** increasing number of page frames results in increase in number of page faults

**locality of references:** tendency of computer progam to access instructions whose address are near one another
1. **temporal locality:** same location will be referenced again in the near future
2. **spatial locality:** nearby memory locations will be referenced in the near future

**variable sized allocation**: given a memory block how to allocate it to satisfy various memory allocation requests, used by C library for `malloc` & kernel for internal data structures

**headers:** every allocated chunk has a header containing size of allocated region, may contain magic number for additional integrity checking

**free list:** free space managed as a linked list, pointer to the next free chunk is embedded within current free chunk, library/kernel tracks the head of the list, allocation happens from the head, must split & coalesce free chunks to satisfy variable sized requests (external fragmentation)

**splitting:** on a allocation request allocator will find a free chunk of memory that can satisfy the request and split it into two, first chunk returned to called & second chunk will remain on the free list

**coalescing:** on a free request allocator will check if free chunk of memory being returned sits next to another free chunk, if yes then merge them into a single larger free chunk

**example: non coalesced free list:** suppose three allocations of 100 bytes each happen, middle chunk freed first then third chunk then first chunk

![](media/operating_systems/non_coalesced_free_list.png)

**buddy allocation**: allocate memory in size of power of 2, easy coalescing, two adjacent chunks can be merged to form a bigger power of 2 chunk

![](media/operating_systems/buddy_allocation.png)

**variable size allocation strategies:**
1. **first fit:** allocate first free chunk that is sufficient
2. **best fit:** allocate free chunk that is closest in size
3. **worst fit:** allocate free chunk that is farthest in size, remaining chunk is bigger & more usable

**fixed size allocations:** memory allocation algorithms are much simpler with fixed size allocations
1. **page-sized allocations:** kernel has free list of pages, pointer to next page stored in the free page itself
2. **slab allocator:** used by kernel for small allocations like PCB, object caches for each type (size) of objects, within each cache only fixed size allocation, each cache made up of one or more slabs

![](media/operating_systems/slab_allocator.png)

## concurrency

**thread:** is like another copy of a process that executes independently, threads share the same address space (code & heap), each thread has seperate PC and stack for independent function calls

![](media/operating_systems/single_vs_multi_threaded.png)

**process vs thread:**
- **parent forks a child:** P & C do not share any memory, needs IPC mechanisms to communicate, extra copies of code & data in memory
- **parent executes two threads:** T1 & T2 share parts of address space, global variables can be used for communication, smaller memory footprint

**concurrency:** running multiple threads/processes at same time by interleaving their execution (even on single CPU core)

**parallelism:** running multiple threads/processes in parallel over different CPU cores

**why threads:** single process can effectively utilize multiple CPU cores, even if no parallelism concurrency of threads ensures effective use of CPU when one of the threads blocked

**scheduling threads:** OS schedules threads that are ready to run independently, context of a thread (PC, registers) is saved into/restored from thread control block (TCB), every PCB has one or more linked TCBs

**kernel threads:** threads that are schedules independently by kernel, example - linux pthreads

**user-level threads:** library multiplexes large number of user threads over a smaller number of kernel threads, low overhead of switching since no expensive context switch, but multiple user threads cannot run in parallel

**example: thread creation:**
```cpp
#include <assert.h>
#include <pthread.h>
#include <stdio.h>

void *mythread(void *arg)
{
    printf("%s\n", (char *)arg);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t p1, p2;
    int rc;
    printf("main: begin\n");
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: end\n");
    return 0;
}
```

**race condition:** when two or more threads can access shared data and try to change it at the same time, concurrent execution can lead to different results

**critical section:** portion of code that can lead to race conditions

**mutual exclusion:** only one thread should be executing critical section at any time

**atomicity:** critical section should execute like one uninterrupible instruction

**lock:** is just a variable that is either available (no thread holds the lock) or acquired (one thread holds the lock, other threads waiting), makes sure only one thread is executing critical section

**goals of lock implementation:**
1. mutual exclusion
2. fairness: all threads should eventually get the lock, no thread should starve
3. low overhead: acquiring, releasing & waiting for lock should not consume too many resources

**disabling interrupts:** this technique is used to implement locks on single processor systems inside the OS, disabling interrupt is a privileged instruction and program can misuse it (example - run forever), will not work on multiprocessor systems since another thread on another core can enter critical section

**example: lock implementation using flag variable:** spin on a flag variable until it is unset, then set it to acquire lock, reset flag variable once done, race condition has moved to lock acquisition code (example - interrupt after loop but before set flag)

**hardware atomic instructions:** very hard to ensure atomicity only in software, modern architectures provide hardware atomic instructions
1. **test-and-set:** update a variable and return old value all in one hardware instruction
```cpp
int test_and_set(int *old_ptr, int new)
{
    int old = *old_ptr;  // fetch old value at old_ptr
    *old_ptr = new;      // store new into old_ptr
    return old;          // return the old value
}
```
2. **compare-and-swap:** update a variable only if equal to expected and return actual value all in one hardware instruction
```cpp
int compare_and_swap(int *ptr, int expected, int new)
{
    int actual = *ptr;
    if (actual == expected)
        *ptr = new;
    return actual;
}
```

**spinlock:** spin until lock is acquired
```cpp
while (test_and_set(&lock->flag, 1) == 1)
while (compare_and_swap(&lock->flag, 0, 1) == 1)
```

**sleeping mutex:** a contending thread could simply give up the CPU and check back later instead of spinning for a lock, `yield` moves thread from running to ready state
```cpp
void lock()
{
    while (test_and_set(&lock->flag, 1) == 1) 
        yield()  // give up the CPU
}
```

**spinlock vs sleeping mutex:** most userspace lock implementations are sleeping mutex since CPU wasted by spinning contending threads, OS uses spinlocks since OS is default software layer and has no other thread to yield to, OS must disable interrupt while lock is held since an interrupt handler could request same lock leading to deadlock, OS must never go to sleep with a locked spinlock (perform blocked operation)

**conditional variables:** is a queue that a thread can put itself into when waiting on some condition, another thread that makes the condition true can signal the conditional variable to wake up a waiting thread, signal wakes up one thread & broadcast wakes up all waiting threads

**example: parent waits for child:** lock must be held when calling wait & signal with conditional variable, wait function releases the lock before putting thread to sleep
```cpp
int done = 0;
pthread_mutex_t m = pthread_MUTEX_INITIALIZER;
pthread_cond_t c = pthread_COND_INITIALIZER;

void thr_exit()
{
    pthread_mutex_lock(&m);
    done = 1;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}

void *child(void *arg)
{
    printf("child\n");
    thr_exit();
    return NULL;
}

void thr_join()
{
    // without mutex race condition, parent checks condition then interrupted
    //child sets to 0 and signals but no one sleeping yet
    // parent resumes now and goes to sleep forver
    pthread_mutex_lock(&m);
    // wait only if child not run
    while (done == 0)  // don't use if, corner case condition is false when wait returns
        pthread_cond_wait(&c, &m);
    pthread_mutex_unlock(&m);
}

int main(int argc, char *argv[])
{
    printf("parent: begin\n");
    pthread_t p;
    pthread_create(&p, NULL, child, NULL);
    thr_join();
    printf("parent: end\n");
    return 0;
}
```

**example: producer/consumer problem:** one or more producer threads, one or more consumer threads, a shared buffer of bounded size
```cpp
cond_t empty, fill;
mutex_t mutex;

void *producer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        pthread_mutex_lock(&mutex);
        while (count == MAX)
            pthread_cond_wait(&empty, &mutex);  // 
        put(i);
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        pthread_mutex_lock(&mutex);
        while (count == 0)
            pthread_cond_wait(&fill, &mutex);
        int tmp = get();
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
        printf("%d\n", tmp);
    }
}
```

**semaphores:** variable with an underlying counter, two functions - up/post increments counter and down/wait decrements counter and blocks calling thread if resulting value negative, binary semaphore (initial value 1) acts as a simple mutex lock

**example: semaphore parent child:**
```cpp
sem_t s;

void *child(void *arg)
{
    printf("child\n");
    sem_post(&s);  // signal here: child is done
    return NULL;
}

int main(int argc, char *argv[])
{
    sem_init(&s, 0, X); // initial value X = 0
    printf("parent: begin\n");
    pthread_t c;
    pthread_create(&c, NULL, child, NULL);
    sem_wait(&s);  // wait here for child
    printf("parent: end\n");
    return 0;
}
```

**example: semaphore producer/consumer problem:** need two semaphore for signalling full & empty, need one semaphore to act as mutex for buffer
```cpp
sem_t empty;
sem_t full;
sem_t mutex;

void *producer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        // if mutex before wait, waiting thread sleeps with mutex
        // signalling thread cannot proceed and wake it up
        sem_wait(&empty);
        sem_wait(&mutex);
        put(i);
        sem_post(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        sem_wait(&full);
        sem_wait(&mutex);
        int tmp = get();
        sem_post(&mutex);
        sem_post(&empty);
        printf("%d\n", tmp);
    }
}

int main(int argc, char *argv[])
{
    // ...
    sem_init(&empty, 0, MAX);  // MAX buffers are empty
    sem_init(&full, 0, 0);     // 0 buffers are full
    sem_init(&mutex, 0, 1);    // binary semaphore
    // ...
}
```

**concurrency bugs:** are non-deterministic and occur based on execution order of threads, very hard to debug
1. **non-deadlock bugs:** non-deadlock but incorrect results when threads execute
   1. **atomicity bugs:** atomicity assumptions made by programmer are violated during execution of concurrent threads, fix - locks for mutual exclusion
   2. **order-violation bugs:** desired order of memory access if flipped during concurrent execution, fix - conditional variables
2. **deadlocks:** threads cannot execute any further and wait for each other, all four of below conditions must hold for a deadlock to occur
   1. **mutual exclusion:** a thread claims exclusive control of a resource
   2. **hold-and-wait:** thread holds a resource and is waiting for another, to prevent acquire all locks at once by acquiring a master lock first
   3. **no preemption:** thread cannot be made to give up its resource
   4. **circular wait:** there exists a cycle in the resource dependency graph, to prevent always acquire locks in certain fixed order, total ordering (or partial ordering on related locks) must be followed

**example: atomicity bug:** one threads reads & prints a shared data item while another concurrently modifies it
``` cpp
// thread 1
if (thd->proc_info)
{
    // ...
    fputs(thd->proc_info, ...);
    // ...
}

// thread 2
thd->proc_info = NULL;
```

**example: order-violation bug:** thread1 assumes thread2 has already run
```cpp
// thread 1
void init()
{
    // ... 
    m_thread = createthread(m_main, ...);
    // ...
}

// thread 2
void m_main(...)
{
    // ...
    m_state = m_thread->state;
    // ...
}
```

**example: deadlock:** thread1 holds lock1 and is waiting for lock2, thread2 holds lock2 and is waiting for lock1
```cpp
// thread 1
pthread_mutex_lock(l1);
pthread_mutex_lock(l2);

// thread 2
pthread_mutex_lock(l2);
pthread_mutex_lock(l1);
```

![](media/operating_systems/deadlock_dependency.png)

**example: cicular wait prevention - total ordering:**
```cpp
// grab locks in high-to-low address order
// code assumes m1 != m2
if (m1 > m2)
{
    pthread_mutex_lock(m1);
    pthread_mutex_lock(m2);
}
else
{
    pthread_mutex_lock(m2);
    pthread_mutex_lock(m1);
}
```

**example: hold-and-wait prevention - master lock:**
```cpp
pthread_mutex_lock(prevention);  // begin lock acquisition
pthread_mutex_lock(l1);
pthread_mutex_lock(l2);
// ...
pthread_mutex_unlock(prevention);  // end
```

**other solutions to deadlocks:**
1. **deadlock avoidance:** if OS knew which process needs which locks it can schedule the processes in that deadlock will not occur
2. **detect and recover:** reboot system or kill deadlocked processes

## I/O and filesystems

![](media/operating_systems/system_architecture.png)

**port:** point of connection to the system, I/O devices connect to the CPU & memory via a bus to a port on the machine

**device types:** devices expose an interface of memory registers (current status of device, command to excute & data to transfer), internals of device usually hidden
1. **block devices:** store a set of numbered blocks (disks)
2. **character devices:** produce/consume stream of bytes (keyboard)

**OS read/write to registers:**
1. **explicit I/O instructions:** priviledged instruction used by OS to read & write to specific registers on device, example - on x86 `in` & `out`
2. **memory mapped I/O:** device makes registers appear like memory locations, OS simply reads/writes from memory (part of address space reserved for I/O devices), memory hardware routes accesses to these special memoru addresses to devices

**example: simple execution of I/O requests:** polling status to see if device ready wastes CPU cycles, CPU explicitly copies data to/from device (programmed I/O)
```cpp
while (STATUS == BUSY)
    ;  // wait until device is not busy
write data to DATA register;
write command to COMMAND register;  // starts the device and executes the command
while (STATUS == BUSY)
    ;  // wait until device is done with your request

```

**interrupt (IRQ):** polling wastes CPU cycles, instead OS can put process to sleep and switch to another process, when I/O request completes device raises interrupt

**interrupt handler:** interrupt switches process to kernel mode, interrupt descriptor table (IDT) stores pointers to interrupt handlers (interrupt service routine (ISR)), IRQ number identifies the interrupt handler to run for a device, interrupt handler acts upon device notification and unblocks the process waiting for I/O (if any) then starts next I/O request (if any pending), handling interrupts imposes kernel mode transition overheads (polling may be faster than interrupts if device is fast)

![](media/operating_systems/interrupt_handler.png)

**direct memory access (DMA):** cpu cycles wasted in copying data to/from device, instead special piece of hardware (DMA engine) copied from main memory to device, CPU gives DMA engine the memory location, size & destination of data, interrupt raised after DMA completes

![](media/operating_systems/direct_memory_access.png)

**device driver:** part of OS code that talks to specific device, gives commands and handles interrupts, most OS code abstracts the device details, example - file system code is written on top of generic block device

![](media/operating_systems/device_driver.png)

**file:** linear array of bytes, stored persistently, identified with filename (human readable) and a OS level identifier (index node (inode) number), inode number unoque within a filesystem

**directory:** contains other subdirectories and files along with their inode numbers, stored like a file whose contents are filename-to-inode mappings

**directory tree:** files & directories arranged in a tree starting with root (/)

![](media/operating_systems/directory_tree.png)

**file operations:**
1. **creating** `open` system call with flag to create, returns a numbers called file descriptor (fd)
2. **opening:** existing files must be opened before they can be read/written, also uses `open` system call and returns fd, all further operations on files uses the fd
3. **closing:** `close` system call closes the file
4. **read/write:** `read`/`write` system calls, arguments - fd, buffer with data, size, reading/writing happens sequentially by default, successive read/write calls fetch from current offset, `lseek` system call to seek to random offset, writes are buffered in memory temporarily so `fsync` system call flushes all writes to disk
5. **other operations:** rename file delete (unlink) file, get statistics of a file

**directory operations:** directory can also be accessed like files, operations like create, open, read & close

**example: `ls`:** `ls` progeam opens and reads all directory entries
```cpp
int main(int argc, char *argv[])
{
    DIR *dp = opendir(".");
    assert(dp != NULL);
    struct dirent *d;
    while ((d = readdir(dp)) != NULL)
    {
        printf("%lu %s\n", (unsigned long)d->d_ino, d->d_name);
    }
    closedir(dp);
    return 0;
}
```

**hard link:** creates another file that points to the same inode number and hence same underslying data, if one file deleted - file data can be accessed through the other links, inode maintains a link count, file data deleted only when no further links to it, you can only unlink OS decides when to delete
```shell
$ ls -i file file2
67158084 file
67158084 file2
```

**soft/symbolic link:** is a file that simply stores a pointer to another filename, if main file is deleted then the link points to invalid entry (dangling reference)
```shell
$ ls -al
drwxr-x--- 2 remzi remzi 29 May 3 19:10 ./
drwxr-x--- 27 remzi remzi 4096 May 3 15:14 ../
-rw-r----- 1 remzi remzi 6 May 3 19:10 file
lrwxrwxrwx 1 remzi remzi 4 May 3 19:10 file2 -> file
```

**mounting a file system:** mounting a file system connects the files to a specific point in the directory tree, several devices & file systems are mounted on a typical machine

**memory mapping a file:** alternate way of accessing a file instead of using fd & read/write syscalls, `mmap` allocates a page in virtual address space of a process, when file is mmap - file data copied into one or more pages in memory, can be accessed like any other memory location in program
1. **anonymous page:** used to store program data
2. **file-backed page:** contains data of file, filename provided as arg to `mmap`

**file system:** an organization of files & directories on disk, OS has one or more file systems, disk exposes set of blocks (usually 512 bytes), file system organizes files onto blocks, two main aspects of file systems are
1. data structures to organize data & metadata on disk
2. implementation of system calls like open, read, write using data structures

**file system organization:**
1. **data blocks:** file data stored in one or more blocks
2. **inode blocks:** each block has one or more inodes, metadata about every file stored in inode
3. **bitmaps:** indicate which inodes/data blocks are free, `i` & `d` in picture
4. **superblock:** holds master plan of all other blocks, example - which are inodes, which are data blocks, `S` in picture

![](media/operating_systems/file_system_organization.png)

**inode table:** inodes usually stored in array, inode number of a file is index into this array, inode stores
1. file metadata like permissions, access time
2. pointers (disk block numbers) of file data

![](media/operating_systems/inode_table.png)

**inode structure:** file data not stored contiguously on disk so need to track multiple block numbers of file, inode tracks disk block bumbers using
1. **direct pointers:** numbers of first few blocks are stored in inode itself, suffices for small files
2. **indirect block:** for larger files inode stores number of indirect blocks which has block numbers of file data
3. similarly double & triple indirect blocks (multi-level index)

**file allocation table (FAT):** alternate wat to track file blocks, FAT stores next block pointer for each block, each disk block has one entry with number of next file block (or null if last block), pointer to first block stored in inode

**directory structure:** directory stores records mapping filename to inode number, directory is a special type of file and has inode & data blocks (which store the file records)

**free space management:**
1. **bitmap:** store one bit per block to indicate if free or not
2. **free list:** super block stores pointer to first free block which inturn stores address of next free block

**opening a file:** to have the inode readily available in mmeory for future operations on file
1. pathname of file is traversed starting from root, inode of root is known
2. recursively do - fetch inode of parent directory ⟶ read its data blocks ⟶ get inode number of each child ⟶ fetch inode of child, repeat till end of path
3. if new file - new inode & data blocks will have to be allocated using bitmap and directory entry updated

**open file table:**
1. **global open file table:** one entry for every file opened (even sockets & pipes), entry points to in-memory copy of inode
2. **per-process open file table:** array of files opened by process, fd is index into this array, per-process table entry points to global open file table entry, every process has three files open by default - std in, std out & std err (fd 0, 1 & 2), open system call creates entries in both tables and returns fd

**reading/writing a file:**
1. access in-memory inode via fd
2. find location of data block at current read/write offset
3. fetch data from disk and perform operation
4. writes may need to allocate new blocks from disk using bitmap of free blocks
5. update time of access & other metadata in inode

**virtual file system (VFS):** file systems differ in implementations of data structures, hence linux supports VFS abstraction, VFS looks a file systems as objects (files, directories, inodes, superblock) and operationf on these objects (lookup filename in directory), system call logic is written on VFS objects, to develop a new file system simply implement functions on VFS objects and provide pointers to these functions to kernel, syscall implementation does not have to change with file system implementation details

**disk buffer cache:** results of recently fetched disk blocks are cached, file system issues block read/write requests via buffer cache, served from cache id block in cache, else block fetched to cache and returned to file system, free pages allocated to both processes & disk buffer cache from common pool (unified page cache in OS), writes are applied either
1. **synchronously (write-through):** cache writes to disk immediately
2. **asynchronously (write-back):** cache stores dirty block in memory and writes back after delay

**disk buffer benefits:**
1. improved performance due to reduced disk I/O
2. single copy of block in memory, so no inconsistency across processes

**hard disk internals:** a set of 512 byte blocks (sectors) that can be read/written atomically, one or more platters connected by a spindle spin at ~10K rpm, each plater has a disk head & arm attached to it, a platter is divided into multiple tracks and each track into 512 byte sectors

**hard disk sector access:** seek to correct track while waiting for disk to rotate, example - sector 30 to 11

![](media/operating_systems/hard_disk_sector_access.png)

**time taken for I/O operation:** given high seek & rotational latency usually rate of sequential access > rate of random access
1. seek time to get to right track (few ms)
2. rotational latency for disk to spin to correct sector (few ms)
3. data transfer time to read sector (few tens μs)

**disk scheduling:** requests to disk are not served in FIFO, they are reordered with other pending requests in order to read blocks in sequence as far as possible (to minimize seek time & rotational delay), OS does not know internal geometry of disk so scheduling done mostly by disk controller

**shortest seek time first (SSTF):** access block that we can seek to fastest, problem - some requests that are far from current position or head may never get served (starvation), example - from 30 go to 21 before 2

![](media/operating_systems/shortest_seek_time_first.png)

**elevator/scan algorithm:** disk head does one sweep over tracks and serves requests that fall on the path
1. **elevator/scan:** sweep outer to inner then inner to outer
2. **c-scam:** sweep only one direction and circle back to start again, sweeping back & forth favors middle tracks more
3. **f-scan:** freeze queue while scanning, avoid starving far away requests

**shortest positioning time first (SPTF):** considers both seek time & rotational latency, example - better to serve 8 before 16 even though seek time is higher but 16 incurs a much higher rotational latency

![](media/operating_systems/shortest_positioning_time_first.png)

**error detection/correction:** bits stored on disk with some error detection/correction bits, correct random bit flips or detect corruption of data, disk controller or OS can handle some errors (blacklisting certain sectors), if errors cannot be masked user perceives hard disk failures

**redundant array of inexpensive disks (RAID):** provide high reliability & performance by replicating across multiple disks