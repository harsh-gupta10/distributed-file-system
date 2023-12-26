#include "header.h"

int SS_PORT;
int sockfd;
int server_sock;
socklen_t addr_size;
struct sockaddr_in client_addr;
int client_sockets[MAX_CLIENT] = {0};
int NoOfClientConnected=0;
// typedef struct MsgToNM
// {
//     char ip[16];
//     int nmPort;
//     int SSport;
//     char accessiblePaths[NO_ACCESSIBLE_PATH][ACCESSIBLE_PATH_LENGTH];
// } MsgToNM;

// typedef struct SSinfoToClient
// {
//     char ip[16];
//     int SSport;
// } SSinfoToClient;

struct QueueNode
{
    char path[PATH_LENGTH];
    struct QueueNode *next;
};

// Queue structure
struct Queue
{
    struct QueueNode *front;
    struct QueueNode *rear;
};

// Function to initialize an empty queue
void initQueue(struct Queue *q)
{
    q->front = q->rear = NULL;
}

// Function to check if the queue is empty
int isEmpty(struct Queue *q)
{
    return q->front == NULL;
}

// Function to enqueue a path into the queue
void enqueue(struct Queue *q, const char *path)
{
    struct QueueNode *newNode = (struct QueueNode *)malloc(sizeof(struct QueueNode));
    strncpy(newNode->path, path, PATH_LENGTH);
    newNode->next = NULL;

    if (isEmpty(q))
    {
        q->front = q->rear = newNode;
    }
    else
    {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// Function to dequeue a path from the queue
void dequeue(struct Queue *q, char *path)
{
    if (isEmpty(q))
    {
        path[0] = '\0'; // Empty string if the queue is empty
        return;
    }

    struct QueueNode *temp = q->front;
    strncpy(path, temp->path, PATH_LENGTH);
    q->front = temp->next;

    free(temp);
}

// Function to perform BFS traversal of directories and files
void bfsTraversal(const char *rootDir, char bfsTravel[][PATH_LENGTH], int *count)
{
    struct Queue q;
    initQueue(&q);
    enqueue(&q, rootDir);
    strncpy(bfsTravel[(*count)++], rootDir, PATH_LENGTH);
    while (!isEmpty(&q))
    {
        char currentPath[PATH_LENGTH];
        dequeue(&q, currentPath);
        DIR *dir = opendir(currentPath);
        if (dir != NULL)
        {
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL)
            {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    char newPath[PATH_LENGTH];
                    snprintf(newPath, PATH_LENGTH, "%s/%s", currentPath, entry->d_name);
                    enqueue(&q, newPath);
                    strncpy(bfsTravel[(*count)++], newPath, PATH_LENGTH);
                }
            }
            closedir(dir);
        }
    }
}

void make_Port()
{
    char *ip = "127.0.0.1";
    int port = SS_PORT;

    struct sockaddr_in server_addr, client_addr;

    char buffer[1024];
    char choiceA_buf[1024], choiceB_buf[1024];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (server_addr.sin_addr.s_addr == INADDR_NONE)
    {
        perror("[-]Invalid IP address");
        close(server_sock);
        exit(1);
    }

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        close(server_sock);
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);
    if (listen(server_sock, 5) < 0)
    {
        perror("[-]Listen error");
        close(server_sock);
        exit(1);
    }
    // printf("Listening...\n");
}
void PrintNMrecvMsg(MsgToNM *Msg)
{
    printf("ACK %d:\n", Msg->ack);
    printf("Command %s:\n", Msg->command);
    printf("From %c:\n", Msg->From);
    printf("IP %s:\n", Msg->ip);
    printf("Name_Port %d:\n", Msg->name_port);
    printf("number_access %d:\n", Msg->number_access);
    printf("path %s:\n", Msg->path);
    printf("port %d:\n", Msg->port);
    printf("From id %d\n", Msg->FromID);
}

