
/*
Edrick Ramos
Simulate a virtual memory management system
*/

#include <stdio.h>
#include <stdlib.h>    
#include <string.h>   
#include <ctype.h>

/* 512 memory locations in main memory */
#define MAIN_MEMORY  512

/* assume maximum number of programs = 100 */                                      
#define MAX_PROG  100

/* algorithms */
#define AL_FIFO 0   
#define AL_LRU 1
#define AL_CLOCK 2

/* define the page structure in main memory */
typedef struct tMainPage {
    int pageno;    /* unique page number, -1=empty */
    int pid;       /* process id */
    int R;         /* ref bit set by clock algorithm */    
    
    struct tMainPage* next;  /* use a list to keep all pages in main memory*/
} MainPage;

/* define the entry in page table */
typedef struct tTableEntry {
    int pageno;
    int valid;     /* 1=valid, 0=it is not in the memory */
    unsigned long lastAccess;   /* last access time */
} TableEntry;

/* define the program structure */
typedef struct tProgram {
    TableEntry* table;  /* array of table entries*/
    int tableSize;      /* table size */
    
    int pid;            /* program id */                                          
    int totalMemory;    /* total number of memory location */
    
    MainPage* pclk;     /* clock pointer for each program, 
                           because we use local page allocation policy,
                           each program will have a different pointer */
} Program;

/* define the simulator structure */
typedef struct tSimulator {
    Program* prog[MAX_PROG];   /* programs */
    int numProgs;              /* number of programs */
    
    int numMainPages;           /* total pages in main memory */
    MainPage* emptyPages;      /* linked list for empty pages */
    
    MainPage* beginPage;   /* first page of the allocated linked list */
    MainPage* endPage;     /* last page of the allocated linked list */
    
    unsigned long counter;   /* time counter */
    
    int pageSize;   /* parameter P1 */
    int prepage;    /* 1= prepaging 0=demand paging */
    int algo;       /* 0=FIFO 1=LRU 2=CLOCK */
    
    int uniqueNo;    /* unique page no */
                                    
    unsigned long numSwaps;   /* final result */
                                              
} Simulator;

/* initialize the structure and parse the arguments, return -1 if fail */
int init_parse(Simulator* sim, int argc, char* argv[]); 
                                                       
/* load plist file, return -1 if fail */
int load_plist(Simulator* sim, char* plist);

/* default load memory */
void default_load(Simulator* sim);

/* simulate with ptrace file, return -1 if fail */
int simulate(Simulator* sim, char* ptrace);

/* destroy the simulator */
void destroy(Simulator* sim);

/* allocate page at slot into memory */
void replace(Simulator* sim, int pid, int slot);

/* fifo algorithm */      
void alfifo(Simulator* sim, int pid, int slot); 
  
/* lru algorithm */      
void allru(Simulator* sim, int pid, int slot);

/* clock algorithm */      
void alclock(Simulator* sim, int pid, int slot);

