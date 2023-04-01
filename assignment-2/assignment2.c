/*
    Assignment 2
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: March 31st, 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 10         // Number of characters stored in buffer
#define PAGES 256           // Number of pages in page table
#define PAGE_SIZE 256       // Page size = 256 bytes
#define FRAMES 128          // Page size/2, as logical address space is 2x physical address space
#define OFFSET_MASK 255     // Offset mask = page size - 1 (as offset starts at 0)
#define OFFSET_BITS 8       // Number of bits to represent offset; 2^8 = 256, so need 8 bits
#define MAX_TLB 16          // Given in assignment
#define MEM_SIZE FRAMES * PAGE_SIZE

typedef signed char BYTE;

// pages in pageTable can be from 0 to 127, as physical address space is half the size of logical address space!
// this is why they are of type char, because -1 will indicate NOTHING IN PAGE TABLE!
BYTE pageTable[PAGES];        // Will store the frames to map to memory
BYTE frameTable[MEM_SIZE];    // Emulates physical memory
int nextFrame = 0;            // Next available frame
BYTE *mmapfptr;

// OUTPUT STATISTICS
int pageFaults = 0;         // Output total # of page faults when finished
int TLBhits = 0;            // Output total # of TLB hits when finished
int TLBmisses = 0;          // Output total # of TLB misses when finished

struct TLBentry {
    int pageNumber;
    BYTE frameNumber;
};

struct TLBentry TLB[MAX_TLB];
int nextTLB = 0;

BYTE findFrame(uint8_t page);

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

void TLB_Update(struct TLBentry data);

// PART 3 of assignment, handling page faults
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

BYTE search_TLB(uint8_t page) {
    for (int i = 0; i < MAX_TLB; i++) {
        // If page found in TLB, increment TLBhits output statistic and return corresponding frame number
        if (TLB[i].pageNumber == (int) page) {
            TLBhits++;
            return TLB[i].frameNumber;
        }
    }
    // Page not found in TLB, increment TLBmisses output statistic
    TLBmisses++;
    // Then, must find frame using page table, then update the TLB
    return findFrame(page);
}

// From assignment doc, will add an entry to the TLB
void TLB_Add(struct TLBentry data) {
    // Replace oldest entry in TLB with new entry; FIFO
    TLB[nextTLB].pageNumber = data.pageNumber;
    TLB[nextTLB].frameNumber = data.frameNumber;
    nextTLB++;
    nextTLB %= MAX_TLB;     // Follows FIFO policy
}

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

// NOTE: modified this function such that the TLB will be used!
BYTE findFrame(uint8_t page) {
    if (pageTable[page] != -1) {
        struct TLBentry entry;
        entry.pageNumber = (int) page;
        entry.frameNumber = pageTable[page];
        TLB_Add(entry);
        return pageTable[page];
    } else {
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
        logical = atoi(buf);
        page = (uint8_t) (atoi(buf) >> OFFSET_BITS);
        off = (uint8_t) (atoi(buf) & OFFSET_MASK);
        frame = (int) search_TLB(page);
        physical = (frame << OFFSET_BITS) | off;
        printf("VIRTUAL ADDR %d: PHYSICAL ADDR %d: SIGNED BYTE VALUE:%hhd\n", logical, physical, frameTable[physical]);
    }
    fclose(fp);
    munmap(mmapfptr, PAGES * PAGE_SIZE);
    printf("Number of TLB hits: %d\n", TLBhits);
    printf("Number of TLB misses: %d\n", TLBmisses);
    printf("Number of page faults: %d\n", pageFaults);
    return 0;
}