void PrintSSrecvMsg(MsgToSS *Msg)
{
    printf("From %c:\n", Msg->From);
    printf("Command %s:\n", Msg->Command);
    printf("Path %s:\n", Msg->path);
    printf("To_write %s:\n", Msg->To_write);
    printf("Done_job %d:\n", Msg->done_Job);
    printf("From id %d\n", Msg->FromID);
}

void PrintCLIENTrecvMsg(MsgToCLIENT *Msg)
{
    printf("From %c:\n", Msg->From);
    printf("Result %s:\n", Msg->Result);
    printf("ACK %d:\n", Msg->Ack);
    printf("Done job %d:\n", Msg->done_Job);
    printf("Port SS %d:\n", Msg->port_SS);
    printf("IP %s:\n", Msg->ip);
    printf("From id %d\n", Msg->FromID);
}

// Function to register the Storage Server with the Naming Server
void registerWithNM(struct MsgToNM *ssInfo)
{
    struct sockaddr_in serverAddr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(NM_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(NM_IP);

    // Connect to the NM server
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Connection to NM server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    ssInfo->noAccessiblePathInitially = 0;
    int hhhh = 0;
    // Find paths in the current working directory
    findPaths(".", ssInfo->Path2dArr, &hhhh);
    // Display the paths
    ssInfo->noAccessiblePathInitially = hhhh;
    printf("Found %d paths:\n", ssInfo->noAccessiblePathInitially);
    ssInfo->ack = ssInfo->noAccessiblePathInitially;
    printf("Ack %d\n", ssInfo->ack);
    if (send(sockfd, ssInfo, sizeof(struct MsgToNM), 0) == -1)
    {
        perror("Failed to send data to NM server");
    }
}

void print_permissions(mode_t mode, char *buffer)
{
    sprintf(buffer, "%cr%c%c%c%c%c%c%c%c",
            (mode & S_IRUSR) ? 'r' : '-',
            (mode & S_IWUSR) ? 'w' : '-',
            (mode & S_IXUSR) ? 'x' : '-',
            (mode & S_IRGRP) ? 'r' : '-',
            (mode & S_IWGRP) ? 'w' : '-',
            (mode & S_IXGRP) ? 'x' : '-',
            (mode & S_IROTH) ? 'r' : '-',
            (mode & S_IWOTH) ? 'w' : '-',
            (mode & S_IXOTH) ? 'x' : '-');
}

int handleFileCommands()
{

    addr_size = sizeof(client_addr);
    int client_socks[MAX_CLIENT], sd;

    // Initialize client_socks array
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        client_socks[i] = 0;
    }
    fd_set readfds;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(server_sock, &readfds);
        int max_sd = server_sock;

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            sd = client_socks[i];
            if (sd > 0)
            {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        // Wait for activity on any of the sockets
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0)
        {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        // Check for incoming connection
        if (FD_ISSET(server_sock, &readfds))
        {
            int new_socket = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
            NoOfClientConnected++;
            if (new_socket < 0)
            {
                perror("Accept error");
                exit(EXIT_FAILURE);
            }
            printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
                   new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add new socket to array of sockets
            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (client_socks[i] == 0)
                {
                    client_socks[i] = new_socket;
                    printf("Adding to list of sockets at index %d\n", i);
                    break;
                }
            }
        }

        // Check for data from clients
        for (int ii = 0; ii < MAX_CLIENT; ii++)
        {
            sd = client_socks[ii];

            if (FD_ISSET(sd, &readfds))
            {
                MsgToSS *Msg;
                Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
                int valread = recv(sd, Msg, sizeof(MsgToSS), 0);

                if (valread <= 0)
                {
                    // Some error or connection closed by client
                    getpeername(sd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size);
                    printf("Host disconnected, ip %s, port %d\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socks[ii] = 0;
                }
                else
                {
                    // Handle received data here
                    // ...
                    PrintSSrecvMsg(Msg);

                    // addr_size = sizeof(client_addr);
                    // int clientA = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
                    // if (clientA < 0)
                    // {
                    //     perror("[-]ClientA connection error");
                    //     // continue; // Move to the next iteration to keep listening
                    // }
                    // printf("[+]Clients connected.\n");
                    // //! abhi stop iplement nhi kiya
                    // MsgToSS *Msg;
                    // Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
                    // int valread = recv(clientA, Msg, sizeof(MsgToSS), 0);
                    // PrintSSrecvMsg(Msg);
                    // if (valread <= 0)
                    // {
                    //     perror("Connection closed or error, remove socket from array\n");
                    //     close(clientA);
                    // }
                    // Process the received command and execute the corresponding action
                    printf("Command recived is %s and Path is %s\n", Msg->Command, Msg->path);
                    if (strcmp(Msg->Command, "READ") == 0)
                    {
                        // Open file -> read it -> send data to client
                        FILE *file;
                        char buffer[4048] = {'\0'};
                        // Open the file for reading
                        printf("PATH %s\n", Msg->path);
                        file = fopen(Msg->path, "r");
                        if (file == NULL)
                        {
                            MsgToCLIENT *M1 = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                            M1->Ack = 0;
                            M1->From = 'S';
                            M1->FromID = Msg->FromID;
                            // strcpy(M1->Result, buffer);
                            M1->error_code = ERROR_READ;
                            // printf("Hello %d\n",M1->error_code);
                            send(sd, M1, sizeof(MsgToCLIENT), 0);
                            perror("File open failed");
                            return -1;
                        }
                        // Read and send data to the client
                        while (!feof(file))
                        {
                            size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
                            // sleep(20);
                            // int hh
                            if (bytesRead > 0)
                            {
                                MsgToCLIENT *M1 = malloc(sizeof(MsgToCLIENT));
                                M1->Ack = 1;
                                M1->From = 'S';
                                M1->FromID = Msg->FromID;
                                strcpy(M1->Result, buffer);
                                printf("Sending This to client %s\n", M1->Result);
                                send(sd, M1, sizeof(MsgToCLIENT), 0);
                            }
                        }
                        fclose(file);
                    }
                    else if (strcmp(Msg->Command, "WRITE") == 0)
                    {
                        // create file , ask for what to write to client , then clint sends data then here we write that thing
                        FILE *file;
                        char buffer[1024];

                        // Create the file for writing
                        file = fopen(Msg->path, "w");
                        if (file == NULL)
                        {
                            perror("File creation failed");
                            MsgToCLIENT *M1 = malloc(sizeof(MsgToCLIENT));
                            M1->Ack = 0;
                            M1->done_Job = 1;
                            M1->From = 'S';
                            M1->error_code = ERROR_WRITE;
                            if (send(sd, M1, sizeof(MsgToCLIENT), 0) == -1)
                            {

                                perror("Failed to send ACK");
                            }
                            return -1;
                        }
                        fwrite(Msg->To_write, 1, strlen(Msg->To_write), file); // ! ye 1 kyu h

                        // Close the file and the client socket
                        fclose(file);
                        MsgToCLIENT *M1 = malloc(sizeof(MsgToCLIENT));
                        M1->Ack = 1;
                        M1->done_Job = 1;
                        M1->From = 'S';
                        // send(sd, M1, sizeof(M1), 0);
                        // printf("Writing done Sended ACK also\n");
                        if (send(sd, M1, sizeof(MsgToCLIENT), 0) == -1)
                        {

                            perror("Failed to send ACK");
                        }
                        else
                        {
                            printf("Writing Done Sended ACK also\n");
                        }
                    }
                    else if (strcmp(Msg->Command, "INFO") == 0)
                    {
                        MsgToCLIENT *AllInfo = malloc(sizeof(MsgToCLIENT));
                        AllInfo->Ack = 0;
                        AllInfo->done_Job = 0;
                        AllInfo->From = 'S';

                        char *filename = malloc(sizeof(char) * (PATH_LENGTH));
                        strcpy(filename, Msg->path);

                        struct stat file_stat;

                        // Use stat to get information about the file
                        if (stat(filename, &file_stat) != 0)
                        {
                            perror("Error getting file information");
                            AllInfo->error_code = ERROR_INFO;
                            if (send(sd, AllInfo, sizeof(MsgToCLIENT), 0) == -1)
                            {
                                perror("Error in sending INFO\n");
                            }
                            return 1;
                        }
                        // Combine all information into a single string
                        char file_info[1024]; // Adjust the size as needed
                        // File permissions
                        char permissions[11];
                        print_permissions(file_stat.st_mode, permissions);
                        // Owner and group
                        struct passwd *owner_info = getpwuid(file_stat.st_uid);
                        struct group *group_info = getgrgid(file_stat.st_gid);
                        // Format the string with file information
                        sprintf(file_info, "File: %s\n"
                                           "Permissions: %s\n"
                                           "Number of hard links: %lu\n"
                                           "Owner: %s\n"
                                           "Group: %s\n"
                                           "Size: %lld bytes\n"
                                           "Last modified: %s\n",
                                filename, permissions, (unsigned long)file_stat.st_nlink,
                                owner_info->pw_name, group_info->gr_name,
                                (long long)file_stat.st_size,
                                ctime(&file_stat.st_mtime));
                        // Print the file information
                        printf("%s", file_info);
                        strcpy(AllInfo->Result, file_info);
                        AllInfo->Ack = 1;
                        if (send(sd, AllInfo, sizeof(MsgToCLIENT), 0) == -1)
                        {
                            perror("Error in sending INFO\n");
                        }
                        printf("sended info Sended ACK also\n");
                    }
                    NoOfClientConnected--;
                    if (NoOfClientConnected==0)
                    {
                        return;
                    }
                    
                }
            }
        }
    }
    // ? sab thik so return 1;
}

