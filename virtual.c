#include "PCB.h"
#include <limits.h>

// This function implements the logic to process a page access in a system that uses the First-In
// First-Out (FIFO) policy for page replacement. Specifically, it takes four inputs:
// 1. process page table
// 2. logical page number
// 3. process frame pool
// 4. current timestamp.
// The function determines the memory frame number for the logical page and returns this number

int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {

// First the function checks if the page being referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it returns the frame number, after modifying the last_access_timestamp and the reference_count fields of the page-table entry.

    if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

// If the page being referenced is not in memory, the function checks if there are any free frames (i.e., the process frame pool is not empty). If so, a frame is removed from the process frame pool and the frame number is inserted into the page-table entry corresponding to the logical page number. In addition, the other fields of the page-table entry are set appropriately. The function returns the frame number associated with the page-table entry.


    if (*frame_cnt > 0) {
        int frame = frame_pool[--(*frame_cnt)];
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame;
    }

    int oldest_timestamp = current_timestamp;
    int oldest_page = -1;
    
    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].arrival_timestamp < oldest_timestamp) {
            oldest_timestamp = page_table[i].arrival_timestamp;
            oldest_page = i;
        }
    }

// If the page being referenced is not in memory and there are no free frames for the process, a page needs to be replaced. The function selects among all the pages of the process that are currently in memory (i.e., they have valid bits as true) the page that has the smallest arrival_timestamp. It marks that page_table entry as invalid, along with setting the frame_number, arrival_timestamp, last_access_timestamp and reference_count to -1. It then sets the frame_number of the page-table entry of the newly-referenced page to the newly freed frame. It also sets the arrival_timestamp, the last_access_timestamp and the reference_count fields of the page-table entry appropriately. Finally, the function returns this frame number.

    int frame = page_table[oldest_page].frame_number;
    page_table[oldest_page].is_valid = 0;
    page_table[oldest_page].frame_number = -1;
    page_table[oldest_page].arrival_timestamp = -1;
    page_table[oldest_page].last_access_timestamp = -1;
    page_table[oldest_page].reference_count = -1;

    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame;
}

// This function simulates the processing of a sequence of page references in a system that uses the First-In-First-Out (FIFO) policy for page replacement. Specifically, it takes three inputs: 
// 1. process page table
// 2. sequence of logical page numbers
// 3. process frame pool.

//The function returns the estimated number of page faults for the reference string, with respect to the pool of frames allocated to the process. For each logical page number (in the sequence), the function simulates the processing of the page access in the FIFO system. It keeps track of the number of page faults that occur in the system as it simulates the processing of the entire sequence of logical page numbers.


int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int page_faults = 0;
    int current_timestamp = 1;
    int frames_in_use = 0;

    struct PTE local_page_table[TABLEMAX];
    for (int i = 0; i < table_cnt; i++) {
        local_page_table[i] = page_table[i];
        if (local_page_table[i].is_valid) {
            frames_in_use++;
        }
    }

// When simulating the processing of a page access, the function first checks if the page being referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it modifies the last_access_timestamp and the reference_count fields of the page-table entry. It then moves on to the processing of the next entry in the sequence of logical page numbers. 

    int local_frame_pool[POOLMAX];
    int local_frame_cnt = frame_cnt;
    for (int i = 0; i < frame_cnt; i++) {
        local_frame_pool[i] = frame_pool[i];
    }

    // In order to simulate timestamps, the function starts with a timestamp of 1 and increments it whenever the processing of a new page access is begun.

    for (int i = 0; i < reference_cnt; i++) {
        int page_number = reference_string[i];

        //If the page being referenced is not in memory, the function checks if there are any free frames (i.e., the process frame pool is not empty). If so, a frame is removed from the process frame pool and the frame number is inserted into the page-table entry corresponding to the logical page number. In addition, the other fields of the page-table entry are set appropriately. The function counts this page access as a page fault

        //If the page being referenced is not in memory and there are no free frames for the process, a page needs to be replaced. The function selects among all the pages of the process that are currently in memory (i.e., they have valid bits as true) the page that has the smallest arrival_timestamp. It marks that page-table entry as invalid, along with setting the arrival_timestamp, last_access_timestamp and reference_count to -1. It then sets the frame_number of the page-table entry of the newly-referenced page to the newly freed frame. It also sets the arrival_timestamp, the last_access_timestamp and the reference_count fields of the page-table entry appropriately. The function counts this page access as a page fault.


        if (!local_page_table[page_number].is_valid) {
            page_faults++;
            if (frames_in_use < frame_cnt) {
                frames_in_use++;
            } else {
                process_page_access_fifo(local_page_table, &table_cnt, page_number, local_frame_pool, &local_frame_cnt, current_timestamp);
            }
        } else {
            process_page_access_fifo(local_page_table, &table_cnt, page_number, local_frame_pool, &local_frame_cnt, current_timestamp);
        }
        current_timestamp++;
    }

    // The function returns the total number of page faults encountered in the simulation.
    return page_faults;
}

/*
 * This function implements the logic to process a page access in a system that uses the
 * Least-Recently-Used (LRU) policy for page replacement. It takes four inputs:
 * 1) process page table, 2) logical page number, 3) process frame pool, and 4) current timestamp.
 * The function determines the memory frame number for the logical page and returns this number.
 */