int main(int argc, char* argv[]) {
    Simulator sim;
    
    /* initialize the sim structure and parse arguments */
    if (init_parse(&sim, argc, argv) < 0)
        return -1;
    
    /* load plist file */    
    if (load_plist(&sim, argv[1]) >= 0) {
        /* make default load */
        default_load(&sim);
        
        /* simulate with ptrace file */
        if (simulate(&sim, argv[2]) >= 0) {
            printf("%lu \n", sim.numSwaps);
        }
    } 
    
    destroy(&sim);

    return 0;
}   
/* initialize the structure and parse the arguments, return -1 if fail */
int init_parse(Simulator* sim, int argc, char* argv[]) {  
    int i;
    
    /* initialize the members */
    for (i = 0; i < MAX_PROG; i++)
        sim->prog[i] = 0; 

    sim->numProgs = 0;                         
    sim->numMainPages = 0;    
    sim->emptyPages = 0;
    sim->beginPage = 0;
    sim->endPage = 0;   
    sim->counter = 0;
    sim->pageSize = 0;  
    sim->prepage = 0;
    sim->algo = 0;    
    sim->numSwaps = 0;
    sim->uniqueNo = 0;
 
    /* invalid number of arguments ? */
    if (argc != 6) {
        printf("Usage: %s <plist file> <ptrace file> <P1> <P2> <P3>\n", 
           argv[0]);
        return -1;
    }
    
    /* parse P1 page size */
    sim->pageSize = atoi(argv[3]);
    
    /* parse P3 pre-paging */
    if (strcmp(argv[5], "+") == 0)
        sim->prepage = 1;
    else if (strcmp(argv[5], "-") == 0)
        sim->prepage = 0;
    else {   
        printf("Invalid <P3> should be + or -: %s\n", argv[5]);
        return -1;
    }
    
    /* convert algorithm string to upper-case */
    for (i = 0; argv[4][i]; i++)
        argv[4][i] = (char)toupper(argv[4][i]);
    
    /* parse P2 algorithm */                                           
    if (strcmp(argv[4], "FIFO") == 0)
        sim->algo = AL_FIFO;
    else if (strcmp(argv[4], "LRU") == 0)
        sim->algo = AL_LRU;        
    else if (strcmp(argv[4], "CLOCK") == 0)
        sim->algo = AL_CLOCK; 
    else {   
        printf("Invalid <P2> should be FIFO/LRU/Clock: %s\n", argv[4]);
        return -1;
    }
    
    /* print the arguments */
    printf("Page Size = %d\nAlgorithm = %s\nPre-Paging = %s\n", 
       sim->pageSize, argv[4], sim->prepage == 1 ? "on" : "off");
    
    /* calculate and print number of main pages */
    sim->numMainPages = MAIN_MEMORY / sim->pageSize;
    if ( sim->pageSize * sim->numMainPages != MAIN_MEMORY)
        sim->numMainPages++;
    printf("Total Main Memory Pages: %d\n\n", sim->numMainPages);
    
    return 0;
} 
                                                       
/* load plist file, return -1 if fail */
int load_plist(Simulator* sim, char* plist) {  
    FILE* fp = fopen(plist, "r");
    int id, num, i;    
    
    /* failed to open the file */
    if (fp == 0) {
        printf("Invalid plist file %s\n", plist);
        return -1;
    }
    
    /* read id and total number */
    while (fscanf(fp, "%d %d", &id, &num) == 2) {
        sim->numProgs++;
        
        /* allocate memory for the program */
        sim->prog[id] = (Program*)malloc(sizeof(Program));
        
        /* initialize the members */
        sim->prog[id]->pid = id;
        sim->prog[id]->totalMemory = num;
        sim->prog[id]->pclk = 0;
        
        /* calculate the table size and allocate memory */                           
        sim->prog[id]->tableSize = num / sim->pageSize;
        if (sim->pageSize * sim->prog[id]->tableSize != num)
            sim->prog[id]->tableSize++;
        
        sim->prog[id]->table = (TableEntry*)malloc(
           sizeof(TableEntry) * sim->prog[id]->tableSize);
        
        /* initialize the page table */                           
        for (i = 0; i < sim->prog[id]->tableSize; i++) {
            sim->uniqueNo++;
            sim->prog[id]->table[i].pageno = sim->uniqueNo;
            sim->prog[id]->table[i].valid = 0;
            sim->prog[id]->table[i].lastAccess = 0;
        } 
        
        printf("pid: %d total %d -- tablesize %d\n",
           id, num, sim->prog[id]->tableSize);       
    }
    
    fclose(fp);
    printf("\n");
     
    return 0;
}