// Function to process and execute commands received from NM
// void processCommand(int sd)
void processCommand(MsgToSS *Msg)
{
    char response[1024];
    // Process and execute the command

    if (strcmp(Msg->Command, "CREATE") == 0)
    {
        // Create an empty file, command format: CREATE_FILE <file_path>
        // char filePath[256];
        // sscanf(buffer, "CREATE %s", filePath);
        FILE *newFile = fopen(Msg->path, "w");
        if (newFile)
        {
            printf("File created scuessfully..\n");
            fclose(newFile);
            Msg->done_Job = 1;

            MsgToNM *M1 = malloc(sizeof(MsgToNM));
            M1->ack = 1;
            M1->FromID = Msg->FromID;
            strcpy(M1->command, Msg->Command);
            strcpy(M1->path, Msg->path);
            M1->From = 'S';
            M1->if_copy = 0;
            // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
            if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
            {
                perror("Failed to send ACK to NM server");
            }
            else
            {
                printf("ACK sended to NM server");
            }
            // sprintf(response, "File created: %s", filePath);
        }
        else
        {
            Msg->done_Job = -1;
            MsgToNM *M1 = malloc(sizeof(MsgToNM));
            M1->ack = 0;
            M1->FromID = Msg->FromID;
            strcpy(M1->command, Msg->Command);
            strcpy(M1->path, Msg->path);
            M1->From = 'S';
            M1->error_code = ERROR_CREATE;
            M1->if_copy = 0;
            // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
            if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
            {
                perror("Failed to send ACK to NM server");
            }
            perror("Failed to create file");
            // sprintf(response, "Failed to create file: %s", filePath);
        }
    }
    else if (strcmp(Msg->Command, "CREATE_D") == 0)
    {
        // Create an empty file, command format: CREATE_FILE <file_path>
        // char filePath[256];
        // sscanf(buffer, "CREATE %s", filePath);
        // FILE *newFile = fopen(Msg->path, "w");
        int status = mkdir(Msg->path, S_IRWXU);
        if (status == 0)
        {
            printf("Directory created successfully..\n");
            Msg->done_Job = 1;
            MsgToNM *M1 = malloc(sizeof(MsgToNM));
            M1->ack = 1;
            M1->FromID = Msg->FromID;
            strcpy(M1->path, Msg->path);
            M1->From = 'S';
            M1->if_copy = 0;
            // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
            if (Msg->copy_dir_flag == 1)
            {
                M1->if_copy = 1;
                // strcpy(M1->command,"COPY_D");
                return;
            }
            else
                strcpy(M1->command, Msg->Command);

            if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
            {
                perror("Failed to send ACK to NM server");
            }
            else
            {
                printf("ACK sended to NM server");
            }
            // sprintf(response, "File created: %s", filePath);
        }
        else
        {

            Msg->done_Job = -1;
            MsgToNM *M1 = malloc(sizeof(MsgToNM));
            M1->ack = 0;
            M1->FromID = Msg->FromID;
            strcpy(M1->command, Msg->Command);
            strcpy(M1->path, Msg->path);
            M1->From = 'S';
            M1->error_code = ERROR_CREATE_D;
            M1->if_copy = 0;
            // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
            if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
            {
                perror("Failed to send ACK to NM server");
            }
            perror("Failed to create Directory");
            // sprintf(response, "Failed to create file: %s", filePath);
        }
    }
    else if (strcmp(Msg->Command, "DELETE") == 0)
    {
        // Delete a file or directory, command format: DELETE <file_path>
        // char pathToDelete[256];
        // sscanf(buffer, "DELETE %s", pathToDelete);
        printf("\n\n1\n");
        int u;
        int flag = 0;
        for (u = 0; u < strlen(Msg->path); u++)
        {
            if (Msg->path[u] == '.')
            {
                flag = 1;
            }
        }
        if (flag == 1)
        {
            int return_remove = remove(Msg->path);
            printf("return_remove %d\n", return_remove);
            if (return_remove == 0)
            {
                printf("DELETED FILE\n");
                MsgToNM *M1 = malloc(sizeof(MsgToNM));
                M1->ack = 1;
                strcpy(M1->command, Msg->Command);
                strcpy(M1->path, Msg->path);
                M1->From = 'S';
                M1->FromID = Msg->FromID;
                M1->if_copy = 0;
                // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
                if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
                {
                    perror("Failed to send ACK to NM server");
                }
                else
                {
                    printf("ACK sended to NM server\n");
                }
            }
            else
            {

                printf("Else exec\n");
                Msg->done_Job = -1;
                MsgToNM *M1 = malloc(sizeof(MsgToNM));
                M1->ack = 0;
                M1->FromID = Msg->FromID;
                strcpy(M1->command, Msg->Command);
                strcpy(M1->path, Msg->path);
                M1->From = 'S';
                M1->error_code = ERROR_DELETE;
                M1->if_copy = 0;
                // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
                if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
                {
                    perror("Failed to send ACK to NM server");
                }
                perror("Failed to delete file");
                // sprintf(response, "Failed to delete: %s", pathToDelete);
            }
        }
        else if (flag == 0)
        {
            char command[256]; // Adjust the buffer size as needed
            snprintf(command, sizeof(command), "rm -r %s", Msg->path);

            // Execute the command using system
            int status = system(command);
            if (status == 0)
            {
                printf("DELETED DIRECTORY\n");
                MsgToNM *M1 = malloc(sizeof(MsgToNM));
                M1->ack = 1;
                strcpy(M1->command, Msg->Command);
                strcpy(M1->path, Msg->path);
                M1->From = 'S';
                M1->FromID = Msg->FromID;
                M1->if_copy = 0;
                // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
                if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
                {
                    perror("Failed to send ACK to NM server");
                }
                else
                {
                    printf("ACK sended to NM server\n");
                }
            }
            else
            {
                printf("Else exec\n");
                Msg->done_Job = -1;
                MsgToNM *M1 = malloc(sizeof(MsgToNM));
                M1->ack = 0;
                M1->FromID = Msg->FromID;
                strcpy(M1->command, Msg->Command);
                strcpy(M1->path, Msg->path);
                M1->From = 'S';
                M1->error_code = ERROR_DELETE;
                M1->if_copy = 0;
                // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
                if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
                {
                    perror("Failed to send ACK to NM server");
                }
                perror("Failed to delete file");
                // sprintf(response, "Failed to delete: %s", pathToDelete);
            }
        }
    }

    else if (strcmp(Msg->Command, "COPY") == 0)
    {
        // char buffer[1024];
        // Copy a file or directory, command format: COPY <source_path> <destination_path>
        // char sourcePath[256], destinationPath[256];
        const char *sourcePath;      // Replace with the actual source directory
        const char *destinationPath; // Replace with the actual destination directory
        // sscanf(Msg->path, "%s %s", sourcePath, destinationPath);
        if (Msg->CopyFlag == 'B')
        {
            FILE *sourceFile, *destinationFile;
            char ch;
            sourceFile = fopen(Msg->path1, "r");
            destinationFile = fopen(Msg->path2, "w");
            printf("Path 1 %s\n", Msg->path1);
            printf("Path 2 %s\n", Msg->path2);

            if (sourceFile == NULL || destinationFile == NULL)
            {
                perror("Error opening either file");
                MsgToNM *M1 = (MsgToNM *)malloc(sizeof(MsgToNM));
                M1->ack = 0;
                M1->From = 'S';
                M1->FromID = Msg->FromID;
                M1->error_code = ERROR_COPY;
                M1->if_copy = 0;
                strcpy(M1->command, Msg->Command);
                send(sockfd, M1, sizeof(MsgToNM), 0);
                perror("File open failed");
                return -1;
            }

            // Open the destination file for writing

            if (destinationFile == NULL)
            {
                perror("Error opening destination file");
                fclose(sourceFile);
                return 2;
            }

            // Copy the contents of the source file to the destination file
            while ((ch = fgetc(sourceFile)) != EOF)
            {
                fputc(ch, destinationFile);
            }

            // Close the files
            fclose(sourceFile);
            fclose(destinationFile);

            printf("File copied successfully.\n");
            MsgToNM *M1 = malloc(sizeof(MsgToNM));
            M1->ack = 1;
            M1->FromID = Msg->FromID;
            strcpy(M1->command, Msg->Command);
            strcpy(M1->path, Msg->path);
            M1->From = 'S';
            M1->if_copy = 0;
            printf("M1 prepared\n");
            // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
            int sendResut = send(sockfd, M1, sizeof(struct MsgToNM), 0);
            printf("send Result %d\n", sendResut);
            if (sendResut == -1)
            {
                perror("Failed to send ACK to NM server\n");
            }
            printf("ACK sended to NM server\n");
        }
        else if (Msg->CopyFlag == 'T')
        {
            // MsgToSS *FileData;
            // FileData = (MsgToSS *)malloc(sizeof(MsgToSS));
            // if (recv(sockfd, FileData, sizeof(MsgToSS), 0) <= 0)
            // {
            //     perror("Error in  reciving inital Msg from NM\n");
            // }
            FILE *destinationFile;
            destinationFile = fopen(Msg->path2, "w");
            if (destinationFile == NULL)
            {
                MsgToNM *M1 = (MsgToNM *)malloc(sizeof(MsgToNM));
                M1->ack = 0;
                M1->From = 'S';
                M1->FromID = Msg->FromID;
                M1->error_code = ERROR_COPY;
                strcpy(M1->command, Msg->Command);
                M1->if_copy = 0;
                strcpy(M1->command, Msg->Command);
                send(sockfd, M1, sizeof(MsgToNM), 0);
                perror("File open failed");
                return -1;
            }
            int chIndex = 0;
            while (Msg->To_write[chIndex] != '\0')
            {
                fputc(Msg->To_write[chIndex], destinationFile);
                chIndex++;
            }
            fclose(destinationFile);
            printf("File copied successfully.\n");
            MsgToNM *M1 = malloc(sizeof(MsgToNM));
            M1->ack = 1;
            M1->FromID = Msg->FromID;
            strcpy(M1->command, Msg->Command);
            strcpy(M1->path, Msg->path);
            M1->From = 'S';
            if (Msg->copy_dir_flag == 1)
            {
                M1->copy_dir_flag = 0;
                M1->rec_copy_dir = 1;
                M1->if_copy = 1;
            }
            else
            {
                M1->if_copy = 0;
            }
            // !! isko update karna h mereko NM isko change kardega baad me  M1->accessible_path
            if (send(sockfd, M1, sizeof(struct MsgToNM), 0) == -1)
            {
                perror("Failed to send ACK to NM server");
            }
            else
            {
                printf("ACK sended to NM server");
            }
        }
        else if (Msg->CopyFlag == 'F')
        {
            FILE *file;
            char buffer[4048] = {'\0'};
            // Open the file for reading
            file = fopen(Msg->path1, "r");
            if (file == NULL)
            {
                MsgToNM *M1 = (MsgToNM *)malloc(sizeof(MsgToNM));
                M1->ack = 0;
                M1->From = 'S';
                M1->FromID = Msg->FromID;
                M1->error_code = ERROR_COPY;
                strcpy(M1->command, Msg->Command);
                M1->if_copy = 1;

                send(sockfd, M1, sizeof(MsgToNM), 0);
                perror("File open failed");
                return -1;
            }
            // Read and send data to the client
            while (!feof(file))
            {
                size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
                if (bytesRead > 0)
                {
                    MsgToNM *M1 = malloc(sizeof(MsgToNM));
                    M1->ack = 1;
                    M1->From = 'S';
                    M1->FromID = Msg->FromID;
                    strcpy(M1->data, buffer);
                    strcpy(M1->command, Msg->Command);
                    if (Msg->copy_dir_flag == 1)
                    {
                        M1->copy_dir_flag = 1;
                        M1->rec_copy_dir = 0;
                    }
                    M1->if_copy = 1;
                    printf("[COPY]: Sending This to client %s\n", M1->data);
                    if (send(sockfd, M1, sizeof(MsgToNM), 0) == -1)
                    {
                        perror("Failed to send ACK to NM server");
                    }
                    else
                    {
                        printf("ACK sended to NM server");
                    }
                }
            }
        }
    }
    else if (strcmp(Msg->Command, "COPY_D") == 0)
    {
        char *rootDir; // Replace with your root directory
        rootDir = malloc(sizeof(PATH_LENGTH));
        strcpy(rootDir, Msg->path1);
        char bfsTravel[PATH_MAXIMUM][PATH_LENGTH];
        int count = 0;
        bfsTraversal(rootDir, bfsTravel, &count);
        // Display the result

        MsgToNM *M1 = (MsgToNM *)malloc(sizeof(MsgToNM));
        M1->ack = 0;
        M1->From = 'S';
        M1->FromID = Msg->FromID;
        strcpy(M1->command, Msg->Command);
        M1->if_copy = 1;
        //! not creating new var sending in this
        M1->ack = count;
        printf("No of path %d\n", M1->ack);
        for (int i = 0; i < count; i++)
        {
            strcpy(M1->Path2dArr[i], bfsTravel[i]);
            printf("M1->Path2dArr[%d]=%s\n", i, M1->Path2dArr[i]);
        }

        if (send(sockfd, M1, sizeof(MsgToNM), 0) == -1)
        {
            perror("Unable To send\n");
        }
        printf("Sended all path sucessfully");
    }

    else if ((strcmp(Msg->Command, "READ") == 0) || (strcmp(Msg->Command, "WRITE") == 0) || (strcmp(Msg->Command, "INFO") == 0))
    {

        printf("Need To connect to client directly...\n");
        handleFileCommands();
        // sprintf(response, "Read command received: %s", filePath);
    }
}