int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    // First check if the page being referenced is already in memory
    if (page_table[page_number].is_valid) {
        // If so, update the last access timestamp and reference count, then return the frame number
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // If the page is not in memory, check if there are any free frames
    if (*frame_cnt > 0) {
        // If there are free frames, allocate one to this page
        int frame = frame_pool[--(*frame_cnt)];
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame;
    }

    // If there are no free frames, a page needs to be replaced
    // Select the page with the smallest last_access_timestamp (least recently used)
    int least_recent_timestamp = current_timestamp;
    int least_recent_page = -1;
    
    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].last_access_timestamp < least_recent_timestamp) {
            least_recent_timestamp = page_table[i].last_access_timestamp;
            least_recent_page = i;
        }
    }

    // Replace the least recently used page with the new page
    int frame = page_table[least_recent_page].frame_number;
    page_table[least_recent_page].is_valid = 0;
    page_table[least_recent_page].frame_number = -1;
    page_table[least_recent_page].arrival_timestamp = -1;
    page_table[least_recent_page].last_access_timestamp = -1;
    page_table[least_recent_page].reference_count = -1;

    // Set up the new page table entry
    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    // Return the frame number of the newly allocated page
    return frame;
}

/*
 * This function simulates the processing of a sequence of page references in a system that uses
 * the Least-Recently-Used (LRU) policy for page replacement. It takes three inputs:
 * 1) process page table, 2) sequence of logical page numbers, and 3) process frame pool.
 * The function returns the estimated number of page faults for the reference string,
 * with respect to the pool of frames allocated to the process.
 */
int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int page_faults = 0;
    int current_timestamp = 1;
    int frames_in_use = 0;

    // Create a copy of the page table to simulate on
    struct PTE local_page_table[TABLEMAX];
    for (int i = 0; i < table_cnt; i++) {
        local_page_table[i] = page_table[i];
        if (local_page_table[i].is_valid) {
            frames_in_use++;
        }
    }

    // Create a copy of the frame pool and its count
    int local_frame_pool[POOLMAX];
    int local_frame_cnt = frame_cnt;
    for (int i = 0; i < frame_cnt; i++) {
        local_frame_pool[i] = frame_pool[i];
    }

    // Process each page reference in the sequence
    for (int i = 0; i < reference_cnt; i++) {
        int page_number = reference_string[i];
        
        // Check if the page is not in memory (page fault)
        if (!local_page_table[page_number].is_valid) {
            page_faults++;
            if (frames_in_use < frame_cnt) {
                frames_in_use++;
            }
        }
        
        // Process the page access, updating the page table
        process_page_access_lru(local_page_table, &table_cnt, page_number, local_frame_pool, &local_frame_cnt, current_timestamp);
        
        // Increment the timestamp for the next iteration
        current_timestamp++;
    }

    // Return the total number of page faults encountered in the simulation
    return page_faults;
}

/*
 * This function implements the logic to process a page access in a system that uses the
 * Least-Frequently-Used (LFU, also known as Not-Frequently-Used) policy for page replacement.
 * It takes four inputs: 1) process page table, 2) logical page number, 3) process frame pool,
 * and 4) current timestamp. The function determines the memory frame number for the logical
 * page and returns this number.
 */
int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    // First check if the page being referenced is already in memory
    if (page_table[page_number].is_valid) {
        // If so, update the last access timestamp and reference count, then return the frame number
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // If the page is not in memory, check if there are any free frames
    if (*frame_cnt > 0) {
        // If there are free frames, allocate one to this page
        int frame = frame_pool[--(*frame_cnt)];
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame;
    }

    // If there are no free frames, a page needs to be replaced
    // Select the page with the smallest reference count (least frequently used)
    int least_frequent_count = INT_MAX;
    int least_frequent_page = -1;
    int oldest_timestamp = current_timestamp;
    
    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid) {
            // If this page has a lower reference count, or equal count but older arrival time
            if (page_table[i].reference_count < least_frequent_count ||
                (page_table[i].reference_count == least_frequent_count && 
                 page_table[i].arrival_timestamp < oldest_timestamp)) {
                least_frequent_count = page_table[i].reference_count;
                least_frequent_page = i;
                oldest_timestamp = page_table[i].arrival_timestamp;
            }
        }
    }

    // Replace the least frequently used page with the new page
    int frame = page_table[least_frequent_page].frame_number;
    page_table[least_frequent_page].is_valid = 0;
    page_table[least_frequent_page].frame_number = -1;
    page_table[least_frequent_page].arrival_timestamp = -1;
    page_table[least_frequent_page].last_access_timestamp = -1;
    page_table[least_frequent_page].reference_count = -1;

    // Set up the new page table entry
    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    // Return the frame number of the newly allocated page
    return frame;
}

/*
 * This function simulates the processing of a sequence of page references in a system that uses
 * the Least-Frequently-Used (LFU, also known as Not-Frequently-Used) policy for page replacement.
 * It takes three inputs:
 * 1) process page table
 * 2) sequence of logical page numbers
 * 3) process frame pool
 * The function returns the estimated number of page faults for the reference string,
 * with respect to the pool of frames allocated to the process.
 */
int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int page_faults = 0;
    int current_timestamp = 1;

    // Create a copy of the frame pool and its count for simulation
    int local_frame_pool[POOLMAX];
    int local_frame_cnt = frame_cnt;
    for (int i = 0; i < frame_cnt; i++) {
        local_frame_pool[i] = frame_pool[i];
    }

    // Process each page reference in the sequence
    for (int i = 0; i < reference_cnt; i++) {
        int page_number = reference_string[i];
        
        // Check if the page is not in memory (page fault)
        if (!page_table[page_number].is_valid) {
            page_faults++;
        }
        
        // Process the page access, updating the page table
        process_page_access_lfu(page_table, &table_cnt, page_number, local_frame_pool, &local_frame_cnt, current_timestamp);
        
        // Increment the timestamp for the next iteration
        current_timestamp++;
    }

    // Return the total number of page faults encountered in the simulation
    return page_faults;
}