#include "header.h"
#include "Trie.h"
typedef struct client_only
{
    int fd;
    int client_id;
} client_only;

int cache_count = 0;
int storage_count = 0;
int flag = 1;
int status_count = 0;
int ssd_port = 0;
int file_port = 0;
int copy_dir_travel_index = 0;
int copy_dir_total = 0;
char copy_dir_path[NO_ACCESSIBLE_PATH][PATH_LENGTH];
char copy_dir_path_org[NO_ACCESSIBLE_PATH][PATH_LENGTH];

int from_copy_dir;
int index_copy_dir;
// int client_only_count = 0;
int s_sock;
char path_to_cpy[PATH_LENGTH] = {'\0'};
char path_from_cpy[PATH_LENGTH] = {'\0'};
int client_id_copy = -1;
int client_id_copy_dir;
char first_copy_path[PATH_LENGTH] = {'\0'};
client_only client[MAX_CLIENT];
int client_id_struct_index = 0;
MsgToNM *ss[MAX_STORAGE];

void PrintSSrecvMsg(MsgToSS *Msg)
{
    printf("From %c:\n", Msg->From);
    printf("Command %s:\n", Msg->Command);
    printf("Path %s:\n", Msg->path);
    printf("To_write %s:\n", Msg->To_write);
    printf("Done_job %d:\n", Msg->done_Job);
}

void ctrlC_Handler(int signal)
{
    printf("Ctrl+C pressed. Closing Sockets... Exiting...\n");
    close(s_sock);
    exit(0);
}
// void handle_rwi(MsgToNM *buffer, char *string, int socket, node_LRU head1)
// {
//     store str;
//     str = (store)malloc(sizeof(struct store));
//     char path[100];
//     strcpy(path, buffer->path);
//     str = search(buffer->command, buffer->path, NULL, &head1, str);
//     Packet_Client *value;
//     if (str == NULL)
//     {
//         // call the trie function

//         value = searchss(buffer->path, ss, storage_count);

//         if (value == NULL || value->port == -1)
//         {

//             MsgToCLIENT *send_packet;
//             send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
//             send_packet->Ack = 0;
//             if (strcmp("READ", string) == 0)
//                 send_packet->error_code = ERROR_READ;
//             else if (strcmp("WRITE", string) == 0)
//                 send_packet->error_code = ERROR_WRITE;
//             else
//                 send_packet->error_code = ERROR_COPY;

//             if (send(socket, send_packet, sizeof(MsgToCLIENT), 0) < 0)
//             {
//                 perror("Couldnot send the data\n");
//                 exit(EXIT_FAILURE);
//             }
//             return;
//         }

//         //    if (strcmp(buffer->command, "COPY") != 0 && strcmp(buffer->command, "COPY_D") != 0)
//         addtolistof20(&head1, buffer->command, 0, buffer->path, NULL, "127.0.0.1", NULL, value->port, -1);
//         // change the parameters
//     }
//     else
//         addtolistof20(&head1, buffer->command, 1, buffer->path, NULL, "127.0.0.1", NULL, str->port1, -1);
//     // call the search function and get the ip and port of the storage server. lets say in ip and port variable
//     MsgToCLIENT *send_packet;
//     send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
//     // send_packet->ip = ip;
//     // send_packet->port = port;
//     strcpy(send_packet->ip, "127.0.0.1");
//     // send_packet->port_SS = str->port1;
//     send_packet->port_SS = value->port;
//     send_packet->error_code = 0;
//     send_packet->Ack = 1;
//     // search for the corressponding port1
//     int a;
//     for (a = 0; a < storage_count; a++)
//     {
//         if (send_packet->port_SS == ss[a]->port)
//         {
//             break;
//         }
//     }
//     if(a<2)
//     {send_packet->flag=0;}
//     else 
//     send_packet->flag=1;
  
//     printf("%s\n", buffer->command);
//     MsgToSS *msg;
//     msg = (MsgToSS *)malloc(sizeof(MsgToSS));
//     strcpy(msg->Command, buffer->command);
//     msg->FromID = buffer->FromID;
//     msg->From = 'N';

