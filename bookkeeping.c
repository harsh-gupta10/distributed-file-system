#include "header.h"
void addtolog(log head, int index, char *command, char *path1, char *path2, char *ip1, char *ip2, int port1, int port2, char* status, int ACK)
{
    if (head->next == NULL)
    {
        log temp = (log)malloc(sizeof(struct log));
        strcpy(temp->command, command);
        strcpy(temp->path1, path1);
        if(path2!=NULL)
        strcpy(temp->path2, path2);
        strcpy(temp->ip1, ip1);
        if(ip2!=NULL)
        strcpy(temp->ip2, ip2);
        temp->index_in_array_of_listed_SS = index;
        strcpy(temp->status,status);
        temp->port1 = port1;
        if(port2!=-1)
        temp->port2 = port2;
        temp->ack=ACK;
        temp->next = NULL;
        head->next = temp;
    }
    else
    {
        log temp = (log)malloc(sizeof(struct log));
        strcpy(temp->command, command);
        strcpy(temp->path1, path1);
        if(path2!=NULL)
        strcpy(temp->path2, path2);
        strcpy(temp->ip1, ip1);
        if(ip2!=NULL)
        strcpy(temp->ip2, ip2);
        temp->index_in_array_of_listed_SS = index;
        strcpy(temp->status,status);
        temp->port1 = port1;
        if(port2!=-1)
        temp->port2 = port2;
        temp->ack=ACK;
        log end = head->next;
        while (end->next != NULL)
        {
            end = end->next;
        }
        end->next=temp;
        temp->next=NULL;
    }
    
}
