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
#include <stdbool.h>

#define BUF_SIZE 10         // Number of characters stored in buffer
#define PAGES 256           // Number of pages in page table
#define PAGE_SIZE 256       // Page size = 256 bytes
#define FRAMES 128          // Page size/2, as logical address space is 2x physical address space
#define OFFSET_MASK 255     // Offset mask = page size - 1 (as offset starts at 0)
#define OFFSET_BITS 8       // Number of bits to represent offset; 2^8 = 256, so need 8 bits
#define MAX_TLB 16          // Given in assignment

// pages in pageTable can be from 0 to 127, as physical address space is half the size of logical address space!
// this is why they are of type char, because -1 will indicate NOTHING IN PAGE TABLE!
char pageTable[PAGES];      // Will store the frames to map to memory
bool frameTable[FRAMES];    // Will store which frames are empty
bool checkVal = False;      // Defines which value in the frame table (T or F) indicates the next available frame 
int pageFaults = 0;         // Output total # of page faults when finished
int TLBhits = 0;            // Output total # of TLB hits when finished
int TLBmisses = 0;          // Output total # of TLB misses when finished

// NOTE: binary file stores SIGNED BYTES; thus they are of type char
// NOTE: physical memory addresses will all look like 0b0xxxxxxxxxxxxxxx (x = 0 or 1), as MSB of 0 means +ve for signed ints/chars

struct TLBentry {
    uint8_t pageNumber;
    char frameNumber;
};

// https://www.programiz.com/dsa/circular-linked-list
struct Node {
    struct TLBentry data;
    struct Node* next;
};

char findFrame(uint8_t page);

// UTILITY FUNCTION: used for handling page faults
int findFrameGivenPage(char frame) {
    for (int i = 0; i < PAGES; i++) {
        if (pageTable[i] == frame) {
            return i;
        }
    }
    return -1;
}

int handlePageFault(uint8_t page) {
    char available = 0;     // Represents available frame to store page with
    bool found = False;
    for (int i = 0; i < FRAMES; i++) {
        if (frameTable[i] == checkVal) {
            // Next available frame is at index i
            frameTable[i] = !frameTable[i];     // Make frame not available
            available = i;
            found = True;
            printf("Next available frame was found to be %c.\n", available);
            break;
        }
    }
    if (found == False) {
        // Did not find an available frame
        // Start again at beginning of frame table (FIFO)
        checkVal = !checkVal;
        frameTable[0] = !frameTable[0];
        // To explain: for instance, if checkVal = F, that means all frameTable = T
        // Then: we change checkVal to T, and set frameTable[0] to F, so next page fault will result in available = 1 and so on.
    }
    // Update the page table
    int curr_page = findFrameGivenPage(available);
    // If curr_page is -1, there's no page that is already using the frame, so don't have to use FIFO
    if (curr_page >= 0) {
        pageTable[curr_page] = -1;
    }
    pageTable[page] = available;

    // Next, we do the memory-mapping stuff which I do not understand yet!

    

    //https://linuxhint.com/c-language-o_donly-o_wrongly-and-o_rdwr-flags/
    // open the backing store file
    int backing_store_fd = open("BACKING_STORE.bin", O_RDONLY); //O_RDONLY is flag used with open() function, read only 

    // map the backing store file to a memory region
    char* backing_store_data = mmap(NULL, BACKING_STORE_SIZE, PROT_READ, MAP_PRIVATE, backing_store_fd, 0);

    // initialize page table and frame table
    int frame_table[NUM_PAGES];
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i] = -1; //set entries in page table to -1,  not in memory 
    }
    for (int i = 0; i < NUM_FRAMES; i++) {
        frame_table[i].page_number = -1; //set entries in frame table to -1, not added yet
    }

    // open the addresses file
    FILE* addresses_file = fopen("addresses.txt", "r");

    // initialize statistics variables
    int num_page_faults = 0;
    int num_tlb_hits = 0;

    // read addresses from file
    //https://lxadm.com/how-to-convert-logical-address-to-physical-address-in-paging/
    int logical_address;
    while (fscanf(addresses_file, "%d", &logical_address) == 1) {
        // translate logical address to physical address
        int page_number = (logical_address >> 8) & 0xFF; //bit mask used to set all bits to 0 except least sig 8
        int page_offset = logical_address & 0xFF;
        int frame_number = page_table[page_number];
        if (frame_number == -1) { // page fault
            num_page_faults++;
            // look for an empty frame in memory
            int empty_frame_number = -1; //no empty frame available
            for (int i = 0; i < NUM_FRAMES; i++) { //itterate through frame table to find an empty spot
                if (frame_table[i].page_number == -1) { //if empty frame found
                    empty_frame_number = i; //index is stored
                    break;
                }
            }
            // copy page from backing store to physical memory
            memcpy(&physical_memory[empty_frame_number * PAGE_SIZE], &backing_store_data[page_number * PAGE_SIZE], PAGE_SIZE);
            // update page table and frame table
            update_page_table(page_number, empty_frame_number, page_table);
            frame_number = empty_frame_number;
        }
        else { // TLB hit
            num_tlb_hits++;
        }
        //output the virtual addressm physical address, signed byte
        int physical_address = (frame_number << 8) | page_offset; //combine frame num and page offset
        int signed_byte_value = (int) physical_memory[frame_number * PAGE_SIZE + page_offset]; //go to physical mempry array using address
        printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, signed_byte_value);
    }

    pageFaults++;
    return -1;
}

// NOTE: last == end of CLL!
// UTILITY FUNCTION: create a new circularly linked list
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

// UTILITY FUNCTION: add to front of circularly linked list
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

// UTILITY FUNCTION: returns size of TLB to ensure it doesn't exceed its limits
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

// From assignment doc, will search TLB for page # and will return frame # if found
    // If not, will search through page table for frame #
char search_TLB (struct Node* last, uint8_t page) {
    struct Node* p;
    if (last == NULL) {
        // If the TLB is empty, we already know the page isn't in the TLB!
        printf("The list is empty.\n");
        TLBmisses++;
        return 0;
    }
    p = last->next;
    do {
        // If page found, exit loop and return frame number
        if (p->data.pageNumber == page) {
            printf("TLB hit!\n");
            TLBhits++;
            return (char)p->data.frameNumber;
        }
        p = p->next;
    } while (p != last->next);
    printf("TLB miss!\n");
    TLBmisses++;
    return findFrame(page);   // Indicates a TLB miss
}

// UTILITY FUNCTION: returns pointer to TLB entry containing corresponding page number
struct Node* find_TLB (struct Node* last, uint8_t page) {
    struct Node* p;
    if (last == NULL) {
        // If the TLB is empty, we already know the page isn't in the TLB!
        printf("The list is empty.\n");
        return last;
    }
    p = last->next;
    do {
        // If page found, exit loop and return frame number
        if (p->data.pageNumber == page) {
            printf("Found page p!\n");
            return p;
        }
        p = p->next;
    } while (p != last->next);
    printf("Could not find page p!\n");
    return last;   // Indicates a TLB miss
}

// From assignment doc, will add an entry to the front of TLB
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
        return last->next;
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
        return handlePageFault(page);
    }
}

int main () {
    for (int i = 0; i < PAGES; i++) {
        pageTable[i] = (char)-1;
    }
    for (int i = 0; i < FRAMES; i++) {
        frameTable[i] = True;
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