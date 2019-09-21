//
// Created by trcoo on 9/3/19.
//

#include "Scheduler.h"
#include "JobQueue.h"
#include <stdio.h>
#include <pthread.h>


void* run_scheduler(void *_data){
    thread_data_t *data;
    data = (thread_data_t*)_data;
    //Set the default policy to first come first serve by setting the comparator to SchedulingPolicies::compare_age
    schedule_comparator = &compare_age;

    while (*data->active){

    }
    printf("Terminating Scheduler\n");
    return NULL;
}

void post(Job* job){
        Node* new_node = malloc(sizeof(Node));
        new_node->next = NULL;
        new_node->job = job;
        insert(new_node);
}

//Used to set the new policy, where fcfs, sjf, and priority are mapped to integers 0, 1, and 2 respectively.
void set_scheduling(int p){
    switch (p){
        case 0: schedule_comparator = &compare_age; break;
        case 1: schedule_comparator = &compare_duration; break;
        case 2: schedule_comparator = &compare_priority; break;
        default: printf("ERROR: SET_SCHEDULING METHOD GIVEN UNKNOWN VALUE %d!\n", p);
    }
    sort();
    printf("Queue has been reordered.\n");
    print_job_queue();
}


void insert(Node* new_node){
    if (job_queue_length() == 0){
        *get_queue() = new_node;
    }else{
        insert_aux(new_node, get_queue());
    }
}

//A recursive helper function for insert.
//It is initially called by insert and the node given should refer to the beginning of the queue.
//Each recursive call passes in the next node after the on passed in, so that the job is compared against each
//node in the queue
void insert_aux(Node* new_node, Node** current_node){
    Job* new_job = new_node->job;
    Node* next_node = ((*current_node)->next); //The node after the one being compared currently
    Job selected_job = *(*current_node)->job; //The current job in the queue to which we compare the new job
    if (schedule_comparator(*new_job, selected_job) < 0){
        //Entering this block indicates that the new job has a lower priority than the current node's job.
        if (next_node != NULL && schedule_comparator(*new_job, *next_node->job) < 0) {
            //Entering this block indicates that the new job also has a lower priority than the job after the current job in the q. So we recurse.
            insert_aux(new_node, &next_node);
        } else{
            //Entering this block indicates that the new job has a greater or equal priority than the q element AFTER the current q element we're comparing against.
            //This branch is only reached if the new job has a lower priority than the current node job AND a greater priority than the next node's job.
            //As a result, we initialize a node for the job, and place it between the current node and the next node.
            new_node->next = next_node;
            (*current_node)->next = new_node;
        }
    }else{
        //Entering this block indicates that the new job has a greater priority than the current node's job.
        //As a result, we initialize a node for the job, and place it before the current node.
        new_node->next = *current_node;
        if (current_node == get_queue()){
            //If the current node is the very first node in the q, then the global q pointer needs to be updated to point to the new node.
            *get_queue() = new_node;
        }
    }
}

void sort(){
    int length = job_queue_length();
    //Create a copy of all Node*'s. Otherwise, it will be impossible to retrieve the nodes after their pointers are cleared with clear_node_linke_length();
    Node* nodes[length];
    for (int i = 0; i < length; i ++){
        nodes[i] = get_node(i);
    }
    //Set all 'next' members of each Node to NULL, so that they can be properly reset.
    clear_node_links();
    //Insert them one by one. insert() automatically places them in the correct order.
    for (int i = 0; i < length; i ++){
        insert(nodes[i]);
    }
}