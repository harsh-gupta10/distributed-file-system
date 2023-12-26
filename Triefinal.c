#include "Trie.h"
#include "header.h"
void createTrieNode(char *name, struct TrieNode **root)
{
    (*root) = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    if (*root)
    {
        strcpy((*root)->name, name);
        // printf("%s\n", (*root)->name);
        (*root)->pointtohead = NULL;
    }
    return;
}
int isdir(char *ipart)
{
    char store[500];
    strcpy(store, ipart);
    int fl = 0;
    for (int i = 0; store[i] != '\0'; i++)
    {
        if (store[i] == '.')
        {
            fl = 1;
            break;
        }
    }
    if (fl)
    {
        return 0;
    }
    return 1;
}
void insertPath(struct TrieNode *root, char *path)
{
    char delimiter = '/';
    char input[4096];
    strcpy(input, path);
    int num_substrings = 0;
    char *temp = input;
    while (*temp)
    {
        if (*temp == delimiter)
        {
            num_substrings++;
        }
        temp++;
    }
    num_substrings++;
    char **substrings = (char **)malloc(num_substrings * sizeof(char *));
    for (int i = 0; i < num_substrings; i++)
    {
        substrings[i] = NULL;
    }
    char *token = strtok(input, &delimiter);
    int index = 0;
    while (token)
    {
        substrings[index] = (char *)malloc(strlen(token) + 1);
        strcpy(substrings[index], token);
        token = strtok(NULL, &delimiter);
        index++;
    }
    struct TrieNode *node = root;
    // printf("%s\n",node->name);
    int flag = 0;
    // printf("%d\n", num_substrings);
    for (int i = 0; i < num_substrings; i++)
    {
        char ipart[500];
        strcpy(ipart, substrings[i]);
        struct node *newnode;
        if (node->pointtohead == NULL)
        {
            newnode = (struct node *)malloc(sizeof(struct node));
            newnode->next = NULL;
            if (isdir(ipart))
            {
                // newnode->pointer = (struct TrieNode *)malloc(sizeof(struct TrieNode));
                // strcpy(newnode->pointer->name, ipart);
                // newnode->pointer->pointtohead = NULL;
                createTrieNode(ipart, &newnode->pointer);
                newnode->isfile = 0;
            }
            else
            {
                newnode->pointer = NULL;
                newnode->isfile = 1;
                strcpy(newnode->filename, ipart);
            }
            node->pointtohead = newnode;
        }
        else
        {
            int fl = 0;
            struct node *cur = node->pointtohead;
            struct node *prev = node->pointtohead;
            while (cur != NULL)
            {
                if (isdir(ipart) == 1)
                {
                    if(cur->isfile==0)
                    {
                    if (strcmp(cur->pointer->name, ipart) == 0)
                    {
                        fl = 1;
                        break;
                    }
                    }
                }
                else
                {
                    if(cur->isfile==1)
                    {
                    if (strcmp(cur->filename, ipart) == 0)
                    {
                        fl = 1;
                        break;
                    }
                    }
                }
                prev = cur;
                cur = cur->next;
            }
            if (fl == 0)
            {
                newnode = (struct node *)malloc(sizeof(struct node));
                prev->next = newnode;
                newnode->next = NULL;
                if (isdir(ipart))
                {
                    // newnode->pointer = (struct TrieNode *)malloc(sizeof(struct TrieNode));
                    // strcpy(newnode->pointer->name, ipart);
                    // newnode->pointer->pointtohead = NULL;
                    createTrieNode(ipart, &newnode->pointer);
                    newnode->isfile = 0;
                }
                else
                {
                    newnode->pointer = NULL;
                    newnode->isfile = 1;
                    strcpy(newnode->filename, ipart);
                }
            }
            else
            {
                newnode = cur;
            }
        }
        node = newnode->pointer;
    }
    return;
}

