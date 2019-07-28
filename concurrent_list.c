#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

typedef struct node {
    int value;
    struct node* next;
    pthread_mutex_t node_lock;
    // add more fields
}Node;

typedef struct list {
    struct node* first;
    pthread_mutex_t list_lock;

    // add fields
}List;

void print_node(node* node)
{
    // DO NOT DELETE
    if(node)
    {
        printf("%d ", node->value);
    }
}

list* create_list()
{
    // add code here
    list *l;
    if((l=(list *)malloc (sizeof(list)))==NULL)
    {
        printf("bad allocation of the list, function creat_list \n");
        exit(-1);
    }
    l->first=NULL;
    if(pthread_mutex_init(&l->list_lock,NULL)!=0)
    {
        printf("bad initialization of list mutex. function: creat_list. \n");
        delete_list(l);
        exit(-1);
    }
    return l; // REPLACE WITH YOUR OWN CODE
}

void delete_list(list* list)
{
    node *temp;
    if(list==NULL)
    {
     /*   printf("there is no list! \n");*/
        return;
    }
    pthread_mutex_lock(&list->list_lock);
    temp=list->first;
    if(temp!=NULL)
        pthread_mutex_lock(&temp->node_lock);
    pthread_mutex_unlock(&list->list_lock);
    while(temp)
    {
        if(temp->next!=NULL)
            pthread_mutex_lock(&temp->next->node_lock);
        pthread_mutex_unlock(&temp->node_lock);
        if(pthread_mutex_destroy(&temp->node_lock)!=0)
        {
            printf("bad removing one of the node's mutex. function: delete list \n");
        }
        temp=temp->next;
        free(list->first);
        list->first=temp;
    }
    if(pthread_mutex_destroy(&list->list_lock)!=0)
    {
        printf("bad removing the list lock. function: delete list \n");
    }
    free(list);
    return;
    // add code here
}

void insert_value(list* list, int value)
{
    node *new_node, *temp_node;
    if(list==NULL)
    {
       /* printf("there is no list! \n");*/
        return;
    }
    pthread_mutex_lock(&list->list_lock);
    if((new_node=(node *)malloc (sizeof(node)))==NULL)
    {
        printf("bad alloction of node, function: insert_value");
        delete_list(list);
        exit(-1);
    }
    new_node->value=value;
    if(pthread_mutex_init(&new_node->node_lock,NULL)!=0)
    {
        printf("bad initialization of new node lock. function: insert_value \n");
        free(new_node);
        delete_list(list);
        exit(-1);
    }
    if(list->first==NULL)
    {
        new_node->next=NULL;
        list->first=new_node;
        pthread_mutex_unlock(&list->list_lock);
        return;
    }
    if (list->first->value>value)
    {
        new_node->next=list->first;
        list->first=new_node;
        pthread_mutex_unlock(&list->list_lock);
        return;
    }
    pthread_mutex_lock(&list->first->node_lock);
    pthread_mutex_unlock(&list->list_lock);
    temp_node=list->first;
    while(temp_node->next!=NULL && temp_node->next->value<value)
    {
        pthread_mutex_lock(&temp_node->next->node_lock);
        pthread_mutex_unlock(&temp_node->node_lock);
        temp_node=temp_node->next;
    }
    new_node->next=temp_node->next;
    temp_node->next=new_node;
    pthread_mutex_unlock(&temp_node->node_lock);
    return;
    // add code here
}

void remove_value(list* list, int value)
{
    if(list==NULL)
    {
       /* printf("there is no list! \n");*/
        return;
    }
    pthread_mutex_lock(&list->list_lock);
    node *temp_node, *index_node;
    if(list->first==NULL)
    {
        pthread_mutex_unlock(&list->list_lock);
        return;
    }
    index_node=list->first;
    pthread_mutex_lock(&index_node->node_lock);
    if(index_node->value==value)
    {
        list->first=index_node->next;
        pthread_mutex_unlock(&list->list_lock);
        pthread_mutex_unlock(&index_node->node_lock);
        if(pthread_mutex_destroy(&index_node->node_lock)!=0)
        {
            printf("bad destroying lock 1. function: remove_value \n");
            delete_list(list);
            exit(-1);
        }
        free(index_node);
        return;

    }
    pthread_mutex_unlock(&list->list_lock);
    while(index_node->next!=NULL && index_node->next->value < value)
    {
        pthread_mutex_lock(&index_node->next->node_lock);
        pthread_mutex_unlock(&index_node->node_lock);
        index_node=index_node->next;
    }
    if(index_node->next==NULL ||index_node->next->value>value)
    {
        pthread_mutex_unlock(&index_node->node_lock);
        return;
    }
    if(index_node->next->value==value)
    {
        pthread_mutex_lock(&index_node->next->node_lock);
        temp_node=index_node->next;
        index_node->next=temp_node->next;
        pthread_mutex_unlock(&index_node->node_lock);
        pthread_mutex_unlock(&temp_node->node_lock);
        if(pthread_mutex_destroy(&temp_node->node_lock)!=0)
        {
            printf("bad destroying lock 2. function: remove_value \n");
            delete_list(list);
            exit(-1);
        }
        free(temp_node);
    }
    return;
    // add code here
}

void print_list(list* list)
{
    // add code here
    node *temp;
    if(list==NULL)
    {
        /*printf("there is no list! \n");*/
        return;
    }
    pthread_mutex_lock(&list->list_lock);
    temp=list->first;
    if(temp!=NULL)
        pthread_mutex_lock(&temp->node_lock);
    pthread_mutex_unlock(&list->list_lock);
    while(temp)
    {
        print_node(temp);
        if(temp->next!=NULL)
            pthread_mutex_lock(&temp->next->node_lock);
        pthread_mutex_unlock(&temp->node_lock);
        temp=temp->next;
    }
    printf("\n"); // DO NOT DELETE
}

void count_list(list* list, int (*predicate)(int))
{
    int count = 0; // DO NOT DELETE
    // add code here
    node *temp;
    if(list==NULL)
    {
     /*   printf("there is no list! \n");*/
        return;
    }
    pthread_mutex_lock(&list->list_lock);
    temp=list->first;
    if(temp!=NULL)
        pthread_mutex_lock(&temp->node_lock);
    pthread_mutex_unlock(&list->list_lock);
    while(temp)
    {
        count+= predicate(temp->value);
        if(temp->next!=NULL)
            pthread_mutex_lock(&temp->next->node_lock);
        pthread_mutex_unlock(&temp->node_lock);
        temp= temp->next;
    }
    printf("%d items were counted \n", count); // DO NOT DELETE
}
