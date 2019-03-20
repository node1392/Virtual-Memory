# Virtual Memory

## Goal:

To simulate a virtual memory management system by experimenting with different page replacement algorithms. These algorithms include: FIFO (First-in, First-Out), LRU (Least Recently Used) Clock (Clock Page Replacement). The program accepts the following parameters at the command prompt in the order specified:

1. P1. Size of pages (# of memory locations on each page).
2. P2. FIFO, LRU, or Clock for type of page replacement algorithms.
3. P3. flag to turn on/off pre-paging. If pre-paging is not turned on, we use demand paging by default.
   1. +: turn it on
   2. -: turn it off
  
plist - contains the list of programs that we will be loading into main memory. Each line in plist is of the format (pID, Total#MemoryLocation), which specifies the total number of memory locations that each program needs.

ptrace - contains a deterministic series of memory access that emulates a real systems memory usage. Each line in ptrace is of the format (pID, ReferencedMemoryLocation), which specifies the memory location that the program requests.

Main memory in the program will hold 512 memory locations.

## Sample Run:
$ ./VMsimulator plist.txt ptrace.txt [page size] [algorithm] [pre-paging]

$ ./VMsimulator plist.txt ptrace.txt 16 CLOCK +

Page Size = 16

Algorithm = CLOCK

Pre-Paging = on
