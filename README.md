# Virtual Memory Lab

This project implements various page replacement algorithms used in virtual memory management systems. It simulates how an operating system might manage page tables, handle page faults, and replace pages when necessary.

## Algorithms Implemented

1. **First-In-First-Out (FIFO)**: Replaces the oldest page in memory.
2. **Least Recently Used (LRU)**: Replaces the page that hasn't been used for the longest time.
3. **Least Frequently Used (LFU)**: Replaces the page with the lowest reference count.

## Key Features

- Simulates a page table as an array of Page Table Entries (PTEs).
- Each PTE tracks its validity, frame number, arrival timestamp, last access timestamp, and reference count.
- Handles scenarios such as page hits, page faults with free frames, and page replacements.
- Implements page fault counting for each algorithm.

## How It Works

Each page replacement algorithm manages the page table based on its specific criteria:

- **FIFO**: Tracks the arrival time of each page and replaces the oldest one when necessary.
- **LRU**: Updates the last access time of each page and replaces the least recently used when necessary.
- **LFU**: Maintains a reference count for each page and replaces the least frequently used (with a tie-breaker based on arrival time) when necessary.

The page fault counting functions simulate a sequence of memory accesses and return the total number of page faults occurred.

## Usage

Include the necessary headers in your project and call the functions as needed:

```c
#include "PCB.h"

// FIFO functions
int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp);
int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt);

// LRU functions
int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp);
int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt);

// LFU functions
int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp);
int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt);
```

## Implementation Details

### Page Table Entry (PTE) Structure

```c
struct PTE {
    int is_valid;
    int frame_number;
    int arrival_timestamp;
    int last_access_timestamp;
    int reference_count;
};
```

### Key Functions

1. `process_page_access_*`: These functions (where * is fifo, lru, or lfu) handle individual page accesses. They update the page table and return the frame number for the accessed page.

2. `count_page_faults_*`: These functions simulate a sequence of page accesses using the respective algorithm and return the total number of page faults.

Each algorithm implementation includes logic for:
- Handling page hits
- Allocating free frames when available
- Implementing the specific page replacement policy when no free frames are available

## Notes

- The project uses predefined constants like TABLEMAX, POOLMAX, and REFERENCEMAX. Ensure these are properly defined in your header files.
- The implementation assumes a simplistic model of virtual memory and may not reflect all complexities of real-world systems.
- Error handling and edge cases (like full page tables) are not extensively covered and may need additional implementation for production use.
