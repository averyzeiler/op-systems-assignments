/*
    Assignment 2
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: March 31st, 2023
*/

/*
    We are writing a SIMPLE MEMORY MANAGEMENT SIMULATOR IN C that supports PAGING
    Logical address space (2^16 bytes) larger than physical address space (2^15 bytes)
    Page size = 256 bytes
    Max # of entries in translation lookaside buffer = 16
*/

/*
    Simulate a MEMORY MANAGEMENT UNIT (MMU); translates logical -> physical addresses
    - How to translate logical -> physical addresses:
        - Check TLB for the page
        - Page not found in TLB: check page table if page exists in memory
        - Page not found in page table -> FAULT OCCURS
    - Handling page faults:
        - Copy page from backing store -> memory
        - Since logical address space > physical, page request might involve replacing a page in memory with the new page
        - Page replacement policy == FIFO page replacement policy
*/

// Divided into 3 parts: address translation, translation lookaside buffer, and page fault handling

/*
    ADDRESS TRANSLATION [15]
    - addresses.txt (in requirements) contains integers representing logical addresses ranging over whole logical address space
    - Program will open addresses.txt using fopen(); read logical addresses + compute page number and offset of address using bitwise operators
    - Will use page number from ^ to look up in TLB
        - TLB-hit (entry @ page number EXISTS): frame number is obtained from TLB
        - TLB-miss (NO entry @ corresponding page number): look up page table
        - In either case, frame number obtained from page table OR page fault occurs
    - Page table can simply be an array! Entries initialized to -1 to indicate a page is not in memory (DEMAND PAGING)
    *** LECTURE NOTES ON MAIN MEMORY AND PL5!
*/

/*
    TRANSLATION LOOKASIDE BUFFER [10]
    - Create data structure TLBentry; stores page # + frame # pair to simulate entries of TLB
    - Need 3 functions related to TLB:
        - search_TLB: search TLB for entry corresponding to page #
        - TLB_add: add an entry to TLB using FIFO policy (TLB full -> new entry replaces oldest entry)
        - TLB_Update: update page when page P is replaced in physical memory & entry corresponding to P already exists in TLB
            - Simple version (this assignment): add new page entry at same location as P (...replace?)
    *** IMPLEMENT TLB AS A CIRCULAR ARRAY!
    *** Bypass TLB + use only a page table initially, then integrate TLB once program works properly
        - TLB only makes adress translation faster, but MM works WITHOUT a TLB
*/

/*
    HANDLING PAGE FAULTS [15]
    - Copy page from backing store -> frame in memory
    - BACKING_STORE.bin represents backing store; size of 2^16 bytes
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