int TrieNodeExists(struct TrieNode *start, char *string)
{
    if (isdir(string) == 1)
    {
        struct node *first = start->pointtohead;
        int flag = 0;
        while (first != NULL)
        {
            if (first->isfile == 0)
            {
                if (strcmp(first->pointer->name, string) == 0)
                {
                    // printf("%s\n", first->pointer->name);
                    flag = 1;
                    break;
                }
            }
            first = first->next;
        }
        return flag;
    }
    else
    {
        struct node *first = start->pointtohead;
        int flag = 0;
        while (first != NULL)
        {
            if (first->isfile == 1)
            {
                if (strcmp(first->filename, string) == 0)
                {
                    // printf("%s\n", first->filename);
                    flag = 1;
                    break;
                }
            }
            first = first->next;
        }
        // printf("i -> %d\n",flag);
        return flag;
    }
}
struct TrieNode *GetNextTrieNode(struct TrieNode *start, char *string)
{
    struct node *first = start->pointtohead;
    struct node *point;
    while (first != NULL)
    {
        if(first->isfile==0)
        {
        if (strcmp(first->pointer->name, string) == 0)
        {
            point = first;
            break;
        }
        }
        first = first->next;

    }
    return point->pointer;
}
Packet_Client *searchss(char *path, MsgToNM **ss, int storage_count)
{
    char delimiter = '/';
    char input[4096];
    strcpy(input, path);
    int num_substrings = 0;
    char *temp = input;
    while (*temp)
    {
        if (*temp == delimiter)
        {
            num_substrings++;
        }
        temp++;
    }
    num_substrings++;
    char **substrings = (char **)malloc((num_substrings + 1) * sizeof(char *));
    for (int i = 0; i < num_substrings + 1; i++)
    {
        substrings[i] = NULL;
    }
    char *token = strtok(input, &delimiter);
    int index2 = 0;
    while (token)
    {
        substrings[index2] = (char *)malloc(strlen(token) + 1);
        strcpy(substrings[index2], token);
        token = strtok(NULL, &delimiter);
        index2++;
    }
    // printf("%d\n",num_substrings);

    Packet_Client *pack = (Packet_Client *)malloc(sizeof(Packet_Client));
    pack->port = -1;
    strcpy(pack->ip, "\0");
    for (int i = 0; i < storage_count; i++)
    {
        struct TrieNode *start = ss[i]->root;
        int flag = 0;
        int index = 0;
        while (substrings[index] != NULL)
        {
            if (TrieNodeExists(start, substrings[index]))
            {
                printf("%s\n", start->name); // SS1
                if (isdir(substrings[index]) == 1)
                {
                    start = GetNextTrieNode(start, substrings[index]); // A
                    printf("%s\n", start->name);
                    index++;
                    if (substrings[index] == NULL)
                    {
                        printf("ju\n");
                        flag = 1;
                        break;
                    }
                }
                else if (isdir(substrings[index]) == 0)
                {
                    flag = 1;
                    break;
                }
            }
            else
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            strcpy(pack->ip, ss[i]->ip);
            pack->port = ss[i]->port;
            break;
        }
    }
    return pack;
}
// Packet_Client *searchssinindex(char *path, MsgToNM *ss, int storage_count, int index)
// {
//     // char delimiter = '/';
//     // char input[4096];
//     // strcpy(input, path);
//     // int num_substrings = 0;
//     // char *temp = input;
//     // while (*temp)
//     // {
//     //     if (*temp == delimiter)
//     //     {
//     //         num_substrings++;
//     //     }
//     //     temp++;
//     // }
//     // num_substrings++;
//     // char **substrings = (char **)malloc(num_substrings * sizeof(char *));
//     // for (int i = 0; i < num_substrings; i++)
//     // {
//     //     substrings[i] = NULL;
//     // }
//     // char *token = strtok(input, &delimiter);
//     // int index1 = 0;
//     // while (token)
//     // {
//     //     substrings[index1] = (char *)malloc(strlen(token) + 1);
//     //     strcpy(substrings[index1], token);
//     //     token = strtok(NULL, &delimiter);
//     //     index1++;
//     // }
//     // Packet_Client *pack = (Packet_Client *)malloc(sizeof(Packet_Client));
//     // pack = NULL;
//     // char ipcopy[16];
//     // int portcopy;

