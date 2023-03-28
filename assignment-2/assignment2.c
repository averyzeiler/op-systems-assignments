/*
    Assignment 2
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: March 31st, 2023
*/

/*
    HANDLING PAGE FAULTS [15]
    - Copy page from backing store -> frame in memory
    - BACKING_STORE.bin represents backing store; size of 2^16 bytes (same ass logical address space)
    - Open backing store using open() + map it to a memory region using mmap()
    - PAGE FAULT: read in 256-byte page from this memory-mapped file + copy it to available frame in physical memory using memcpy()
    - FIFO replacement means that 2 ENTRIES of page table must be updated
        - Replaced page must have entry -1
        - Other = new page brought into memory
    *** SIMULATE PHYSICAL MEMORY AS CIRCULAR ARRAY
    *** SLIDES ON VIRTUAL MEMORY + MEMORY MAPPED FILES + PL5
*/

/*
    PAGE FAULTS EXAMPLE:
    logical address w/ page # 15 resulted in page fault
    - Read in page 15 from BACKING STORE + store in an empty frame in physical memory
    - Empty frame DNE -> replace the oldest page
    - For example, page 2 == oldest page; thus, store page 15 in frame occupied by page 2
    - UPDATE PAGE TABLE TOO!
*/

/*
    PROGRAM OUTPUT
    *** EXAMPLE in output.txt in requirements
    - Logical address being translated (from addresses.txt)
    - Corresponding physical address
    - Signed byte value stored @ physical address
    - Compute + output stats (report these):
        - Total # of page faults that occurred
        - Total # of TLB-hits that occurred
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 10
#define PAGES 256
#define PAGE_SIZE 256
#define OFFSET_MASK 255
#define OFFSET_BITS 8
#define MAX_TLB 16

// pages in pageTable can be from 0 to 127, as physical address space is half the size of logical address space!
// this is why they are of type char, because -1 will indicate NOTHING IN PAGE TABLE!
char pageTable[PAGES];   // Will store the frames to map to memory
int pageFaults = 0;
int TLBhits = 0;
int TLBmisses = 0;

// NOTE: binary file stores SIGNED BYTES; thus they are of type char

// https://www.programiz.com/dsa/circular-linked-list
struct TLBentry {
    uint8_t pageNumber;
    uint8_t frameNumber;
};

struct Node {
    struct TLBentry data;
    struct Node* next;
};

char findFrame(uint8_t page);

int handlePageFault() {
    pageFaults++;
    printf("Page fault occurred; put page fault handling code here.\n");
    return -1;
}

// NOTE: last == end of CLL!
// UTILITY FUNCTION: create a new CLL
struct Node* addToEmpty (struct Node* last, struct TLBentry data) {
    // do nothing if list is not empty
    if (last != NULL) return last;
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data.pageNumber = data.pageNumber;
    newNode->data.frameNumber = data.frameNumber;
    last = newNode;
    last->next = last;
    // last points to itself
    return last;
}

// NOTE: add to this function to make sure list stays smaller than 16!
struct Node* addFront(struct Node* last, struct TLBentry data) {
    // create new list if list is empty
    if (last == NULL) return addToEmpty(last, data);
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data.pageNumber = data.pageNumber;
    newNode->data.frameNumber = data.frameNumber;
    newNode->next = last->next;
    last->next = newNode;
    // above makes list go from last->next to last->newNode->next
    return last;
}

int sizeOfTLB (struct Node* last) {
    struct Node* p;
    if (last == NULL) {
        printf("The list is empty.\n");
        return 0;
    }
    p = last->next;
    int count = 0;
    do {
        p = p->next;
        count++;
    } while (p != last->next);
    return count;
}

int search_TLB (struct Node* last, uint8_t page) {
    struct Node* p;
    if (last == NULL) {
        printf("The list is empty.\n");
        TLBmisses++;
        return 0;
    }
    p = last->next;
    do {
        // if page found, exit loop and return frame number
        if (p->data.pageNumber == page) {
            printf("TLB hit!\n");
            TLBhits++;
            return (int)p->data.frameNumber;
        }
        p = p->next;
    } while (p != last->next);
    printf("TLB miss!\n");
    TLBmisses++;
    return findFrame(page);   // indicates a TLB miss
}

struct Node* find_TLB (struct Node* last, uint8_t page) {
    struct Node* p;
    if (last == NULL) {
        printf("The list is empty.\n");
        return last;
    }
    p = last->next;
    do {
        // if page found, exit loop and return frame number
        if (p->data.pageNumber == page) {
            printf("Found page p!\n");
            return p;
        }
        p = p->next;
    } while (p != last->next);
    printf("Could not find page p!\n");
    return last;   // indicates a TLB miss
}

struct Node* TLB_Add (struct Node* last, struct TLBentry data) {
    // add entry if traverse returns less than 16
    int size = sizeOfTLB(last);
    if (size == 0) {
        struct Node* newNode = addToEmpty(last, data);
        return newNode;     // returns last
    } else if (size < 16) {
        struct Node* newNode = addFront(last, data);
        return newNode;     // returns last
    } else {
        // if TLB already has 16 entries, must replace first entry
        last->data.frameNumber = data.frameNumber;
        last->data.pageNumber = data.pageNumber;
        return last;
    }
}

struct Node* TLB_Update (struct Node* last, struct TLBentry data) {
    struct Node* tmp = find_TLB(last, data.pageNumber);
    if (tmp == last) {
        // page p was NOT found in the TLB!
        tmp = TLB_Add(last, data);
        return tmp;
    } else {
        // page p was found in the TLB!
        tmp->data.pageNumber = data.pageNumber;
        tmp->data.frameNumber = data.frameNumber;
        return last;
    }
}

uint8_t pageNum (char* buf) {
    int temp = atoi(buf);
    uint8_t result = (uint8_t) (temp >> OFFSET_BITS);
    return result;
}

uint8_t offset (char* buf) {
    int temp = atoi(buf);
    uint8_t result = (uint8_t) (temp & OFFSET_MASK);
    return result;
}

char findFrame(uint8_t page) {
    if (pageTable[page] != -1) {
        printf("Found in page table that frame num = %d.\n", pageTable[page]);
        return pageTable[page];
    } else {
        // page fault occurs!!
        // call function to handle page faults
        return handlePageFault();
    }
}

int main () {
    for (int i = 0; i < PAGES; i++) {
        pageTable[i] = (char)-1;
    }
    FILE * fp = fopen("requirements/addresses.txt", "r");
    char buf[BUF_SIZE];
    uint8_t page = 0x00;
    uint8_t off = 0x00;
    // Note that maximum # of characters in "addresses.txt" for 1 address is FIVE
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        page = pageNum(buf);
        off = offset(buf);
        printf("For address %s pageNum = %u, offset = %u\n", buf, page, off);
        int frame = (int) findFrame(page);
        printf("Frame number for page %u is %d\n", page, frame);
        int physical_loc = (frame << OFFSET_BITS) | off;
    }
    fclose(fp);
    printf("Number of TLB hits: %d\n", TLBhits);
    printf("Number of TLB misses: %d\n", TLBmisses);
    printf("Number of page faults: %d\n", pageFaults);
    return 0;
}