/*
 
 Lab 3 - CPU scheduling
 Created by jessat on 5/31/21
 
 This lab project addresses the various memory-management schemes in an
 operating system.  In this lab project, we address the various ways in
 which memory can be allocated (based on different schemes:
 - Best Fit
 - First Fit
 - Next Fit
 - Worst (Largest) Fit).
 In addition, we also develop a method to manage the release of memory
 blocks by processes.
 
 */
 

#include <stdio.h>
#include <stdbool.h>
#include "oslabs.h"

const struct MEMORY_BLOCK nullmb = {0,0,0,0};

//helper to set a memory block
struct MEMORY_BLOCK set_mb (int sa, int ea, int ss, int pid) {
    struct MEMORY_BLOCK mb;
    mb.start_address = sa;
    mb.end_address = ea;
    mb.segment_size = ss;
    mb.process_id = pid;
    return mb;
}

//helper to split mb
struct MEMORY_BLOCK split_mb (int ind, int request_size, int process_id, int *map_cnt,
                              struct MEMORY_BLOCK memory_map[MAPMAX]) {
        
    int sa = memory_map[ind].start_address;
    int ea = memory_map[ind].end_address;
    int new_ea = sa + (request_size - 1);
    int ss = memory_map[ind].segment_size - request_size;
    int pid = memory_map[ind].process_id;
    
    struct MEMORY_BLOCK new_mem = {new_ea + 1,ea,ss,pid};
    memory_map[ind] = set_mb(sa,new_ea,request_size,process_id); //update old mem
    memory_map[*map_cnt] = new_mem;
    (*map_cnt)++;
    
    return memory_map[ind];
}


struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id) {
    
    int ind = - 1;
    
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].segment_size > request_size) {
            if (ind == -1) {
                ind = i;
            }
            else if (memory_map[ind].segment_size > memory_map[i].segment_size) {
                ind = i;
            }
        }
        else if (memory_map[i].segment_size == request_size) {
            return memory_map[i];
        }
        else { //no free block as large as requested size
            return nullmb;
        }
    }
    
    return split_mb(ind, request_size, process_id, map_cnt, &memory_map[ind]);
}


struct MEMORY_BLOCK first_fit_allocate(int request_size,
                                       struct MEMORY_BLOCK memory_map[MAPMAX],
                                       int *map_cnt, int process_id) {
        
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].segment_size > request_size) {
            return split_mb(i, request_size, process_id, map_cnt, &memory_map[i]);
        }
        else if (memory_map[i].segment_size == request_size) {
            return memory_map[i];
        }
    }
    return nullmb;
}


struct MEMORY_BLOCK worst_fit_allocate(int request_size,
                                       struct MEMORY_BLOCK memory_map[MAPMAX],
                                       int *map_cnt, int process_id) {
    
    int ind = - 1;
    
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].segment_size > request_size) {
            if (ind == -1) {
                ind = i;
            }
            else if (memory_map[ind].segment_size < memory_map[i].segment_size) {
                ind = i;
            }
        }
        else if (memory_map[i].segment_size == request_size) {
            return memory_map[i];
        }
        else { //no free block as large as requested size
            return nullmb;
        }
    }

    return split_mb(ind, request_size, process_id, map_cnt, &memory_map[ind]);
}

//next fit allocate
struct MEMORY_BLOCK next_fit_allocate(int request_size,
                                      struct MEMORY_BLOCK memory_map[MAPMAX],
                                      int *map_cnt, int process_id, int last_address) {
    
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].start_address >= last_address) {
            if (memory_map[i].segment_size > request_size) {
                return split_mb(i, request_size, process_id, map_cnt, &memory_map[i]);
            }
            else if (memory_map[i].segment_size == request_size) {
                return memory_map[i];
            }
        }
    }
    return nullmb;
}

//release memory
void release_memory(struct MEMORY_BLOCK freed_block,
                    struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt) {
        
    for (int i = 0; i < *map_cnt; i++) {
        if (freed_block.process_id == memory_map[i].process_id) {
            if ((i + 1) < *map_cnt && (i-1) >= 0) {
                int fb_ss = freed_block.segment_size;
                memory_map[i-1].end_address = memory_map[i+1].end_address;
                memory_map[i-1].segment_size += (fb_ss + memory_map[i+1].segment_size);
                memory_map[i-1].process_id = memory_map[i+1].process_id;
                for (int j = i; j < *map_cnt; j++) {
                    memory_map[j] = memory_map[j+2];
                }
                *map_cnt -= 2;
            }
        }
    }
}