//     if (send(ss[a]->number_access, msg, sizeof(MsgToSS), 0) < 0)
//     {
//         perror("Couldnot send the data\n");
//         exit(EXIT_FAILURE);
//     }
//     if(a>=2)
//     {
//          printf("Hello from a %d",a);
//          printf("Flag %d\n",send_packet->flag);
//         send_packet->num = a + 1;
//         send_packet->flag = 1;
//         send_packet->port2 = ss[a - 2]->port;
//         send_packet->port3 = ss[a - 1]->port;
//           if (send(socket, send_packet, sizeof(MsgToCLIENT), 0) < 0)
//     {
//         perror("Couldnot send the data\n");
//         exit(EXIT_FAILURE);
//     }
//     if (send(ss[a-1]->number_access, msg, sizeof(MsgToSS), 0) < 0)
//     {
//         perror("Couldnot send the data\n");
//         exit(EXIT_FAILURE);
//     }
//     send_packet->flag=1;
//     if (send(ss[a-2]->number_access, msg, sizeof(MsgToSS), 0) < 0)
//     {
//         perror("Couldnot send the data\n");
//         exit(EXIT_FAILURE);
//     }
//     return;
//     }
//     if (send(socket, send_packet, sizeof(MsgToCLIENT), 0) < 0)
//     {
//         perror("Couldnot send the data\n");
//         exit(EXIT_FAILURE);
//     }
// }
void handle_rwi(MsgToNM *buffer, char *string, int socket, node_LRU head1)
{
    store str;
    str = (store)malloc(sizeof(struct store));
    char path[100];
    strcpy(path, buffer->path);
    str = search(buffer->command, buffer->path, NULL, &head1, str);
    Packet_Client *value;
    if (str == NULL)
    {
        // call the trie function

        value = searchss(buffer->path, ss, storage_count);

        if (value == NULL || value->port == -1)
        {

            MsgToCLIENT *send_packet;
            send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
            send_packet->Ack = 0;
            if (strcmp("READ", string) == 0)
                send_packet->error_code = ERROR_READ;
            else if (strcmp("WRITE", string) == 0)
                send_packet->error_code = ERROR_WRITE;
            else
                send_packet->error_code = ERROR_COPY;

            if (send(socket, send_packet, sizeof(MsgToCLIENT), 0) < 0)
            {
                perror("Couldnot send the data\n");
                exit(EXIT_FAILURE);
            }
            return;
        }
        
        //    if (strcmp(buffer->command, "COPY") != 0 && strcmp(buffer->command, "COPY_D") != 0)
        addtolistof20(&head1, buffer->command, 0, buffer->path, NULL, "127.0.0.1", NULL, value->port, -1);
        // change the parameters
        
    }
    else
        addtolistof20(&head1, buffer->command, 1, buffer->path, NULL, "127.0.0.1", NULL, str->port1, -1);
    // call the search function and get the ip and port of the storage server. lets say in ip and port variable
    MsgToCLIENT *send_packet;
    send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
    // send_packet->ip = ip;
    // send_packet->port = port;
    strcpy(send_packet->ip, "127.0.0.1");
    // send_packet->port_SS = str->port1;
    send_packet->port_SS = value->port;
    send_packet->error_code = 0;
    send_packet->Ack = 1;
    // search for the corressponding port1
    int a;
    for (a = 0; a < storage_count; a++)
    {
        if (send_packet->port_SS == ss[a]->port)
        {
            break;
        }
    }
    printf("%s\n", buffer->command);

    MsgToSS *msg;
    msg = (MsgToSS *)malloc(sizeof(MsgToSS));
    strcpy(msg->Command, buffer->command);
    msg->FromID = buffer->FromID;
    msg->From = 'N';

    if (send(ss[a]->number_access, msg, sizeof(MsgToSS), 0) < 0)
    {
        perror("Couldnot send the data\n");
        exit(EXIT_FAILURE);
    }

    if (send(socket, send_packet, sizeof(MsgToCLIENT), 0) < 0)
    {
        perror("Couldnot send the data\n");
        exit(EXIT_FAILURE);
    }
}
void handle_create(MsgToNM *buffer, char *string, int socket, node_LRU head1)
{
    MsgToSS *message;
    int single = 0;

    message = (MsgToSS *)malloc(sizeof(MsgToSS));
    char path[100];
    strcpy(path, buffer->path);
    int x;
    int h;
    int min = 9999999;
    for (x = 0; x < storage_count; x++)
    {
        if (ss[x]->size < min)
        {
            min = ss[x]->size;
            h = x;
        }
    }
    Packet_Client *value;
    const char *lastSlash = strrchr(buffer->path, '/');

    // Allocate memory for the substring (plus one for the null terminator)

    if (lastSlash != NULL)
    {
        // Calculate the length of the substring
        size_t substringLength = lastSlash - buffer->path; // Corrected from 'path' to 'buffer->path'
        char substring[substringLength + 1];

        strncpy(substring, buffer->path, substringLength);

        substring[substringLength] = '\0'; // Null-terminate the string
        value = searchss(substring, ss, storage_count);

        // Print or use the extracted substring
        // printf("Substring: %s\n", substring);
    }

    else
    {
        // '/' not found, handle accordingly
        single = 1;
    }

    if (single == 1)
    {

        ss[h]->size += 50;

        message->From = 'N';
        if (strcmp("CREATE", string) == 0)
            strcpy(message->Command, "CREATE");
        else
            strcpy(message->Command, "CREATE_D");

        // printf("Passed till here\n");
        message->done_Job = true;
        message->FromID = buffer->FromID;
        strcpy(message->path, path);
        if (send(ss[h]->number_access, message, sizeof(MsgToSS), 0) == -1)
        {
            perror("Failed to send data to NM server");
        }

        insertPath(ss[h]->root, buffer->path);
        if (h >= 2)
        {
            int newStringLength = snprintf(NULL, 0, "$%d/%s", h + 1, buffer->path);

            // Allocate memory for the new string
            char *newString = (char *)malloc(newStringLength + 1);

            // Use snprintf to create the new string
            snprintf(newString, newStringLength + 1, "$%d/%s", h + 1, buffer->path);
            insertPath(ss[h - 1]->root, newString);
            insertPath(ss[h - 2]->root, newString);
            strcpy(message->path, newString);
            if (send(ss[h - 1]->number_access, message, sizeof(MsgToSS), 0) == -1)
            {
                perror("Failed to send data to NM server");
            }
            if (send(ss[h - 2]->number_access, message, sizeof(MsgToSS), 0) == -1)
            {
                perror("Failed to send data to NM server");
            }
        }
    }
    else if (value->port != -1)
    {

        int a;
        for (a = 0; a < storage_count; a++)
        {
            if (ss[a]->port == value->port)
            {
                break;
            }
        }

        ss[a]->size += 50;

        message->From = 'N';

        if (strcmp("CREATE", string) == 0)
            strcpy(message->Command, "CREATE");
        else
            strcpy(message->Command, "CREATE_D");

        // printf("Passed till here\n");
        message->done_Job = true;
        message->FromID = buffer->FromID;
        strcpy(message->path, path);
        if (send(ss[a]->number_access, message, sizeof(MsgToSS), 0) == -1)
        {
            perror("Failed to send data to NM server");
        }

        insertPath(ss[a]->root, buffer->path);
        if (a >= 2)
        {
            int newStringLength = snprintf(NULL, 0, "$%d/%s", a + 1, buffer->path);

            // Allocate memory for the new string
            char *newString = (char *)malloc(newStringLength + 1);

            // Use snprintf to create the new string
            snprintf(newString, newStringLength + 1, "$%d/%s", a + 1, buffer->path);
            insertPath(ss[a - 1]->root, newString);
            insertPath(ss[a - 2]->root, newString);
            strcpy(message->path, newString);
            if (send(ss[a - 1]->number_access, message, sizeof(MsgToSS), 0) == -1)
            {
                perror("Failed to send data to NM server");
            }
            if (send(ss[a - 2]->number_access, message, sizeof(MsgToSS), 0) == -1)
            {
                perror("Failed to send data to NM server");
            }
        }
    }

    PrintSSrecvMsg(message);
}
void handle_delete(MsgToNM *buffer, char *string, int socket, node_LRU head1)
{
    store str;
    str = (store)malloc(sizeof(struct store));
    char path[100];
    int qw = 0;
    Packet_Client *value;
    strcpy(path, buffer->path);
    MsgToSS *NM;
    NM = (MsgToSS *)malloc(sizeof(MsgToSS));
    strcpy(NM->Command, "DELETE");
    strcpy(NM->path, path);

    // call the search function and get the ip and port of the storage server. Lets say in ip and port variable or socket
    str = search(buffer->command, buffer->path, NULL, &head1, str);
    if (str == NULL)
    {
        // call the trie function
        value = searchss(buffer->path, ss, storage_count);
        if (value->port == -1)
        {
            MsgToCLIENT *send_packet;
            send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
            send_packet->Ack = 0;
            send_packet->error_code = ERROR_DELETE;
            if (send(socket, send_packet, sizeof(MsgToCLIENT), 0) < 0)
            {
                perror("Couldnot send the data\n");
                exit(EXIT_FAILURE);
            }
            return;
        }
        qw = 1;
        addtolistof20(&head1, buffer->command, 0, buffer->path, NULL, "127.0.0.1", NULL, value->port, -1); // change the parameters
    }
    else
        addtolistof20(&head1, buffer->command, 1, buffer->path, NULL, "127.0.0.1", NULL, str->port1, -1);
    int a;
    int act_port;
    if (qw == 0)
        act_port = str->port1;
    else
        act_port = value->port;
    for (a = 0; a < storage_count; a++)
    {
        if (act_port == ss[a]->port)
        {
            break;
        }
    }
    NM->FromID = buffer->FromID;
    deletePath(ss[a]->root, buffer->path, ss, storage_count, a);
    if (send(ss[a]->number_access, NM, sizeof(MsgToSS), 0) < 0) // modify the code to replace the storage server socket number from accept command
    {
        perror("Error sending the command packet to the storage server\n");
        exit(EXIT_FAILURE);
    }
    if (a >= 2)
    {
        int newStringLength = snprintf(NULL, 0, "$%d/%s", a + 1, buffer->path);

        // Allocate memory for the new string
        char *newString = (char *)malloc(newStringLength + 1);
        // Use snprintf to create the new string
        snprintf(newString, newStringLength + 1, "$%d/%s", a + 1, buffer->path);
        deletePath(ss[a - 1]->root, buffer->path, ss, storage_count, a);
        deletePath(ss[a - 2]->root, buffer->path, ss, storage_count, a);
        MsgToSS *packet = (MsgToSS *)malloc(sizeof(sizeof(MsgToSS)));
        strcpy(packet->path, newString);
        strcpy(packet->Command, buffer->command);
        packet->FromID = buffer->FromID;
        packet->num = a + 1;
        // printf("%s\n",packet->path);
        // printf("%d\n",a);
        if (send(ss[a - 1]->number_access, packet, sizeof(MsgToSS), 0) < 0) // modify the code to replace the storage server socket number from accept command
        {
            perror("Error sending the command packet to the storage server\n");
            exit(EXIT_FAILURE);
        }
        if (send(ss[a - 2]->number_access, packet, sizeof(MsgToSS), 0) < 0) // modify the code to replace the storage server socket number from accept command
        {
            perror("Error sending the command packet to the storage server\n");
            exit(EXIT_FAILURE);
        }
    }
}
void handle_log(MsgToNM *buffer, char *string, int socket, log head)
{
    log tem;
    if (head != NULL)
        tem = head->next;

    while (tem != NULL)
    {

        printf("Command: %s  SS No: %d  IP_Used: %s  Port_Used: %d Status: %s\n ", tem->command, tem->index_in_array_of_listed_SS + 1, tem->ip1, tem->port1, tem->status);
        tem = tem->next;
    }
}

