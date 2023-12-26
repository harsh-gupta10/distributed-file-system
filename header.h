#ifndef _HEADER_H
#define _HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define NM_IP "127.0.0.1" // Replace with the actual NM server IP
#define NM_PORT 7685 // Replace with the actual NM server port
#define SS_IP "127.0.0.1" // Replace with the actual NM server IP
// #define SS_PORT 8008     // Replace with the actual NM server port
#define PATH_LENGTH 1000
#define COMMAND_LENGTH 100
#define NO_ACCESSIBLE_PATH 100
#define ACCESSIBLE_PATH_LENGTH 500
#define PATH_MAXIMUM 1024
#define MAX_RESULT_SIZE 1024
#define MAX_WRITE_LEN 1024
#define MAX_REQUEST 1000
#define MAX_CLIENT 100
#define MAX_STORAGE 100
#define CACHE_STORE 100
#define FILE_SIZE 50

// ERROR CODES

#define ERROR_READ 100
#define ERROR_WRITE 101
#define ERROR_CREATE 102
#define ERROR_CREATE_D 103
#define ERROR_DELETE 104
#define ERROR_COPY 105
#define ERROR_INFO 106

typedef struct MsgToNM
{
    char From;
    int FromID;
    int copy_dir_flag;
    int name_port;
    int number_access;
    char ip[100];
    int port;
    char data[4096];
    char path[PATH_LENGTH];
    char path1[PATH_LENGTH];
    char path2[PATH_LENGTH];
    char command[COMMAND_LENGTH];
    int ack;
    int error_code;
    struct TrieNode *root;
    int size;
    int if_copy;
    char Path2dArr[NO_ACCESSIBLE_PATH][PATH_LENGTH];
    int noAccessiblePathInitially;
    int rec_copy_dir; // make this 1 while sending the data
} MsgToNM;

/// @brief this is struct whic client will send NM the NM will send SS it has command and path for copy as it has two paths so both are seprated by " " space
typedef struct MsgToSS
{
    char From;
    int copy_dir_flag;
    int FromID;
    char CopyFlag; // ye from ke F to ke liye T and both ke B
    char Command[COMMAND_LENGTH];
    char path[PATH_LENGTH];
    char path1[PATH_LENGTH];
    char path2[PATH_LENGTH];
    char To_write[MAX_WRITE_LEN];
    bool done_Job;
    int num;
} MsgToSS;

typedef struct MsgToCLIENT
{
    char From;
    int Ack;
    int FromID;
    char ip[100];
    int port_SS;
    char Result[MAX_RESULT_SIZE];
    bool done_Job;
    int error_code;
    int num;
    int port2;
    int port3;
    char ip2[100];
    char ip3[100];
    int flag;

} MsgToCLIENT;

extern MsgToNM *ss[MAX_STORAGE];

struct log
{
    char command[10];
    char path1[4096];
    char path2[4096];
    char ip1[100];
    int port1;
    char ip2[100];
    int port2;
    char status[100];
    int ack;
    int index_in_array_of_listed_SS;
    struct log *next;
};

typedef struct log *log;
void addtolog(log head, int index, char *command, char *path1, char *path2, char *ip1, char *ip2, int port1, int port2, char *status, int ACK);
struct node_LRU
{
    char command[10];
    char path1[4096];
    char path2[4096];
    char ip1[100];
    int port1;
    char ip2[100];
    int port2;
    struct node_LRU *next;
};
typedef struct node_LRU *node_LRU;
struct store
{
    char ip1[100];
    int port1;
    char ip2[100];
    int port2;
};
typedef struct store *store;
store search(char *command, char *path1, char *path2, node_LRU *head, store packet);
void addtolistof20(node_LRU **head, char *command, int exist, char *path1, char *path2, char *ip1, char *ip2, int port1, int port2);
#endif