/*
    Assignment 2
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: April 1st, 2023
*/

/*
    REFERENCES:
    Practice Lab 5 Part I (address translation code and syntax)
    Practice Lab 5 Part II (memory mapping code and syntax)
    Chapter 9 slides II (logical to physical memory mapping)
    Chapter 9 slides II (how page table and TLB work)
    For help with typedef: https://www.tutorialspoint.com/cprogramming/c_typedef.htm
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 10                     // Number of characters stored in buffer
#define PAGES 256                       // Number of pages in page table
#define PAGE_SIZE 256                   // Page size = 256 bytes
#define FRAMES 128                      // Page size/2, as logical address space is 2x physical address space
#define OFFSET_MASK 255                 // Offset mask = page size - 1 (as offset starts at 0)
#define OFFSET_BITS 8                   // Number of bits to represent offset; 2^8 = 256, so need 8 bits
#define MAX_TLB 16                      // Given in assignment
#define MEM_SIZE FRAMES * PAGE_SIZE     // Define size of physical memory

// TLBentry and BYTE created for better code readability
typedef signed char BYTE;
struct TLBentry {
    int pageNumber;
    BYTE frameNumber;
};

// DATA STRUCTURES FOR MMU
BYTE pageTable[PAGES];              // Will store the frames to map to memory
BYTE frameTable[MEM_SIZE];          // Emulates physical memory
int nextFrame = 0;                  // Next available frame
BYTE *mmapfptr;                     // Used for memory mapping BACKING_STORE.bin
struct TLBentry TLB[MAX_TLB];       // Translation lookaside buffer
int nextTLB = 0;                    // Next available TLB location

// OUTPUT STATISTICS
int pageFaults = 0;         // Output total # of page faults when finished
int TLBhits = 0;            // Output total # of TLB hits when finished
int TLBmisses = 0;          // Output total # of TLB misses when finished

BYTE findFrame(uint8_t page);
void TLB_Update(struct TLBentry data);

// UTILITY FUNCTION: used for handling page faults
int findFrameGivenPage(BYTE frame) {
    // Essentially, just searches through page table to find any matches
    for (int i = 0; i < PAGES; i++) {
        if (pageTable[i] == frame) {
            return i;
        }
    }
    return -1;
}

// PART 3 OF ASSIGNMENT: handling page faults
BYTE handlePageFault(uint8_t page) {
    pageFaults++;
    // Firstly, we have to copy a 256-byte page from backing store -> physical memory array
    memcpy(frameTable + nextFrame*PAGE_SIZE, mmapfptr + page*PAGE_SIZE, PAGE_SIZE);
    // Next, we must update the page table!
    int curr_page = findFrameGivenPage(nextFrame);
    // If curr_page is -1, there's no page that is already using the frame, so don't have to use FIFO
    if (curr_page >= 0) {
        pageTable[curr_page] = -1;
    }
    pageTable[page] = nextFrame;
    // Now, we must replace the page curr_page with the new page in TLB
    struct TLBentry entry;
    entry.pageNumber = (int) page;
    entry.frameNumber = (BYTE) nextFrame;
    TLB_Update(entry);
    // Finally, increment nextFrame:
    nextFrame++;
    nextFrame %= FRAMES;
    return nextFrame - 1;
}

// Search through TLB to find relevant page
BYTE search_TLB(uint8_t page) {
    for (int i = 0; i < MAX_TLB; i++) {
        // If page found in TLB, increment TLBhits output statistic and return corresponding frame number
        // (i + nextTLB) % MAX_TLB ensures we start searching at oldest entry, maintaining CLL FIFO
        if (TLB[(i + nextTLB) % MAX_TLB].pageNumber == (int) page) {
            TLBhits++;
            return TLB[(i + nextTLB) % MAX_TLB].frameNumber;
        }
    }
    // Page not found in TLB, increment TLBmisses output statistic
    TLBmisses++;
    // Must find frame using page table, then update the TLB
    return findFrame(page);
}

// Add entry to TLB following FIFO
void TLB_Add(struct TLBentry data) {
    // Replace oldest entry in TLB with new entry; FIFO
    TLB[nextTLB].pageNumber = data.pageNumber;
    TLB[nextTLB].frameNumber = data.frameNumber;
    nextTLB++;
    nextTLB %= MAX_TLB;     // Follows FIFO policy
}

// Update TLB when physical memory mapping is chagned (ie page fault occurred)
void TLB_Update(struct TLBentry data) {
    // First, search TLB to see if page containing data's frame is already there; then we update
    for (int i = 0; i < MAX_TLB; i++) {
        // (i + nextTLB) % MAX_TLB ensures we start searching at oldest entry, maintaining CLL FIFO
        if (TLB[(i + nextTLB) % MAX_TLB].frameNumber == data.frameNumber) {
            // FOUND!
            TLB[(i + nextTLB) % MAX_TLB].pageNumber = data.pageNumber;
            return;
        }
    }
    // If not found, use TLB_Add to add the new entry to TLB
    TLB_Add(data);
}

// Called by search_TLB if a TLB miss occurs
BYTE findFrame(uint8_t page) {
    if (pageTable[page] != -1) {
        // Frame found in page table; update TLB and return frame number
        struct TLBentry entry;
        entry.pageNumber = (int) page;
        entry.frameNumber = pageTable[page];
        TLB_Add(entry);
        return pageTable[page];
    } else {
        // Frame not found in page table; handle page fault
        return handlePageFault(page);
    }
}

int main () {
    for (int i = 0; i < MAX_TLB; i++) {
        TLB[i].pageNumber = -1;     // Can range from 0 to 255, so -1 means nothing is in the TLB yet
        TLB[i].frameNumber = (BYTE) -1;
    }
    for (int i = 0; i < PAGES; i++) {
        pageTable[i] = (BYTE) -1;
    }
    // Open addresses and backing store files
    FILE * fp = fopen("addresses.txt", "r");
    int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
    mmapfptr = mmap(0, PAGES * PAGE_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
    // Variables to help read addresses.txt
    BYTE buf[BUF_SIZE];
    uint8_t page = 0x00;
    uint8_t off = 0x00;
    int physical = 0;
    int frame = 0;
    int logical = 0;
    // Note that maximum # of characters in "addresses.txt" for 1 address is FIVE
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        // PART 1 OF ASSIGNMENT: address translation (follows PL5)
        logical = atoi(buf);
        page = (uint8_t) (atoi(buf) >> OFFSET_BITS);
        off = (uint8_t) (atoi(buf) & OFFSET_MASK);
        frame = (int) search_TLB(page);
        physical = (frame << OFFSET_BITS) | off;
        printf("VIRTUAL ADDR %d: PHYSICAL ADDR %d: SIGNED BYTE VALUE:%hhd\n", logical, physical, frameTable[physical]);
    }
    // Close and unmap addresses.txt and BACKING_STORE.bin
    fclose(fp);
    munmap(mmapfptr, PAGES * PAGE_SIZE);
    // OUTPUT STATISTICS
    printf("Number of TLB hits: %d\n", TLBhits);
    printf("Number of TLB misses: %d\n", TLBmisses);
    printf("Number of page faults: %d\n", pageFaults);
    printf("Total number of addresses: %d\n", TLBhits + TLBmisses);
    return 0;
}