//     // struct TrieNode *start = ss[index].root;
//     // int flag = 0;
//     // int index2= 0;
//     // while (substrings[index2] != NULL)
//     // {
//     //     if (TrieNodeExists(start, substrings[index2]))
//     //     {
//     //         if (isdir(substrings[index2]) == 1)
//     //         {
//     //             start = GetNextTrieNode(start, substrings[index2]);
//     //         }
//     //         index2++;
//     //         if (substrings[index2] == NULL)
//     //         {
//     //             flag = 1;
//     //             break;
//     //         }
//     //     }
//     //     else
//     //     {
//     //         break;
//     //     }
//     // }
//     // if (flag)
//     // {
//     //     strcpy(pack->ip, ss[index].ip);
//     //     pack->port = ss[index].client_port;
//     // }
//     // return pack;
//     char delimiter = '/';
//     char input[4096];
//     strcpy(input, path);
//     int num_substrings = 0;
//     char *temp = input;
//     while (*temp)
//     {
//         if (*temp == delimiter)
//         {
//             num_substrings++;
//         }
//         temp++;
//     }
//     num_substrings++;
//     char **substrings = (char **)malloc(num_substrings * sizeof(char *));
//     for (int i = 0; i < num_substrings; i++)
//     {
//         substrings[i] = NULL;
//     }
//     char *token = strtok(input, &delimiter);
//     int index3 = 0;
//     while (token)
//     {
//         substrings[index3] = (char *)malloc(strlen(token) + 1);
//         strcpy(substrings[index3], token);
//         token = strtok(NULL, &delimiter);
//         index3++;
//     }

//     Packet_Client *pack = (Packet_Client *)malloc(sizeof(Packet_Client));
//     pack->port = -1;
//     strcpy(pack->ip, "\0");
//     struct TrieNode *start = ss[index].root;
//     int flag = 0;
//     int index1 = 0;
//     while (substrings[index1] != NULL)
//     {
//         if (TrieNodeExists(start, substrings[index1]))
//         {
//             if (isdir(substrings[index1]) == 1)
//             {
//                 start = GetNextTrieNode(start, substrings[index1]);
//                 // printf("%s\n", start->name);
//                 index1++;
//                 if (substrings[index1] == NULL)
//                 {
//                     flag = 1;
//                     break;
//                 }
//             }
//             else if (isdir(substrings[index1]) == 0)
//             {
//                 flag = 1;
//                 break;
//             }
//         }
//         else
//         {
//             break;
//         }
//     }
//     if (flag)
//     {
//         strcpy(pack->ip, ss[index].ip);
//         pack->port = ss[index].client_port;
//     }
//     return pack;
// }