void handle()
{
    MsgToSS *Msg;
    Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
    if (recv(sockfd, Msg, sizeof(MsgToSS), 0) <= 0)
    {
        perror("Error in  reciving inital Msg from NM\n");
    }
    PrintSSrecvMsg(Msg);
    processCommand(Msg);
}
void ctrlC_Handler(int signal)
{
    printf("Ctrl+C pressed. Closing Sockets... Exiting...\n");
    close(sockfd);
    exit(0);
}

void findPaths(const char *basePath, char Path2dArr[NO_ACCESSIBLE_PATH][PATH_LENGTH], int *pathCount)
{
    struct dirent *dp;
    struct stat fileStat;
    char path[PATH_LENGTH];

    DIR *dir = opendir(basePath);

    if (!dir)
    {
        fprintf(stderr, "Cannot open directory: %s\n", basePath);
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            snprintf(path, sizeof(path), "%s/%s", basePath, dp->d_name);

            if (stat(path, &fileStat) < 0)
            {
                fprintf(stderr, "Error getting file information for %s\n", path);
                continue;
            }

            if (S_ISDIR(fileStat.st_mode))
            {
                // Recursive call for directories
                findPaths(path, Path2dArr, pathCount);
            }
            else
            {
                // Store relative path
                snprintf(Path2dArr[*pathCount], PATH_LENGTH, "%s", path + 2); // +2 to skip "./" in the path
                (*pathCount)++;
            }
        }
    }

    closedir(dir);
}

