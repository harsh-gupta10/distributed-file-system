#include "header.h"

int client_socket;
int client_id;
void send_request(int socket, MsgToNM *packet)
{
    send(socket, packet, sizeof(MsgToNM), 0);
}
void send_request_SS(int socket, MsgToSS *packet)
{
    if (send(socket, packet, sizeof(MsgToSS), 0) == -1)
    {
        perror("Could not send message\n");
    }
}

void PrintNMrecvMsg(MsgToNM *Msg)
{
    printf("ACK %d:\n", Msg->ack);
    // printf("Accessible path %s:\n", Msg->accessible_path[0]);
    printf("Command %s:\n", Msg->command);
    printf("From %c:\n", Msg->From);
    printf("IP %s:\n", Msg->ip);
    printf("Name_Port %d:\n", Msg->name_port);
    printf("number_access %d:\n", Msg->number_access);
    printf("path %s:\n", Msg->path);
    printf("port %d:\n", Msg->port);
}

void PrintSSrecvMsg(MsgToSS *Msg)
{
    printf("From %c:\n", Msg->From);
    printf("Command %s:\n", Msg->Command);
    printf("Path %s:\n", Msg->path);
    printf("To_write %s:\n", Msg->To_write);
    printf("Done_job %d:\n", Msg->done_Job);
}

void PrintCLIENTrecvMsg(MsgToCLIENT *Msg)
{
    printf("From %c:\n", Msg->From);
    printf("Result %s:\n", Msg->Result);
    printf("ACK %d:\n", Msg->Ack);
    printf("Done job %d:\n", Msg->done_Job);
    printf("Port SS %d:\n", Msg->port_SS);
    printf("IP %s:\n", Msg->ip);
}