int deletePathRecursive(struct TrieNode *node, char **substrings, int num_substrings, struct TrieNode *parentnode, char *path, MsgToNM **ss, int storage_count, int index)
{
    // if (searchssinindex(path, ss, storage_count, index) == NULL)
    // {
    //     return 0; // not present
    // }
    int flag=0;
    // else
    // {
    TrieNode *prev = node;
    TrieNode *cur = node;
    int index2 = 0;
    // while (substrings[index2] != NULL)
    // {
    //     if (TrieNodeExists(cur, substrings[index2]))
    //     {
    //         if (isdir(substrings[index2]) == 1)
    //         {
    //             prev = cur;
    //             cur = GetNextTrieNode(cur, substrings[index2]);
    //         }
    //         index2++;
    //         if (substrings[index2] == NULL)
    //         {
    //             break;
    //         }
    //     }
    //     else
    //     {
    //         break;
    //     }
    // }
    while (substrings[index2] != NULL)
    {
        if (TrieNodeExists(cur, substrings[index2]))
        {
            if (isdir(substrings[index2]) == 1)
            {
                prev = cur;
                cur = GetNextTrieNode(cur, substrings[index2]);
                // printf("%s\n", cur->name);
                index2++;
                if (substrings[index2] == NULL)
                {
                    flag = 1;
                    break;
                }
            }
            else if (isdir(substrings[index2]) == 0)
            {
                flag = 1;
                break;
            }
        }
        else
        {
            flag = 0;
            break;
        }
    }
    if (flag == 0)
    {
        return 0;
    }
    printf("prev- %s\n", prev->name);
    printf("cur- %s\n", cur->name);
    // ok
    if (isdir(substrings[num_substrings - 1]) == 1) // -> /test1/test2  prev to test1 and cur to test2
    {
        struct node *head = prev->pointtohead;
        struct node *copy = prev->pointtohead;
        struct node *pichla = prev->pointtohead;
        int n = 0;
        struct node *count = prev->pointtohead;
        while (count != NULL)
        {
            n++;
            count = count->next;
        }
        while (head != NULL)
        {
            if(head->isfile==0)
            {
            if (strcmp(head->pointer->name, substrings[num_substrings - 1]) == 0)
            {
                if (head == copy)
                {
                    head = head->next;
                    free(pichla);
                    prev->pointtohead = head;
                    if (n == 1)
                    {
                        prev->pointtohead = NULL;
                    }
                    break;
                }
                pichla->next = head->next;
                free(head);
                head = NULL;
                break;
            }
            }
            pichla = head;
            head = head->next;
        }
        struct node *start = cur->pointtohead;
        struct node *freee = cur->pointtohead;
        while (start != NULL)
        {
            start = start->next;
            free(freee);
            freee = start;
        }
        free(cur);
        cur = NULL;
    }
    else //  -> /test1/test2/test.txt  prev to test1 and cur to test2
    {
        // struct node *head = prev->pointtohead;
        // struct node *pichla = prev->pointtohead;
        // while (head != NULL)
        // {
        //     if (strcmp(head->pointer->name, substrings[num_substrings - 1]) == 0)
        //     {
        //         pichla->next = head->next;
        //         free(head);
        //         head = NULL;
        //         break;
        //     }
        //     pichla = head;
        //     head = head->next;
        // }
        // in cur which points to test2 we have to delete only test.txt node
        struct node *front = cur->pointtohead;
        struct node *frontprev = cur->pointtohead;
        struct node *copy2 = cur->pointtohead;
        int n = 0;
        struct node *count = cur->pointtohead;
        while (count != NULL)
        {
            n++;
            count = count->next;
        }
        // printf("%d\n",n);
        while (front != NULL)
        {
            if (front->isfile == 1)
            {
                if (strcmp(front->filename, substrings[num_substrings - 1]) == 0)
                {
                    if (front == copy2)
                    {
                        front = front->next;
                        free(frontprev);
                        if (n == 1)
                        {
                            cur->pointtohead = NULL;
                        }
                        cur->pointtohead=front;
                        break;
                    }
                    frontprev->next = front->next;
                    free(front);
                    front = NULL;
                    break;
                }
            }
            frontprev = front;
            front = front->next;
        }
    }
    return 1;
}
void deletePath(struct TrieNode *root, char *path, MsgToNM **ss, int storage_count, int index)
{
    char delimiter = '/';
    char input[4096];
    strcpy(input, path);
    int num_substrings = 0;
    char *temp = input;
    while (*temp)
    {
        if (*temp == delimiter)
        {
            num_substrings++;
        }
        temp++;
    }
    num_substrings++;
    char **substrings = (char **)malloc((num_substrings + 1) * sizeof(char *));
    for (int i = 0; i < num_substrings + 1; i++)
    {
        substrings[i] = NULL;
    }
    char *token = strtok(input, &delimiter);
    int index1 = 0;
    while (token)
    {
        substrings[index1] = (char *)malloc(strlen(token) + 1);
        strcpy(substrings[index1], token);
        token = strtok(NULL, &delimiter);
        index1++;
    }
    // printf("%d\n",num_substrings);
    // for(int i=0;i<num_substrings;i++)
    // {
    //     printf("%s\n",substrings[i]);
    // }
    int result = deletePathRecursive(root, substrings, num_substrings, NULL, path, ss, storage_count, index);
    // use value of result for error checking   0->not present
    for (int i = 0; i < num_substrings; i++)
    {
        free(substrings[i]);
    }
    free(substrings);
    return;
}
// }
// int main()
// {
//     // struct TrieNode *root = (struct TrieNode *)malloc(sizeof(struct TrieNode));
//     // strcpy(root->name, "ss1");
//     // root->pointtohead = NULL;
//     ss[0].client_port = 8555;
//     strcpy(ss[0].ip, "125.0.0.1");
//     ss[0].size = 80;
//     strcpy(ss[0].servername, "SS1");
//     struct TrieNode *root1;
//     createTrieNode("SS1", &root1);
//     insertPath(root1, "A/B/file.txt");
//     insertPath(root1, "A/file2.txt");
//     insertPath(root1, "A/B/file2.txt");
//     insertPath(root1, "B");
//     insertPath(root1, "M");
//     insertPath(root1, "file4.c");
//     ss[0]-root = root1;

