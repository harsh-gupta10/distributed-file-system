#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#define MAX_REQUEST 1000
#define MAX_CLIENT 100
#define MAX_STORAGE 100
#define CACHE_STORE 100


typedef struct node
{
    struct TrieNode *pointer;
    struct node *next;
    char filename[50];
    int isfile;
} node;
typedef struct TrieNode
{
    char name[50];
    struct node *pointtohead;
} TrieNode;

typedef struct Packet_Client
{
    char ip[16];
    int port;
} Packet_Client; // to client from nm


void createTrieNode(char* name,struct TrieNode** root);
int isdir(char *ipart);
void insertPath(struct TrieNode *root, char *path);
// Packet_Client *searchssinindex(char *path, Data_Packet_To_NM *ss, int storage_count, int index);
int TrieNodeExists(struct TrieNode *start, char *string);
int deletePathRecursive(struct TrieNode *node, char **substrings, int num_substrings, struct TrieNode *parentnode, char *path, MsgToNM **ss, int storage_count,int index);
struct TrieNode *GetNextTrieNode(struct TrieNode *start, char *string);
Packet_Client *searchss(char *path, MsgToNM **ss, int storage_count);
void deletePath(struct TrieNode *root, char *path, MsgToNM **ss, int storage_count,int index);