/* default load memory */
void default_load(Simulator* sim) {
    /* calculate unit per program */
    int unit = sim->numMainPages / sim->numProgs;
    int i, j;
    MainPage* tmp;
    
    /* create the empty main pages */
    for (i = 0; i < sim->numMainPages; i++) {
        tmp = (MainPage*)malloc(sizeof(MainPage));
        tmp->pageno = -1;
        tmp->pid = -1;
        tmp->R = 0;
        tmp->next = sim->emptyPages;
        
        sim->emptyPages = tmp;
    }
    
    /* for each program, make default load */
    for (i = 0; i < MAX_PROG; i++) {
        if (sim->prog[i] != 0) {
            for (j = 0; j < unit && j < sim->prog[i]->tableSize; j++) {
                replace(sim, i, j);
            }
            /* clock pointers to the first page */
            sim->prog[i]->pclk = sim->beginPage;
        }
    }
    
    /* make the linked list circle for clock */
    if (sim->algo == AL_CLOCK) {
        sim->endPage->next = sim->beginPage;
    }
}

/* simulate with ptrace file, return -1 if fail */
int simulate(Simulator* sim, char* ptrace) {   
    /* open the trace file */
    FILE* fp = fopen(ptrace, "r");
    int pid, refmem, slot;
    MainPage* tmp;
    
    /* invalid file ? */
    if (fp == 0) {
        printf("Invalid ptrace file %s\n", ptrace);
        return -1;
    }
    
    /* continue to read pid-referent from the file */
    while (fscanf(fp, "%d %d", &pid, &refmem) == 2) {
        /* increase the counter */
        sim->counter++;         
        
        /* calculate the slot no */
        slot = (refmem-1)/sim->pageSize;
        
        /* if hit? */
        if ( sim->prog[pid]->table[slot].valid == 1 ) {
            /* if use FIFO, do NOT update the access time when hit */
            if (sim->algo == AL_LRU)
                sim->prog[pid]->table[slot].lastAccess = sim->counter;
            else if (sim->algo == AL_CLOCK) {
                /* for clock algorithm, we must set the R bit */
                tmp = sim->beginPage;
                while ( tmp->pageno != sim->prog[pid]->table[slot].pageno )
                    tmp = tmp->next;
                tmp->R = 1;
            }    
        }
        else {
            sim->numSwaps++;              
            
            replace(sim, pid, slot);
            
            /* if prepaging, find next page */
            if (sim->prepage == 1) {
                slot++;
                while (slot < sim->prog[pid]->tableSize &&
                    sim->prog[pid]->table[slot].valid == 1)
                    slot++;
                
                /* find a page not in memory */
                if (slot < sim->prog[pid]->tableSize)
                    replace(sim, pid, slot);
            }
        }        
    }
    
    fclose(fp);
  
    return 0;
}

/* destroy the simulator */
void destroy(Simulator* sim) {       
    int i;
    MainPage* tmp;
    
    /* delete all page table for the programs */
    for (i = 0; i < MAX_PROG; i++) {
        if (sim->prog[i] != 0) {
            free(sim->prog[i]->table);
            free(sim->prog[i]);
        }
    }
    
    /* free all empty pages if exists */
    while (sim->emptyPages != 0) {
        tmp = sim->emptyPages;
        sim->emptyPages = tmp->next;
        free(tmp);
    }
    
    /* break the circle linked list if clock is used */
    if (sim->endPage != 0)
        sim->endPage->next = 0;
    
    /* free all pages in the linked list */                            
    while (sim->beginPage != 0) {
        tmp = sim->beginPage;
        sim->beginPage = tmp->next;
        free(tmp);
    }
}  