void SendAccesiblePath(MsgToNM *ssInfo1)
{
    // char cwd[PATH_LENGTH];
    // if (getcwd(cwd, sizeof(cwd)) == NULL)
    // {
    //     perror("getcwd() error");
    //     return 1;
    // }
    ssInfo1->noAccessiblePathInitially = 0;
    // Find paths in the current working directory
    findPaths(".", ssInfo1->Path2dArr, &ssInfo1->noAccessiblePathInitially);
    // Display the paths
    printf("Found %d paths:\n", ssInfo1->noAccessiblePathInitially);
    for (int i = 0; i < ssInfo1->noAccessiblePathInitially; i++)
    {
        printf("%s\n", ssInfo1->Path2dArr[i]);
    }
}

int main()
{

    signal(SIGINT, ctrlC_Handler);
    printf("Enter the port number for the SS server:  ");
    scanf("%d", &SS_PORT);
    //* REgister with NM Start
    MsgToNM *ssInfo;
    ssInfo = (MsgToNM *)malloc(sizeof(MsgToNM));
    fd_set readfds;
    strcpy(ssInfo->ip, SS_IP); //! see this server ip
    ssInfo->port = SS_PORT;
    ssInfo->From = 'S';
    ssInfo->ack = 0;
    ssInfo->number_access = 1;
    ssInfo->name_port = NM_PORT;
    ssInfo->size = 0;
    ssInfo->noAccessiblePathInitially = 0;

    // ! Find all accible path and copy it here
    //! replace here alse
    registerWithNM(ssInfo);
    printf("Registerd SS to NM with SS Port %d:\n", SS_PORT);
    //* REgister with NM Start
    make_Port();
    while (1)
    {
        // Start listening for commands from NM
        handle();
    }
    // MsgToSS *Msg;
    // Msg = (MsgToSS *)malloc(sizeof(MsgToSS));
    // if(recv(sockfd, Msg, sizeof(MsgToSS), 0)<=0)
    // {
    //     perror("Error in  reciving inital Msg from NM\n");
    // }
    // // printf("Msg recived from %c \n", Msg->From);
    // // printf("Command:-%s\n", Msg->Command);
    // // printf("Path:-%s\n", Msg->path);
    // PrintSSrecvMsg(Msg);
    // ! ctrl +c se code band karo to usme close(sockfd) karna h varna vo khula rah jayega
    return 0;
}