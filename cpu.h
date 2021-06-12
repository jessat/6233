/*
 
 Lab 3 - CPU scheduling
 Created by jessat on 5/29/21
 
 This lab project addresses the implementation of CPU-scheduling algorithms in
 an operating system.  To determine the schedule of execution for the processes
 in an operating system, we consider three policies:
 - Priority-based Preemptive Scheduling (PP)
 - Shortest-Remaining-Time-Next Preemptive Scheduling (SRTP)
 - Round-Robin Scheduling (RR)
 In order to implement the above policies, we need to develop methods that handle
 arrival of processes for execution and the completion of process execution.
 
 */

#include <stdio.h>
#include <stdbool.h>
#include "oslabs.h"


const struct PCB nullpcb = {0,0,0,0,0,0};


//helper to test for null PCB
int is_pcb_null (struct PCB pcb) {
    if (pcb.process_id == 0 && pcb.arrival_timestamp == 0 &&
        pcb.total_bursttime == 0 && pcb.execution_starttime == 0 &&
        pcb.execution_endtime == 0 && pcb.remaining_bursttime == 0 &&
        pcb.process_priority == 0 ) {
        return true;
    }
    else return false;
}


struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt,
                                     struct PCB current_process, struct PCB new_process,
                                     int timestamp) {

    if (is_pcb_null(current_process)) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else {
        if (new_process.process_priority >= current_process.process_priority){
            new_process.execution_starttime = 0;
            new_process.execution_endtime = 0;
            new_process.remaining_bursttime = new_process.total_bursttime;
            
            ready_queue[*queue_cnt] = new_process;
            (*queue_cnt)++;
            return current_process;
        }
        else {
            new_process.execution_starttime = timestamp;
            new_process.execution_endtime = timestamp + new_process.total_bursttime;
            new_process.remaining_bursttime = new_process.total_bursttime;
            
            current_process.execution_endtime = 0;
            current_process.remaining_bursttime = new_process.total_bursttime;
            ready_queue[*queue_cnt] = current_process;
            (*queue_cnt)++;
            return new_process;
        }
    }
}


struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt,
                                        int timestamp) {
        
    if (*queue_cnt == 0) // if ready queue is empty, return null PCB
        return nullpcb;
    else { //else find highest priority process, remove it from queue, return it
        int high_priority = ready_queue[0].process_priority;
        int temp = 0;
        for (int i = 1; i < *queue_cnt; i++) {
            if (ready_queue[i].process_priority <= high_priority){
                high_priority = ready_queue[i].process_priority;
                temp = i;
            }
        }
        
        struct PCB next_process = ready_queue[temp];
        next_process.execution_starttime = timestamp;
        next_process.execution_endtime = timestamp + next_process.total_bursttime;
        
        for (int i = temp; i < *queue_cnt - 1; i++) {
            ready_queue[i] = ready_queue[i+1];
        }
        *queue_cnt -= 1;
        
        return next_process;
    }
 }


struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt,
                                       struct PCB current_process, struct PCB new_process,
                                       int time_stamp) {

    if (is_pcb_null(current_process)) {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else { //if there is a running process
        if (new_process.total_bursttime >= current_process.remaining_bursttime){
            new_process.execution_starttime = 0;
            new_process.execution_endtime = 0;
            new_process.remaining_bursttime = new_process.total_bursttime;
            
            ready_queue[*queue_cnt] = new_process;
            (*queue_cnt)++;
            
            return current_process;
        }
        else {
            new_process.execution_starttime = time_stamp;
            new_process.execution_endtime = time_stamp + new_process.total_bursttime;
            new_process.remaining_bursttime = new_process.total_bursttime;
      
            current_process.execution_starttime = 0;
            current_process.execution_endtime = 0;
            current_process.remaining_bursttime = new_process.total_bursttime;
            ready_queue[*queue_cnt] = current_process;
            (*queue_cnt)++;
            
            return new_process;
        }
    }
}


struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX],
                                          int *queue_cnt, int timestamp) {
    
    if (*queue_cnt == 0) {
        return nullpcb;
    }
    else { //find shortest burst time process, remove it from queue, return it
        int smallest_bt = ready_queue[0].remaining_bursttime;
        int temp = 0;
        for (int i = 1; i < *queue_cnt; i++) {
            if (ready_queue[i].remaining_bursttime <= smallest_bt){
                smallest_bt = ready_queue[i].process_priority;
                temp = i;
            }
        }
        
        struct PCB next_process = ready_queue[temp];
        next_process.execution_starttime = timestamp;
        next_process.execution_endtime = timestamp + next_process.total_bursttime;
        
        for (int i = temp; i < *queue_cnt - 1; i++) {
            ready_queue[i] = ready_queue[i+1];
        }
        
        *queue_cnt -= 1;
        
        return next_process;
    }
}


struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt,
                                     struct PCB current_process, struct PCB new_process,
                                     int timestamp, int time_quantum) {

    if (is_pcb_null(current_process)) {
        new_process.execution_starttime = timestamp;
        if (time_quantum < new_process.total_bursttime) {
            new_process.execution_endtime = timestamp + time_quantum;
        }
        else {
            new_process.execution_endtime = timestamp + new_process.total_bursttime;
        }
        
        new_process.remaining_bursttime = new_process.total_bursttime;
        
        return new_process;
    }
    else {
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        
        return current_process;
    }
}


struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt,
                                        int time_stamp, int time_quantum) {
    
    if (*queue_cnt == 0) {
        return nullpcb;
    }
    else { //else find smallest remaining burst time, remove it from rq, return it
        int earliest_at = ready_queue[0].arrival_timestamp;
        int temp = 0;
        for (int i = 1; i < *queue_cnt; i++) { //can skip 0 in rq
            if (ready_queue[i].arrival_timestamp <= earliest_at){
                earliest_at = ready_queue[i].process_priority;
                temp = i;
            }
        }
        
        struct PCB next_process = ready_queue[temp];
        next_process.execution_starttime = time_stamp;
        
        if (time_quantum < next_process.remaining_bursttime) {
            next_process.execution_endtime = time_stamp + time_quantum;
        }
        else {
            next_process.execution_endtime = time_stamp + next_process.remaining_bursttime;
        }
        
        for (int i = temp; i < *queue_cnt - 1; i++) {
            ready_queue[i] = ready_queue[i+1];
        }
        
        *queue_cnt -= 1;
        
        return next_process;
    }
 }
