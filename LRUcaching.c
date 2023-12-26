#include "header.h"
void addtolistof20(node_LRU **head, char *command, int exist, char *path1, char *path2, char *ip1, char *ip2, int port1, int port2)
{
    if (exist == 0) // not present in cache list
    {
        if (*head == NULL) // empty list
        {
            // printf("Hello\n");
            node_LRU temp = (node_LRU)malloc(sizeof(struct node_LRU));
            strcpy(temp->ip1, ip1);
            if (ip2 != NULL)
            {
                strcpy(temp->ip2, ip2);
            }
            else
            {
                strcpy(temp->ip2, "\0");
            }
            strcpy(temp->path1, path1);
            if (path2 != NULL)
            {
                strcpy(temp->path2, path2);
            }
            else
            {
                strcpy(temp->path2, "\0");
            }
            temp->port1 = port1;
            if (port2 != -1)
            {
                temp->port2 = port2;
            }
            else
            {
                temp->port2 = -1;
            }
            temp->next = NULL;
            *head = temp;
        }
        else
        {
            int num = 0; // number of nodes in cache list
            node_LRU start = *head;
            while (start != NULL)
            {
                num++;
                start = start->next;
            }
            if (num < 20)
            {
                node_LRU temp = (node_LRU)malloc(sizeof(struct node_LRU));
                strcpy(temp->ip1, ip1);
                if (ip2 != NULL)
                {
                    strcpy(temp->ip2, ip2);
                }
                else
                {
                    strcpy(temp->ip2, "\0");
                }
                strcpy(temp->path1, path1);
                if (path2 != NULL)
                {
                    strcpy(temp->path2, path2);
                }
                else
                {
                    strcpy(temp->path2, "\0");
                }
                temp->port1 = port1;
                if (port2 != -1)
                {
                    temp->port2 = port2;
                }
                else
                {
                    temp->port2 = -1;
                }
                temp->next = *head;
                *head = temp;
            }
            else if (num == 20)
            {
                node_LRU temp = (node_LRU)malloc(sizeof(struct node_LRU));
                strcpy(temp->ip1, ip1);
                if (ip2 != NULL)
                {
                    strcpy(temp->ip2, ip2);
                }
                else
                {
                    strcpy(temp->ip2, "\0");
                }
                strcpy(temp->path1, path1);
                if (path2 != NULL)
                {
                    strcpy(temp->path2, path2);
                }
                else
                {
                    strcpy(temp->path2, "\0");
                }
                temp->port1 = port1;
                if (port2 != -1)
                {
                    temp->port2 = port2;
                }
                else
                {
                    temp->port2 = -1;
                }
                node_LRU end = *head;
                node_LRU secondlastend = *head;
                while (secondlastend->next->next != NULL)
                {
                    secondlastend = secondlastend->next;
                }
                while (end->next != NULL)
                {
                    end = end->next;
                }
                free(end);
                // end = NULL;
                secondlastend->next = NULL; // removed from last
                temp->next = *head;
                *head = temp;
            }
        }
    }
    else // present in cache list
    {
        int index = 1;
        node_LRU cur = *head;
        while (cur != NULL)
        {
            if (strcmp(command, "COPY") == 0)
            {
                if ((strcmp(cur->path1, path1) == 0) && (strcmp(cur->path2, path2) == 0) && (strcmp(cur->ip1, ip1) == 0) && (strcmp(cur->ip2, ip2) == 0) && (cur->port1 == port1) && (cur->port2 == port2))
                {
                    break;
                }
                else
                {
                    index++;
                    cur = cur->next;
                }
            }
            else
            {
                if ((strcmp(cur->path1, path1) == 0) && (strcmp(cur->ip1, ip1) == 0) && (cur->port1 == port1))
                {
                    break;
                }
                else
                {
                    index++;
                    cur = cur->next;
                }
            }
        }
        if (index != 1)
        {
            node_LRU prev = *head;
            node_LRU cur = *head;
            int i = 1;
            while (i != index)
            {
                prev = cur;
                cur = cur->next;
                i++;
            }
            prev->next = cur->next;
            cur->next = *head;
            *head = cur;
        }
    }
    return;
}
store search(char *command, char *path1, char *path2, node_LRU* head, store packet)
{
    if (*head == NULL)
    {
        return NULL;
    }
    else
    {
        int flag = 0;
        node_LRU start = *head;
        while (start != NULL)
        {
            if (strcmp(command, "COPY") == 0)
            {
                if ((strcmp(start->path1, path1) == 0) && (strcmp(start->path2, path2) == 0))
                {
                    packet->port1 = start->port1;
                    packet->port2 = start->port2;
                    strcpy(packet->ip1, start->ip1);
                    strcpy(packet->ip2, start->ip2);
                    flag = 1;
                    break;
                }
            }
            else
            {
                if ((strcmp(start->path1, path1) == 0))
                {
                    packet->port1 = start->port1;
                    strcpy(packet->ip1, start->ip1);
                    flag = 1;
                    break;
                }
            }
            start = start->next;
        }
        if (flag == 0)
        {
            return NULL;
        }
        else
        {
            return packet;
        }
    }
}

// int main()
// {
//     node_LRU head=NULL;
//     addtolistof20(&head,"CREATE",0,"file.txt",NULL,"124.24.4.3",NULL,4545,-1);
//     addtolistof20(&head,"WRITE",0,"file.c",NULL,"127.0.0.1",NULL,8080,-1);
//     addtolistof20(&head,"COPY",0,"file.c","file2.txt","127.0.0.1","12.56",8080,5050);
//      store str;
//     str=(store)malloc(sizeof(struct store));
//     str=search("COPY","file.c","file2.txt",&head,str);
//     if(str==NULL)
//     {printf("Not found\n");
//     return 0;}
//     printf("%s %s %d %d\n",str->ip1,str->ip2,str->port1,str->port2);
//     // node_LRU ptr=head;
//     // while(ptr!=NULL)
//     // {
//     //     printf("%s %s %s %d \n",ptr->command,ptr->path1,ptr->path1,ptr->port1);
//     //     ptr=ptr->next;
//     // }
//     return 0;


    
// }
