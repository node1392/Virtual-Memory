# File System

## Goal: 
Manage a file system by first instantiating a file system based on an existing directory structure, and then manipulating the files in the file system. Three primary data structures are implemented in the file system:
1. A directed tree structure representing the hierarchical directory,
2. A linked-list data structure representation of disk space - free and in use, and
3. A linked-list data structure storing disk block addresses for a file.

The program accepts the following parameters at the command prompt:
1. -f [input files storing information on files]
2. -d [input files storing information on directories]
3. -s [disk size]
4. -b [block size]

The program starts by instantiating the file system based on the input files and input
parameters. Once the file system structure has been created, the program accepts the User
Commands defined below until the exit command is issued by the user.

## Manipulating Files and Directories via User Commands

Program accepts the following user commands:
1. cd [directory] - set specified directory as the current directory
2. cd.. - set parent directory as current directory
3. ls - list all files and sub-directories in current directory
4. mkdir [name] - create a new directory in the current directory
5. create [name] - create a new file in the current directory
6. append [name] [bytes] - append a number of bytes to the file
7. remove [name] [bytes] - delete a number of bytes from the file
8. delete [name] - delete the file or directory
9. exit - deallocate data structures and exit program
10. dir - print out directory tree in breadth-first order
11. prfiles - print out all file information
12. prdisk - print out disk space information




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