void handle_comm_client(log head, node_LRU head1, struct sockaddr_in client_addr, socklen_t client_len, MsgToNM *buffer, int sd)
{
    if (buffer->From == 'S')
    {
        if (flag == 1)
        {
            ss[storage_count] = buffer;
            buffer->number_access = sd;
            struct TrieNode *root1;
            char ssString[10]; // Adjust the size based on your needs
            // sprintf(ssString, "D%d", storage_count + 1);
            createTrieNode(ssString, &root1);
            ss[storage_count++]->root = root1;
            if (storage_count >= 3)
            {

                int newStringLength = snprintf(NULL, 0, "$%d", storage_count - 1);

                // Allocate memory for the new string
                char *newString = (char *)malloc(newStringLength + 1);

                // Use snprintf to create the new string
                snprintf(newString, newStringLength + 1, "$%d", storage_count);
                MsgToSS *message = (MsgToSS *)malloc(sizeof(MsgToSS));
                strcpy(message->path, newString);
                strcpy(message->Command, "CREATE_D");
                if (send(ss[storage_count - 3]->number_access, message, sizeof(MsgToSS), 0) == -1)
                {
                    perror("Failed to send data to NM server");
                }
                if (send(ss[storage_count - 2]->number_access, message, sizeof(MsgToSS), 0) == -1)
                {
                    perror("Failed to send data to NM server");
                }
            }
            int yu;
            for (yu = 0; yu < buffer->ack; yu++)
            {
                printf("path %s \n", buffer->Path2dArr[yu]);

                char *pathCopy = strdup(buffer->Path2dArr[yu]);
                char *token1 = strtok(pathCopy, "/");
                // char *token1 = strtok(buffer->Path2dArr[yu], "/");
                int flag23 = 0;
                int op = 1;
                int po;
                for (po = 1; po < 10; po++)
                {
                    char *pathCopy = strdup(buffer->Path2dArr[yu]);
                    char *token1 = strtok(pathCopy, "/");
                    char prevToken[100] = {'\0'};

                    for (op = 0; op < po; op++)
                    {
                        if (token1 == NULL)
                        {
                            flag23 = 1;
                            break;
                        }
                        if (op == 0)
                            strcpy(prevToken, token1);

                        else
                        {
                            strcat(prevToken, "/");
                            strcat(prevToken, token1);
                        }
                        token1 = strtok(NULL, "/");
                    }

                    if (flag23 == 1)
                    {
                        break;
                    }
                    else
                    {
                        insertPath(ss[storage_count - 1]->root, prevToken);
                    }
                }
            }
            flag = 0;
            return;
        }
        else
        {
            printf("%s\n", buffer->command);
            printf("%d\n", buffer->if_copy);
            if (strcmp(buffer->command, "COPY") == 0 || strcmp(buffer->command, "COPY_D") == 0)
            {

                if (buffer->if_copy == 1)
                {
                    if (strcmp(buffer->command, "COPY_D") == 0)
                    {
                        MsgToSS *Msg;
                        Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
                        int a;
                        int kl = 0, ct = 0;
                        while (buffer->Path2dArr[0][kl] != '\0')
                        {
                            if (buffer->Path2dArr[0][kl] == '/')
                            {
                                /* code */
                                ct++;
                            }
                            kl++;
                        }
                        int ctChar = 0;
                        int Noofslash = 0;
                        while (Noofslash != ct)
                        {
                            if (buffer->Path2dArr[0][ctChar] == '/')
                            {
                                Noofslash++;
                            }
                            ctChar++;
                        }
                        for (a = 0; a < buffer->ack; a++)
                        {

                            // Display the result
                            // for (int dv = 0; dv < M1->ack; dv++)
                            // {
                            //     // M1->Path2dArr[dv]=M1->Path2dArr[dv]+ctChar;

                            // }
                            char path_F[100];
                            strcpy(path_F, buffer->Path2dArr[a]);
                            // printf(" path to copy %s\n", buffer->Path2dArr[a]);
                            int j = 0;
                            while (j < strlen(path_F))
                            {
                                if (path_F[j] == '.')
                                    break;
                                j++;
                            }
                            if (j == strlen(path_F))
                            {
                                strcpy(Msg->Command, "CREATE_D");
                                char path_2[100] = {'\0'};
                                strcpy(path_2, path_to_cpy);
                                strcat(path_2, "/");

                                int ff = ctChar;
                                char str[200];
                                int strInd = 0;
                                while (buffer->Path2dArr[a][ff] != '\0')
                                {
                                    str[strInd] = buffer->Path2dArr[a][ff];
                                    strInd++;
                                    ff++;
                                }
                                str[strInd] = buffer->Path2dArr[a][ff];
                                char storeOrignal[100] = {'\0'};
                                strcpy(storeOrignal, buffer->Path2dArr);
                                strcpy(buffer->Path2dArr[a], str);
                                printf("bfsTravel[%d]=%s\n", a, buffer->Path2dArr[a]);
                                strcpy(path_F, buffer->Path2dArr[a]);
                                strcat(path_2, path_F);
                                strcpy(Msg->path, path_2);
                                Msg->copy_dir_flag = 1;
                                Msg->From = 'N';
                                Msg->FromID = buffer->FromID;
                                int m;
                                for (m = 0; m < storage_count; m++)
                                {
                                    if (ss[m]->port == ssd_port)
                                        break;
                                }
                                if (send(ss[m]->number_access, Msg, sizeof(MsgToSS), 0) < 0)
                                {
                                    perror("Couldnot send the data\n");
                                    exit(EXIT_FAILURE);
                                }

                                insertPath(ss[m]->root, path_2);
                            }
                            else
                            {
                                if (copy_dir_travel_index == 0)
                                {
                                    strcpy(first_copy_path, path_F);
                                }
                                strcpy(copy_dir_path_org[copy_dir_travel_index], path_F);
                                int ff = ctChar;
                                char str[200];
                                int strInd = 0;
                                while (buffer->Path2dArr[a][ff] != '\0')
                                {
                                    str[strInd] = buffer->Path2dArr[a][ff];
                                    strInd++;
                                    ff++;
                                }
                                str[strInd] = buffer->Path2dArr[a][ff];
                                // char storeOrignal[100]={'\0'};
                                // strcpy(storeOrignal,buffer->Path2dArr);
                                strcpy(buffer->Path2dArr[a], str);
                                printf("bfsTravel[%d]=%s\n", a, buffer->Path2dArr[a]);
                                strcpy(path_F, buffer->Path2dArr[a]);
                                char path_2[100] = {'\0'};
                                strcpy(path_2, path_to_cpy);
                                strcat(path_2, "/");
                                strcat(path_2, path_F);
                                strcpy(copy_dir_path[copy_dir_travel_index++], path_2);
                                printf("Copy number file %d\n", copy_dir_travel_index);
                                printf("copy path dir file: %s\n", copy_dir_path[0]);
                            }
                        }
                        if (copy_dir_travel_index >= 1)
                        {
                            from_copy_dir = buffer->FromID;
                            copy_dir_total = copy_dir_travel_index;
                            index_copy_dir = sd;
                            copy_file();
                        }
                    }
                    else
                    {
                        if (buffer->copy_dir_flag == 1)
                        {
                            MsgToSS *Msg;
                            Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
                            printf("copy directory file data came \n");

                            if (copy_dir_travel_index < copy_dir_total)
                            {
                                strcpy(Msg->Command, "COPY");
                                printf("%s\n", buffer->data);
                                Msg->CopyFlag = 'T';
                                Msg->From = 'N';
                                Msg->FromID = buffer->FromID;
                                Msg->copy_dir_flag = 1;
                                strcpy(Msg->To_write, buffer->data);
                                char path_2[100] = {'\0'};

                                strcpy(Msg->path2, copy_dir_path[copy_dir_travel_index]);
                                int m;
                                for (m = 0; m < storage_count; m++)
                                {
                                    if (ss[m]->port == ssd_port)
                                        break;
                                }
                                if (send(ss[m]->number_access, Msg, sizeof(MsgToSS), 0) < 0)
                                {
                                    perror("Couldnot send the data\n");
                                    exit(EXIT_FAILURE);
                                }
                                insertPath(ss[m]->root, Msg->path2);

                                copy_dir_travel_index++;
                                if (copy_dir_travel_index >= copy_dir_total)
                                {
                                    printf("Why not here\n");
                                    MsgToCLIENT *M1;
                                    M1 = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                                    M1->Ack = 1;
                                    M1->From = 'N';
                                    if (send(client_id_copy_dir, Msg, sizeof(MsgToCLIENT), 0) < 0)
                                    {
                                        perror("Couldnot send the data\n");
                                        exit(EXIT_FAILURE);
                                    }
                                    copy_dir_total = 0;
                                    copy_dir_travel_index = 0;
                                    client_id_copy_dir = -1;
                                    for (int ioi = 0; ioi < PATH_LENGTH; ioi++)
                                    {
                                        first_copy_path[ioi] = '\0';
                                    }
                                    for (int joj = 0; joj < NO_ACCESSIBLE_PATH; joj++)
                                    {
                                        for (int ioi = 0; ioi < PATH_LENGTH; ioi++)
                                        {
                                            copy_dir_path[joj][ioi] = '\0';
                                            copy_dir_path_org[joj][ioi] = '\0';
                                        }
                                    }

                                    return;
                                }
                            }
                        }
                        else if (buffer->rec_copy_dir == 1)
                        {
                            printf("BUFFER REC ENTERED\n");
                            if (copy_dir_travel_index == copy_dir_total)
                            {
                                return;
                            }
                            MsgToSS *Msg;
                            Msg = (MsgToSS *)malloc(sizeof(Msg));
                            strcpy(Msg->Command, "COPY");
                            strcpy(Msg->path1, copy_dir_path_org[copy_dir_travel_index]);
                            Msg->CopyFlag = 'F';
                            Msg->From = 'N';
                            Msg->copy_dir_flag = 1;

                            Msg->FromID = from_copy_dir;
                            if (send(index_copy_dir, Msg, sizeof(MsgToSS), 0) < 0)
                            {
                                perror("Couldnot send the data\n");
                                exit(EXIT_FAILURE);
                            }
                        }
                        else
                        {

                            MsgToSS *Msg;
                            Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
                            Packet_Client *value;
                            value = searchss(path_to_cpy, ss, storage_count);
                            if (value == NULL || value->port == -1)
                            {
                                MsgToCLIENT *send_packet;
                                send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                                send_packet->Ack = 0;
                                send_packet->error_code = ERROR_COPY;
                                int jk;
                                for (jk = 0; jk < client_id_struct_index; jk++)
                                {
                                    if (client[jk].client_id == client_id_copy)
                                    {
                                        break;
                                    }
                                }
                                if (send(client[jk].fd, send_packet, sizeof(MsgToCLIENT), 0) < 0)
                                {
                                    perror("Couldnot send the data\n");
                                    exit(EXIT_FAILURE);
                                }
                            }

                            strcpy(Msg->Command, buffer->command);
                            strcpy(Msg->To_write, buffer->data);
                            Msg->CopyFlag = 'T';
                            Msg->FromID = client_id_copy;
                            Msg->From = 'N';
                            int a;
                            for (a = 0; a < storage_count; a++)
                            {
                                if (ss[a]->port == value->port)
                                    break;
                            }

                            int df = strlen(path_from_cpy) - 1;
                            while (df >= 0)
                            {
                                if (path_from_cpy[df] == '/')
                                    break;
                                df--;
                            }
                            char mod_path[PATH_LENGTH];

                            if (df != -1)
                            {

                                path_to_cpy[strlen(path_to_cpy)] = '/';
                                strcpy(mod_path, path_to_cpy);
                                strcat(mod_path, path_from_cpy + df + 1);
                            }
                            else
                            {
                                path_to_cpy[strlen(path_to_cpy)] = '/';
                                strcpy(mod_path, path_to_cpy);
                                strcat(mod_path, path_from_cpy);
                            }

                            strcpy(Msg->path2, mod_path);
                            memset(path_from_cpy, '\0', sizeof(path_from_cpy));
                            memset(path_to_cpy, '\0', sizeof(path_to_cpy));
                            if (send(ss[a]->number_access, Msg, sizeof(MsgToSS), 0) < 0)
                            {
                                perror("Couldnot send the data\n");
                                exit(EXIT_FAILURE);
                            }

                            insertPath(ss[a]->root, mod_path);
                        }
                    }
                }
                else
                {
                    printf("Received the acknowledgement\n");
                    MsgToCLIENT *message;
                    message = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                    message->Ack = buffer->ack;
                    if (buffer->ack == 0)
                    {
                        message->error_code = buffer->error_code;
                    }

                    int g;
                    for (g = 0; g < client_id_struct_index; g++)
                    {
                        if (client[g].client_id == buffer->FromID)
                        {
                            //      printf("Hello\n");

                            if (strcmp(buffer->command, "COPY") == 0 || strcmp(buffer->command, "COPY_D") == 0)
                            {
                                int a;
                                for (a = 0; a < storage_count; a++)
                                {
                                    if (ss[a]->number_access == sd)
                                        break;
                                }
                                if (buffer->ack == 1)
                                    addtolog(head, a, buffer->command, buffer->path, NULL, "127.0.0.1", NULL, ss[a]->port, -1, "Success", buffer->ack);
                                else
                                    addtolog(head, a, buffer->command, buffer->path, NULL, "127.0.0.1", NULL, ss[a]->port, -1, "Failed", buffer->ack);
                            }
                            if (send(client[g].fd, message, sizeof(MsgToCLIENT), 0) == -1)
                            {
                                perror("The message could not be sent\n");
                            }

                            break;
                        }
                    }
                }
            }
            else
            {
                printf("Received the acknowledgement\n");
                MsgToCLIENT *message;
                message = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                message->Ack = buffer->ack;
                if (buffer->ack == 0)
                {
                    message->error_code = buffer->error_code;
                }

                int g;
                for (g = 0; g < client_id_struct_index; g++)
                {
                    if (client[g].client_id == buffer->FromID)
                    {
                        //      printf("Hello\n");
                        if (strcmp(buffer->command, "COPY") != 0 && strcmp(buffer->command, "COPY_D") != 0)
                        {
                            int a;
                            for (a = 0; a < storage_count; a++)
                            {
                                if (ss[a]->number_access == sd)
                                    break;
                            }
                            if (buffer->ack == 1)
                                addtolog(head, a, buffer->command, buffer->path, NULL, "127.0.0.1", NULL, ss[a]->port, -1, "Success", buffer->ack);
                            else
                                addtolog(head, a, buffer->command, buffer->path, NULL, "127.0.0.1", NULL, ss[a]->port, -1, "Failed", buffer->ack);
                        }
                        // addtolog(head, 0, buffer->command, buffer->path, NULL, buffer->ip, NULL, buffer->port, -1, buffer->ack, buffer->ack);
                        if (send(client[g].fd, message, sizeof(MsgToCLIENT), 0) == -1)
                        {
                            perror("The message could not be sent\n");
                        }
                        break;
                    }
                }
            }
            // }
        }
        // pack the struct with the info of the storage servers and send.
    }
    else if (buffer->From == 'C')
    {
        flag = 0;
        store str;
        str = (store)malloc(sizeof(struct store));
        client[client_id_struct_index].fd = sd;
        client[client_id_struct_index++].client_id = buffer->FromID;
        printf(" Received command: %s\n ", buffer->command);
        printf(" Received path: %s\n ", buffer->path);

        char string[100];
        strcpy(string, buffer->command);
        if (strcmp("LOG", string) == 0)
        {
            handle_log(buffer, string, sd, head); //
            return;
        }
        if (strcmp("READ", string) == 0 || strcmp("WRITE", string) == 0 || strcmp("INFO", string) == 0)
        {
            handle_rwi(buffer, string, sd, head1);
        }
        else if (strcmp("CREATE", string) == 0 || strcmp("CREATE_D", string) == 0)
        {
            handle_create(buffer, string, sd, head1);
        }
        else if (strcmp("DELETE", string) == 0)
        {
            handle_delete(buffer, string, sd, head1);
        }
        else if (strcmp("COPY", string) == 0 || strcmp("COPY_D", string) == 0)
        {
            MsgToSS *Msg;
            Packet_Client *value1;
            Packet_Client *value2;
            int qw = 0;
            Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
            strcpy(Msg->Command, string);
            Msg->From = 'N';
            Msg->FromID = buffer->FromID;
            strcpy(Msg->path1, buffer->path1);
            // strcpy(Msg->path2,buffer->path2);
            str = search(buffer->command, buffer->path1, buffer->path2, &head1, str);
            if (str == NULL)
            {
                // call the trie function
                value1 = searchss(buffer->path1, ss, storage_count);
                if (value1->port == -1)
                {
                    MsgToCLIENT *send_packet;
                    send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                    send_packet->Ack = 0;
                    send_packet->error_code = ERROR_COPY;
                    if (send(sd, send_packet, sizeof(MsgToCLIENT), 0) < 0)
                    {
                        perror("Couldnot send the data\n");
                        exit(EXIT_FAILURE);
                    }
                    return;
                }
                value2 = searchss(buffer->path2, ss, storage_count);
                if (value2->port == -1)
                {
                    MsgToCLIENT *send_packet;
                    send_packet = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                    send_packet->Ack = 0;
                    send_packet->error_code = ERROR_COPY;
                    if (send(sd, send_packet, sizeof(MsgToCLIENT), 0) < 0)
                    {
                        perror("Couldnot send the data\n");
                        exit(EXIT_FAILURE);
                    }
                    return;
                }
                qw = 1;
                addtolistof20(&head1, buffer->command, 0, buffer->path1, buffer->path2, "127.0.0.1", "127.0.0.1", value1->port, value2->port); // change the parameters
            }
            else
                addtolistof20(&head1, buffer->command, 0, buffer->path1, buffer->path2, "127.0.0.1", "127.0.0.1", str->port1, str->port2); // change the parameters

            int a;
            int act_port1, act_port2;
            if (qw == 0)
            {
                act_port1 = str->port1;
                act_port2 = str->port2;
            }
            else
            {
                act_port1 = value1->port;
                act_port2 = value2->port;
            }
            if (act_port1 == act_port2)
            {
                for (a = 0; a < storage_count; a++)
                {
                    if (act_port1 == ss[a]->port)
                    {
                        break;
                    }
                }
                Msg->CopyFlag = 'B';
                int df = strlen(buffer->path1) - 1;
                while (df >= 0)
                {
                    if (buffer->path1[df] == '/')
                        break;
                    df--;
                }

                char mod_path[PATH_LENGTH];

                if (df != -1)
                {

                    buffer->path2[strlen(buffer->path2)] = '/';
                    strcpy(mod_path, buffer->path2);
                    // printf("PPP\n");

                    strcat(mod_path, buffer->path1 + df + 1);
                    // printf("PPPPPPP\n");
                }
                else
                {

                    buffer->path2[strlen(buffer->path2)] = '/';
                    strcpy(mod_path, buffer->path2);
                    strcat(mod_path, buffer->path1);
                }
                // printf("QQQQ\n");

                insertPath(ss[a]->root, mod_path);

                strcpy(Msg->path2, mod_path);

                if (send(ss[a]->number_access, Msg, sizeof(MsgToSS), 0) < 0) // modify the code to replace the storage server socket number from accept command
                {
                    perror("Error sending the command packet to the storage server\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {

                strcpy(path_from_cpy, buffer->path1);
                strcpy(path_to_cpy, buffer->path2);
                if (strcmp("COPY_D", string) == 0)
                {
                    ssd_port = act_port2;
                    file_port = act_port1;
                    client_id_copy_dir = sd;
                    handle_copy_directory(act_port1, act_port2, buffer->path1, buffer->path2, buffer->FromID);
                    return;
                }

                int gh;
                for (gh = 0; gh < client_id_struct_index; gh++)
                {
                    if (sd == client[gh].fd)
                    {
                        client_id_copy = client[gh].client_id;
                        break;
                    }
                }
                for (a = 0; a < storage_count; a++)
                {
                    if (act_port1 == ss[a]->port)
                    {
                        break;
                    }
                }
                Msg->CopyFlag = 'F';

                if (send(ss[a]->number_access, Msg, sizeof(MsgToSS), 0) < 0) // modify the code to replace the storage server socket number from accept command
                {
                    perror("Error sending the command packet to the storage server\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}
int main()
{
    signal(SIGINT, ctrlC_Handler);
    int client_count = 0;
    int s_sock;
    node_LRU head1 = NULL;
    log head = (log)malloc(sizeof(struct log));
    head->next = NULL;
    log front;
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sock == -1)
    {
        printf("error establishing the socket\n");
        return 0;
    }
    struct sockaddr_in server_addr, client_addr, storage_addr;
    socklen_t client_len = sizeof(client_addr);
    socklen_t storage_len = sizeof(storage_addr);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(NM_PORT);
    if (bind(s_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("Error binding\n");
        close(s_sock);
        return 0;
    }
    if (listen(s_sock, MAX_CLIENT + MAX_STORAGE) == -1)
    {
        printf("Error listening\n");
        close(s_sock);
        return 0;
    }
    int client_only[MAX_CLIENT] = {0};

    // struct timeval timeout;

    int client_sockets[MAX_CLIENT + MAX_STORAGE] = {0};
    fd_set readfds;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(s_sock, &readfds);
        int max_sd = s_sock;
        // timeout.tv_sec = 1;
        // timeout.tv_usec = 0;
        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            int sd = client_sockets[i];

            if (sd > 0)
            {
                FD_SET(sd, &readfds);
            }

            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0)
        {
            perror("Select error");
            exit(1);
        }
        int new_socket;
        // Check for incoming connections on the server socket
        if (FD_ISSET(s_sock, &readfds))
        {
            new_socket = accept(s_sock, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
            flag = 1;
            if (new_socket == -1)
            {
                perror("Accept failed");
                exit(1);
            }

            printf("New connection, socket fd is %d\n", new_socket);

            // Add new socket to array of client sockets
            for (int i = 0; i < MAX_CLIENT + MAX_STORAGE; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Check for data from clients
        int i;
        for (i = 0; i < MAX_CLIENT + MAX_STORAGE; i++)
        {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds))
            {
                MsgToNM *buffer;
                buffer = (MsgToNM *)malloc(sizeof(MsgToNM));

                int valread = recv(sd, buffer, sizeof(MsgToNM), 0);
                if (valread <= 0)
                {
                    for (int y = 0; y < storage_count; y++)
                    {
                        if (ss[y]->number_access == sd)
                        {
                            printf("Storage server %d disconnected\n", y + 1);
                            break;
                        }
                    }
                    // Connection closed or error, remove socket from array

                    close(sd);
                    client_sockets[i] = 0;
                }
                else
                {

                    handle_comm_client(head, head1, client_addr, client_len, buffer, sd);

                    // Process the received data
                    // printf("Client %d: %s\n", sd, buffer);
                }
            }
        }
    }

    return 0;
}
void copy_file()
{
    MsgToSS *Msg;
    Msg = (MsgToSS *)malloc(sizeof(Msg));
    strcpy(Msg->Command, "COPY");
    strcpy(Msg->path1, first_copy_path);
    Msg->CopyFlag = 'F';
    Msg->From = 'N';
    Msg->copy_dir_flag = 1;
    Msg->FromID = from_copy_dir;
    copy_dir_travel_index = 0;
    if (send(index_copy_dir, Msg, sizeof(MsgToSS), 0) < 0)
    {
        perror("Couldnot send the data\n");
        exit(EXIT_FAILURE);
    }
}

void handle_copy_directory(int act_port1, int act_port2, char *path1, char *path2, int FromID)
{
    int i;
    for (i = 0; i < storage_count; i++)
    {
        if (ss[i]->port == act_port1)
            break;
    }
    int rec_sock = ss[i]->number_access;
    MsgToSS *Msg;
    Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
    strcpy(Msg->Command, "COPY_D");
    strcpy(Msg->path1, path1);
    strcpy(Msg->path2, path2);
    Msg->From = 'N';
    Msg->FromID = FromID;
    if (send(rec_sock, Msg, sizeof(MsgToSS), 0) < 0) // modify the code to replace the storage server socket number from accept command
    {
        perror("Error sending the command packet to the storage server\n");
        exit(EXIT_FAILURE);
    }
    return;
}