void ctrlC_Handler(int signal)
{
    printf("Ctrl+C pressed. Closing Sockets... Exiting...\n");
    close(client_socket);
    exit(0);
}
int main()
{
    signal(SIGINT, ctrlC_Handler);

    struct sockaddr_in server_address;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(NM_PORT);
    server_address.sin_addr.s_addr = inet_addr(NM_IP);

    //  Connect to the Naming Server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection to Naming Server failed");
        exit(1);
    }
    printf("Enter clientID :");
    scanf("%d", &client_id);

    char use[100];
    fgets(use, 100, stdin);

    while (1)
    {
        char *request = (char *)malloc(sizeof(char) * 4096);

        // Take user input for the request
        printf("Enter your command: ");
        fgets(request, 4096, stdin);

        // Remove the newline character from the input
        size_t len = strlen(request);
        if (len > 0 && request[len - 1] == '\n')
        {

            request[len - 1] = '\0';
        }
        MsgToNM *packet;
        packet = (MsgToNM *)malloc(sizeof(MsgToNM));
        strcpy(packet->command, request);
        packet->FromID = client_id;
        char *request2;
        char *request4;
        char *request3;
        char *request1 = (char *)malloc(sizeof(char) * 4096);
        request2 = (char *)malloc(sizeof(char) * 4096);
        if (strcmp("LOG", packet->command) == 0)
        {
            MsgToNM *msg;
            msg = (MsgToNM *)malloc(sizeof(MsgToNM));
            strcpy(msg->command, "LOG");
            msg->From = 'C';
            send_request(client_socket, msg);
            continue;
        }
        if (strcmp("COPY", packet->command) != 0 && strcmp("COPY_D", packet->command) != 0)
        {
            printf("Enter your path: ");
            fgets(request1, 4096, stdin);
            request1[strlen(request1) - 1] = '\0';
            strcpy(packet->path, request1);
        }
        packet->From = 'C';
        if (strcmp("WRITE", packet->command) == 0)
        {

            printf("Enter your data: ");
            fgets(request2, 4096, stdin);
            len = strlen(request2);
            if (len > 0 && request2[len - 1] == '\n')
            {

                request2[len - 1] = '\0';
            }
            strcpy(packet->data, request2);
            //  printf("%s %s %s\n ",packet->command,packet->path,request2);
            // copy the write command content
        }
        if (strcmp("COPY", packet->command) == 0 || strcmp("COPY_D", packet->command) == 0)
        {
            request3 = (char *)malloc(sizeof(char) * 4096);

            printf("Enter your path1: ");
            fgets(request3, 4096, stdin);
            len = strlen(request3);
            if (len > 0 && request3[len - 1] == '\n')
            {

                request3[len - 1] = '\0';
            }
            request4 = (char *)malloc(sizeof(char) * 4096);

            printf("Enter your path2: ");
            fgets(request4, 4096, stdin);
            size_t len = strlen(request4);
            if (len > 0 && request4[len - 1] == '\n')
            {

                request4[len - 1] = '\0';
            }
            strcpy(packet->path1, request3);
            strcpy(packet->path2, request4);
            // copy the copy content
        }

        send_request(client_socket, packet);
        if ((strcmp(packet->command, "READ") == 0) || (strcmp(packet->command, "WRITE") == 0) || (strcmp(packet->command, "LS") == 0) || (strcmp(packet->command, "INFO") == 0))
        {
            struct MsgToCLIENT *servercom;
            servercom = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));

            int info_received = recv(client_socket, servercom, sizeof(MsgToCLIENT), 0);
            if (info_received > 0)
            {
                if (servercom->Ack == 0)
                {
                    if (servercom->error_code == ERROR_READ)
                    {
                        printf("The file could not be opened beacuse it doesnot exist\n");
                    }
                    else if (servercom->error_code = ERROR_WRITE)
                    {
                        printf("The write command could not executed\n");
                    }
                    else
                    {
                        printf("The information regarding the file could not be fetched\n");
                    }
                    continue;
                }
                printf("%s\n", servercom->ip);
                printf("Port %d\n", servercom->port_SS);

                char *ip = "127.0.0.1";
                int port = servercom->port_SS;

                int sock;
                struct sockaddr_in addr;
                socklen_t addr_size;
                char buffer[1024];
                char choice_buf[1024], result_buf[1024];
                int n;

                sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock < 0)
                {
                    perror("[-]Socket error");
                    exit(1);
                }
                printf("[+]TCP server socket created.\n");

                memset(&addr, '\0', sizeof(addr));
                addr.sin_family = AF_INET;
                addr.sin_port = port;
                addr.sin_addr.s_addr = inet_addr(ip);

                if (addr.sin_addr.s_addr == INADDR_NONE)
                {
                    perror("[-]Invalid IP address");
                    close(sock);
                    exit(1);
                }

                if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                {
                    perror("[-]Connection error");
                    close(sock);
                    exit(1);
                }

                printf("Connected to the server.\n");
                MsgToSS *message;
                message = (MsgToSS *)malloc(sizeof(MsgToSS));
                strcpy(message->Command, packet->command);
                strcpy(message->path, packet->path);
                if (strcmp(packet->command, "WRITE") == 0)
                    strcpy(message->To_write, request2);

                if (send(sock, message, sizeof(MsgToSS), 0) == -1)
                {
                    perror("Could not send message\n");
                }
                // send_request_SS(client_socket, message);

                MsgToCLIENT *ss_response;
                ss_response = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                int response_received = recv(sock, ss_response, sizeof(MsgToCLIENT), 0);
                if (response_received > 0)
                {
                    if (ss_response->Ack == 1)
                    {
                        if (strcmp("WRITE", packet->command) == 0)
                            printf("Writing successfully done\n");
                        else
                        {
                            printf("Received response from Storage Server: %s\n", ss_response->Result);
                        }
                    }

                    else
                    {

                        if (ss_response->error_code == ERROR_READ)
                        {
                            printf("The file could not be opened beacuse it doesnot exist\n");
                        }
                        else if (ss_response->error_code = ERROR_WRITE)
                        {
                            printf("The write command could not executed\n");
                        }
                        else
                        {
                            printf("The information regarding the file could not be fetched\n");
                        }
                    }
                }
                // MsgToCLIENT *Ack_from_ss;
                // Ack_from_ss = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
                // int bytes_recvied = recv(sock, Ack_from_ss, sizeof(MsgToCLIENT), 0);
                // if (bytes_recvied > 0)
                // {
                //     printf("Received Ack from Storage Server on Complete Of READ:\n");
                // }
                close(sock);
            }
        }
        // if ((strcmp(packet->command, "READ") == 0) || (strcmp(packet->command, "WRITE") == 0) || (strcmp(packet->command, "LS") == 0) || (strcmp(packet->command, "INFO") == 0))
        // {
        //     struct MsgToCLIENT *servercom;
        //     servercom = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));

        //     int info_received = recv(client_socket, servercom, sizeof(MsgToCLIENT), 0);
        //     if (info_received > 0)
        //     {
        //         printf("%d\n", servercom->flag);
        //         if (servercom->flag == 0)
        //         {
        //             printf("Hello hi\n");
        //             if (servercom->Ack == 0)
        //             {
        //                 printf("Hello what\n");
        //                 printf("%d\n", servercom->Ack);
        //                 if (servercom->error_code == ERROR_READ)
        //                 {
        //                     printf("The file could not be opened beacuse it doesnot exist\n");
        //                 }
        //                 else if (servercom->error_code == ERROR_WRITE)
        //                 {
        //                     printf("The write command could not executed\n");
        //                 }
        //                 else
        //                 {
        //                     printf("The information regarding the file could not be fetched\n");
        //                 }
        //                 continue;
        //             }
        //             // printf("%s\n", servercom->ip);
        //             printf("Port %d\n", servercom->port_SS);

        //             char *ip = "127.0.0.1";
        //             int port = servercom->port_SS;

        //             int sock;
        //             struct sockaddr_in addr;
        //             socklen_t addr_size;
        //             char buffer[1024];
        //             char choice_buf[1024], result_buf[1024];
        //             int n;

        //             sock = socket(AF_INET, SOCK_STREAM, 0);
        //             if (sock < 0)
        //             {
        //                 perror("[-]Socket error");
        //                 exit(1);
        //             }
        //             printf("[+]TCP server socket created.\n");

        //             memset(&addr, '\0', sizeof(addr));
        //             addr.sin_family = AF_INET;
        //             addr.sin_port = port;
        //             addr.sin_addr.s_addr = inet_addr(ip);

        //             if (addr.sin_addr.s_addr == INADDR_NONE)
        //             {
        //                 perror("[-]Invalid IP address");
        //                 close(sock);
        //                 exit(1);
        //             }

        //             if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        //             {
        //                 perror("[-]Connection error");
        //                 close(sock);
        //                 exit(1);
        //             }

        //             printf("Connected to the server.\n");
        //             MsgToSS *message;
        //             message = (MsgToSS *)malloc(sizeof(MsgToSS));
        //             strcpy(message->Command, packet->command);
        //             strcpy(message->path, packet->path);
        //             if (strcmp(packet->command, "WRITE") == 0)
        //                 strcpy(message->To_write, request2);

        //             if (send(sock, message, sizeof(MsgToSS), 0) == -1)
        //             {
        //                 perror("Could not send message\n");
        //             }
        //             // send_request_SS(client_socket, message);

        //             MsgToCLIENT *ss_response;
        //             ss_response = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
        //             int response_received = recv(sock, ss_response, sizeof(MsgToCLIENT), 0);
        //             if (response_received > 0)
        //             {
        //                 if (ss_response->Ack == 1)
        //                 {
        //                     if (strcmp("WRITE", packet->command) == 0)
        //                         printf("Writing successfully done\n");
        //                     else
        //                     {
        //                         printf("Received response from Storage Server: %s\n", ss_response->Result);
        //                     }
        //                 }

        //                 else
        //                 {

        //                     if (ss_response->error_code == ERROR_READ)
        //                     {
        //                         printf("The file could not be opened beacuse it doesnot exist\n");
        //                     }
        //                     else if (ss_response->error_code = ERROR_WRITE)
        //                     {
        //                         printf("The write command could not executed\n");
        //                     }
        //                     else
        //                     {
        //                         printf("The information regarding the file could not be fetched\n");
        //                     }
        //                 }
        //             }

        //             // MsgToCLIENT *Ack_from_ss;
        //             // Ack_from_ss = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
        //             // int bytes_recvied = recv(sock, Ack_from_ss, sizeof(MsgToCLIENT), 0);
        //             // if (bytes_recvied > 0)
        //             // {
        //             //     printf("Received Ack from Storage Server on Complete Of READ:\n");
        //             // }
        //             close(sock);
        //         }
        //         else
        //         {
        //             if (servercom->Ack == 0)
        //             {
        //                 if (servercom->error_code == ERROR_READ)
        //                 {
        //                     printf("The file could not be opened beacuse it doesnot exist\n");
        //                 }
        //                 else if (servercom->error_code == ERROR_WRITE)
        //                 {
        //                     printf("The write command could not executed\n");
        //                 }
        //                 else
        //                 {
        //                     printf("The information regarding the file could not be fetched\n");
        //                 }
        //                 continue;
        //             }
        //             // printf("%s\n", servercom->ip);
        //             // printf("Port %d\n", servercom->port_SS);

        //             char *ip = "127.0.0.1";
        //             int port = servercom->port_SS;
        //             int sock;
        //             struct sockaddr_in addr;
        //             socklen_t addr_size;
        //             char buffer[1024];
        //             char choice_buf[1024], result_buf[1024];
        //             int n;
        //             sock = socket(AF_INET, SOCK_STREAM, 0);
        //             if (sock < 0)
        //             {
        //                 perror("[-]Socket error");
        //                 exit(1);
        //             }
        //             printf("[+]TCP server socket created.\n");
        //             memset(&addr, '\0', sizeof(addr));
        //             addr.sin_family = AF_INET;
        //             addr.sin_port = port;
        //             addr.sin_addr.s_addr = inet_addr(ip);
        //             if (addr.sin_addr.s_addr == INADDR_NONE)
        //             {
        //                 perror("[-]Invalid IP address");
        //                 close(sock);
        //                 exit(1);
        //             }
        //             if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        //             {
        //                 perror("[-]Connection error");
        //                 close(sock);
        //                 exit(1);
        //             }
        //             printf("Connected to the server.\n");
        //             MsgToSS *message;
        //             message = (MsgToSS *)malloc(sizeof(MsgToSS));
        //             strcpy(message->Command, packet->command);
        //             strcpy(message->path, packet->path);
        //             if (strcmp(packet->command, "WRITE") == 0)
        //                 strcpy(message->To_write, request2);

        //             if (send(sock, message, sizeof(MsgToSS), 0) == -1)
        //             {
        //                 perror("Could not send message\n");
        //             }
        //             MsgToCLIENT *ss_response;
        //             ss_response = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
        //             int response_received = recv(sock, ss_response, sizeof(MsgToCLIENT), 0);
        //             if (response_received > 0)
        //             {
        //                 if (ss_response->Ack == 1)
        //                 {
        //                     if (strcmp("WRITE", packet->command) == 0)
        //                         printf("Writing successfully done\n");
        //                     else
        //                     {
        //                         printf("Received response from Storage Server: %s\n", ss_response->Result);
        //                     }
        //                 }
        //                 else
        //                 {
        //                     if (ss_response->error_code == ERROR_READ)
        //                     {
        //                         printf("The file could not be opened beacuse it doesnot exist\n");
        //                     }
        //                     else if (ss_response->error_code == ERROR_WRITE)
        //                     {
        //                         printf("The write command could not executed\n");
        //                     }
        //                     else
        //                     {
        //                         printf("The information regarding the file could not be fetched\n");
        //                     }
        //                 }
        //             }
        //             close(sock);

        //             // for num-1
        //             char *ip2 = "127.0.0.1";
        //             int port2 = servercom->port2;
        //             int sock2;
        //             struct sockaddr_in addr2;
        //             socklen_t addr_size2;
        //             char buffer2[1024];
        //             char choice_buf2[1024], result_buf2[1024];
        //             int n2;
        //             sock2 = socket(AF_INET, SOCK_STREAM, 0);
        //             if (sock2 < 0)
        //             {
        //                 perror("[-]Socket error");
        //                 exit(1);
        //             }
        //             printf("[+]TCP server socket created.\n");
        //             memset(&addr2, '\0', sizeof(addr2));
        //             addr2.sin_family = AF_INET;
        //             addr2.sin_port = port2;
        //             addr2.sin_addr.s_addr = inet_addr(ip2);
        //             if (addr2.sin_addr.s_addr == INADDR_NONE)
        //             {
        //                 perror("[-]Invalid IP address");
        //                 close(sock2);
        //                 exit(1);
        //             }
        //             if (connect(sock2, (struct sockaddr *)&addr2, sizeof(addr2)) < 0)
        //             {
        //                 perror("[-]Connection error");
        //                 close(sock2);
        //                 exit(1);
        //             }
        //             printf("Connected to the server.\n");
        //             MsgToSS *message2;
        //             message2 = (MsgToSS *)malloc(sizeof(MsgToSS));
        //             strcpy(message2->Command, packet->command);
        //             int newStringLength2 = snprintf(NULL, 0, "$%d/%s", servercom->num, request1);
        //             char *newString2 = (char *)malloc(newStringLength2 + 1);
        //             snprintf(newString2, newStringLength2 + 1, "$%d/%s", servercom->num, request1);
        //             strcpy(message2->path, newString2);
        //             if (strcmp(packet->command, "WRITE") == 0)
        //                 strcpy(message2->To_write, request2);

        //             if (send(sock2, message2, sizeof(MsgToSS), 0) == -1)
        //             {
        //                 perror("Could not send message\n");
        //             }
        //             MsgToCLIENT *ss_response2;
        //             ss_response2 = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
        //             int response_received2 = recv(sock2, ss_response2, sizeof(MsgToCLIENT), 0);
        //             if (response_received2 > 0)
        //             {
        //                 if (ss_response->Ack == 1)
        //                 {
        //                     if (strcmp("WRITE", packet->command) == 0)
        //                         printf("Writing successfully done\n");
        //                     else
        //                     {
        //                         printf("Received response from Storage Server: %s\n", ss_response2->Result);
        //                     }
        //                 }
        //                 else
        //                 {
        //                     if (ss_response2->error_code == ERROR_READ)
        //                     {
        //                         printf("The file could not be opened beacuse it doesnot exist\n");
        //                     }
        //                     else if (ss_response2->error_code == ERROR_WRITE)
        //                     {
        //                         printf("The write command could not executed\n");
        //                     }
        //                     else
        //                     {
        //                         printf("The information regarding the file could not be fetched\n");
        //                     }
        //                 }
        //             }
        //             close(sock2);

        //             // num-2
        //             char *ip3 = "127.0.0.1";
        //             int port3 = servercom->port3;
        //             int sock3;
        //             struct sockaddr_in addr3;
        //             socklen_t addr_size3;
        //             char buffer3[1024];
        //             char choice_buf3[1024], result_buf3[1024];
        //             int n3;
        //             sock3 = socket(AF_INET, SOCK_STREAM, 0);
        //             if (sock3 < 0)
        //             {
        //                 perror("[-]Socket error");
        //                 exit(1);
        //             }
        //             printf("[+]TCP server socket created.\n");
        //             memset(&addr3, '\0', sizeof(addr3));
        //             addr3.sin_family = AF_INET;
        //             addr3.sin_port = port2;
        //             addr3.sin_addr.s_addr = inet_addr(ip3);
        //             if (addr3.sin_addr.s_addr == INADDR_NONE)
        //             {
        //                 perror("[-]Invalid IP address");
        //                 close(sock3);
        //                 exit(1);
        //             }
        //             if (connect(sock3, (struct sockaddr *)&addr3, sizeof(addr3)) < 0)
        //             {
        //                 perror("[-]Connection error");
        //                 close(sock3);
        //                 exit(1);
        //             }
        //             printf("Connected to the server.\n");
        //             MsgToSS *message3;
        //             message3 = (MsgToSS *)malloc(sizeof(MsgToSS));
        //             strcpy(message3->Command, packet->command);
        //             int newStringLength3 = snprintf(NULL, 0, "$%d/%s", servercom->num, request1);
        //             char *newString3 = (char *)malloc(newStringLength3 + 1);
        //             snprintf(newString3, newStringLength3 + 1, "$%d/%s", servercom->num, request1);
        //             strcpy(message3->path, newString3);
        //             if (strcmp(packet->command, "WRITE") == 0)
        //                 strcpy(message3->To_write, request2);

        //             if (send(sock3, message3, sizeof(MsgToSS), 0) == -1)
        //             {
        //                 perror("Could not send message\n");
        //             }
        //             MsgToCLIENT *ss_response3;
        //             ss_response3 = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
        //             int response_received3 = recv(sock3, ss_response3, sizeof(MsgToCLIENT), 0);
        //             if (response_received3 > 0)
        //             {
        //                 if (ss_response->Ack == 1)
        //                 {
        //                     if (strcmp("WRITE", packet->command) == 0)
        //                         printf("Writing successfully done\n");
        //                     else
        //                     {
        //                         printf("Received response from Storage Server: %s\n", ss_response3->Result);
        //                     }
        //                 }
        //                 else
        //                 {
        //                     if (ss_response3->error_code == ERROR_READ)
        //                     {
        //                         printf("The file could not be opened beacuse it doesnot exist\n");
        //                     }
        //                     else if (ss_response3->error_code == ERROR_WRITE)
        //                     {
        //                         printf("The write command could not executed\n");
        //                     }
        //                     else
        //                     {
        //                         printf("The information regarding the file could not be fetched\n");
        //                     }
        //                 }
        //             }
        //             close(sock3);
        //         }
        //     }
        // }
        else
        {
            MsgToCLIENT *response;
            response = (MsgToCLIENT *)malloc(sizeof(MsgToCLIENT));
            int bytes_received = recv(client_socket, response, sizeof(MsgToCLIENT), 0);
            if (bytes_received > 0)
            {
                if (response->Ack == 1)
                {
                    printf("Operation done successfully...\n");
                }
                //    printf("Received response from Naming Server: %s\n", response);
                else
                {
                    if (response->error_code == ERROR_CREATE)
                    {
                        printf("The file could not be created\n");
                    }
                    else if (response->error_code == ERROR_CREATE_D)
                    {
                        printf("The directory could not be created\n");
                    }
                    else if (response->error_code == ERROR_DELETE)
                    {
                        printf("The delete command could not be executed\n");
                    }
                    else if (response->error_code == ERROR_COPY)
                    {
                        printf("Copy could not be done\n");
                    }
                    // printf("The command did not execute successfully\n");
                }
            }
        }
    }
    close(client_socket);

    return 0;
}