//     // SS1
//     // A B M file4.c
//     // B file2.txt
//     // file.txt file2.txt

//     deletePath(root1, "M", ss, 2, 1);
//     printf("%s\n", root1->name);
//     struct node *as = root1->pointtohead;
//     while (as != NULL)
//     {
//         if (as->isfile == 1)
//         {
//             printf("%s ", as->filename);
//         }
//         else
//         {
//             printf("%s ", as->pointer->name);
//         }
//         as = as->next;
//     }
//     printf("\n");
//     struct node *A = root1->pointtohead->pointer->pointtohead;
//     // printf("%s\n",A->filename);
//     // if(A->next==NULL)
//     // {
//     //     printf("65\n");
//     // }
//     // return 0;
//     // printf("%s\n",root1->pointtohead->next->next->filename);
//     // struct node *A =root1->pointtohead->pointer->pointtohead;
//     // if(A==NULL)
//     // {
//     //     printf("65\n");
//     // }
//     // return 0;
//     while (A != NULL)
//     {
//         if (A->isfile == 1)
//         {
//             printf("%s ", A->filename);
//         }
//         else
//         {
//             printf("%s ", A->pointer->name);
//         }
//         A = A->next;
//     }
//     printf("\n");
//     struct node *B = root1->pointtohead->pointer->pointtohead->pointer->pointtohead;
//     while (B != NULL)
//     {
//         if (B->isfile == 1)
//         {
//             printf("%s ", B->filename);
//         }
//         else
//         {
//             printf("%s ", B->pointer->name);
//         }
//         B = B->next;
//     }
//     printf("\n");

//     // ss[1].client_port = 8556;
//     // strcpy(ss[1].ip, "125.0.0.2");
//     // ss[1].size = 100;
//     // strcpy(ss[1].servername, "SS2");
//     // struct TrieNode *root2;
//     // createTrieNode("SS2", &root2);
//     // insertPath(root2, "C/D/file3.txt");
//     // insertPath(root2, "C/file4.txt");
//     // insertPath(root2, "E");
//     // ss[1].root = root2;
//     storage_count = 1;

//     // struct node* a=root1->pointtohead->pointer->pointtohead->pointer->pointtohead;
//     // if(a==NULL)
//     // {
//     //     printf("jkl\n");
//     // }

//     Packet_Client *packet;
//     packet = searchss("M", ss, 1);
//     if ((packet->port == -1) && (strcmp(packet->ip, "\0") == 0))
//     {
//         printf("Not found\n");
//     }
//     else
//     {
//         printf("%s %d\n", packet->ip, packet->port);
//     }

//     // struct node *a = root->pointtohead->pointer->pointtohead->next;
//     // printf("%s\n", a->filename);
//     // struct node* b=root->pointtohead->next;
//     // printf("%s\n",b->pointer->name);
//     // if (root != NULL && root->pointtohead != NULL && root->pointtohead->pointer != NULL)
//     // {
//     //     printf("%s %s %s\n", root->pointtohead->pointer->name, root->pointtohead->pointer->pointtohead->pointer->name, root->pointtohead->pointer->pointtohead->pointer->pointtohead->filename);
//     // }
//     // else
//     // {
//     //     if (root->pointtohead == NULL)
//     //     {
//     //         printf("1\n");
//     //     }
//     //     else if (root->pointtohead->pointer == NULL)
//     //     {
//     //         printf("2\n");
//     //     }
//     //     else
//     //     {
//     //         printf("Error: Trie is empty or path not inserted correctly.\n");
//     //     }
//     // }
// }