Discussion

1.  Discuss what you expected to see vs. what you actually saw.

    If the results do not mirror your expectations / textbooks claims, 
    discuss why.
  
 a. Expected to see LRU is better than CLOCK, and CLOCK is better than FIFO.
    The results matched expectations.
   
 b. Expected to see large page size is better than small page size.

    Results matched expectations except that when the size was very large,
    the number of swaps increased.
    This is because when the page size is very large, swap a page will swap
    out a lot of memory locations.
 
 c. Expected to see pre-paging is better than demand paging.
  
    The results matched expectations except that when the size was very large,
    the number of swaps increased.
    This is because that when the page size is very large, pre-paging will swap
    out more memory locations.
     

2. Discuss the relative complexity of programming each algorithm.

   FIFO: This is most simple algorithm. It has the lowest performance.
   LRU: This is more complex than FIFO. It has the best performance.
   CLOCK: This is more complex than FIFO. It has a better performance than FIFO. 


3. Discuss how the data might have changed if a completely random memory 
   access trace had been supplied
  
   The three algorithms will be close to each other.


sample run:

$ ./VMsimulator plist.txt ptrace.txt 16 CLOCK +
Page Size = 16
Algorithm = CLOCK
Pre-Paging = on
Total Main Memory Pages: 32

pid: 0 total 500 -- tablesize 32
pid: 1 total 250 -- tablesize 16
pid: 2 total 400 -- tablesize 25
pid: 3 total 300 -- tablesize 19
pid: 4 total 600 -- tablesize 38
pid: 5 total 350 -- tablesize 22
pid: 6 total 430 -- tablesize 27
pid: 7 total 650 -- tablesize 41
pid: 8 total 345 -- tablesize 22
pid: 9 total 550 -- tablesize 35

165603