/* allocate page at slot into memory */
void replace(Simulator* sim, int pid, int slot) {   
    MainPage* tmp;     
                  
    if (sim->algo == AL_LRU || sim->algo == AL_FIFO)
        sim->prog[pid]->table[slot].lastAccess = sim->counter;
    sim->prog[pid]->table[slot].valid = 1;
                
    /* there is empty pages */
    if (sim->emptyPages != 0) {
        /* allocate an empty page */
        tmp = sim->emptyPages;
        sim->emptyPages = tmp->next;
        
        /* set the page information */
        tmp->pageno = sim->prog[pid]->table[slot].pageno;
        tmp->pid = pid;
        tmp->R = 1;
        tmp->next = 0;          
        
        /* moved to the end of the linked list */
        if (sim->beginPage == 0) {
            sim->beginPage = tmp;
            sim->endPage = tmp;
        }
        else {                       
            sim->endPage->next = tmp;
            sim->endPage = tmp;
        }           
        
        /* make the list circle for clock */
        if (sim->algo == AL_CLOCK)
            sim->endPage->next = sim->beginPage;         
    }
    else if (sim->algo == AL_FIFO) {
        alfifo(sim, pid, slot);
    }
    else if (sim->algo == AL_LRU) {    
        allru(sim, pid, slot);
    }                  
    else if (sim->algo == AL_CLOCK) {    
        alclock(sim, pid, slot);
    }
}  

/* fifo algorithm */      
void alfifo(Simulator* sim, int pid, int slot) {
    /* find the first */
    int i, rslot = -1;
    MainPage* tmp;
    
    for (i = 0; i < sim->prog[pid]->tableSize; i++) {
        if (i != slot && sim->prog[pid]->table[i].valid == 1) {
            if (rslot == -1)
                rslot = i;
            else if ( sim->prog[pid]->table[i].lastAccess < 
                      sim->prog[pid]->table[rslot].lastAccess)
                rslot = i;
        }
    }
    
    sim->prog[pid]->table[rslot].valid = 0;
    
    /* find the page in main memory */
    tmp = sim->beginPage;
    while (tmp->pageno != sim->prog[pid]->table[rslot].pageno)
        tmp = tmp->next;
        
    tmp->pageno = sim->prog[pid]->table[slot].pageno;   
    tmp->pid = pid;   
    
} 
  
/* lru algorithm */      
void allru(Simulator* sim, int pid, int slot) {      
    /* find the least recent used slot */

    int i, rslot = -1;
    MainPage* tmp;
    
    for (i = 0; i < sim->prog[pid]->tableSize; i++) {
        if (i != slot && sim->prog[pid]->table[i].valid == 1) {
            if (rslot == -1)
                rslot = i;
            else if ( sim->prog[pid]->table[i].lastAccess < 
                      sim->prog[pid]->table[rslot].lastAccess)
                rslot = i;
        }
    }
    
    sim->prog[pid]->table[rslot].valid = 0;
    
    /* find the page in main memory */
    tmp = sim->beginPage;
    while (tmp->pageno != sim->prog[pid]->table[rslot].pageno)
        tmp = tmp->next;
        
    tmp->pageno = sim->prog[pid]->table[slot].pageno;
    tmp->pid = pid;
    
}

/* clock algorithm */      
void alclock(Simulator* sim, int pid, int slot) {
    MainPage* tmp;
    int i;
    
    /* find a page with R = 0 in the clock list */
    while (sim->prog[pid]->pclk->pid != pid ||
       sim->prog[pid]->pclk->R == 1) {
        /* R is 1 */
        if (sim->prog[pid]->pclk->pid == pid) {
            sim->prog[pid]->pclk->R = 0;
        }
        sim->prog[pid]->pclk = sim->prog[pid]->pclk->next;
    }
    
    tmp = sim->prog[pid]->pclk;
    
    /* move the clock pointer to next */
    sim->prog[pid]->pclk = sim->prog[pid]->pclk->next;
    
    /* set the valid bit to 0 in the table */
    for (i = 0; i < sim->prog[pid]->tableSize; i++) {
        if (sim->prog[pid]->table[i].pageno == tmp->pageno) {
            sim->prog[pid]->table[i].valid = 0;
            break;
        }
    }
    
    tmp->pageno = sim->prog[pid]->table[slot].pageno;
    tmp->pid = pid;   
    tmp->R = 1;
}




