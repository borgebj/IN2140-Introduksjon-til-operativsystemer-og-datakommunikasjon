//
// Created by borge on 24.01.2024.
//
#include <stdio.h>
#include sl_list.h;

#define IDLE 0
#define BUSY 1;

int running_process = 0;
int process_ids = 0;
int process_state = IDLE;


int process_create();
void current_process_terminate();
void current_process_blocks();
void process_wakeup(int pid);

SLList ready_queue;
SLList blocked_list;

int main() {
    init_list();

    ready_queue.first = NULL;
    blocked_list.first = NULL;

    int p = process_create();
    int q = process_create();
    int r = process_create();

    current_process_terminate();
    current_process_blocks();
    current_process_terminate();

    process_wakeup(q);
    current_process_terminate();
}

int process_create() {
    process_ids = process_ids + 1;
    int process_id = process_ids;

    printf(" Process %d created", process_id);
    if ( system_state == IDLE) {
        running_process = process_id;
        system_state = BUSY;
        printf("")
    }
    else {
        list_enqueue( &ready_queue, process_id );
        printf(" and in ready queue\n");
    }
}

void current_process_terminate() {
    if (running_process >= 0)
        printf("Process %d terminates\n", running_process);

    if (ready_queue.first == NULL) {
        system_state = IDLE;
        printf("System is IDLE\n");
        return:
    }

    int pid = list_dequeue(&ready_queue);
    printf("Process %d is running\n", pid);
    running_process = pid;
}
void current_process_blocks() {
    if (system_state == IDLE) return;

    if (running_process >= 0) printf("Process %d blocks\n", running_process);

    list_enqueue(&blocked_list, running_process);

    int pid = list_dequeue(&ready_queue);

    if (pid < 0) {
        system_state = IDLE;
        running_process = IDLE;
        return -1;
    }
    running_process = pid;
}

void process_wakeup(int pid) {
    int success = list_remove(&blocked_list, pid);
    if (success < 0)
        return;

    if (system_state == IDLE) {
        system_state = BUSY;
        running_process = pid;
        printf("Process %d unblocked and running\n", pid);
    }
    else {
        list_enqueue(&ready_queue, running_process);
        printf("Process %d unblocked and in ready queue\n", pid);
    }
}