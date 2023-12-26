# Network File System (NFS) Implementation

## Introduction
The project involves creating a Network File System (NFS) comprising three main components: Clients, Naming Server (NM), and Storage Servers (SS). Clients interact with the NFS to perform operations such as reading, writing, deleting, and more. The NM acts as a central hub, coordinating communication between clients and storage servers, while Storage Servers manage the physical storage and retrieval of files.
- Problem Statement can be found here [Problem Statement NFS](https://nfs.tiiny.site/)

## How to Run this
- clone this repo.
- Run ```script.sh``` script in that directory.
- Do./nm in the terminal which is in this Dir only
- Do ./ss in D1
- Do ./ss in D2
- Do ./c in this dir only 

### Available Commands
- **CREATE:** Create a file.
- **CREATE_D:** Create a directory.
- **DELETE:** Delete a file/directory.
- **INFO:** Get information about a file.
- **WRITE:** Write to a file.
- **READ:** Read the content of a file.
- **COPY:** Copy a file from one location to another.
- **COPY_D:** Copy a directory from one location to another.

## Specifications

### 1. Naming and Storage Servers
#### 1.1 Initialization
- **Initialize Naming Server (NM):** Set up the central coordination point managing the directory structure and file location information.
  - **Solution:** Define a port for NM in `header.h`; NM listens on that port.

- **Initialize Storage Servers (SS_1 to SS_n):**
  - **SS_1 Initialization:** Provide vital details to NM, including IP address, ports for NM and client connection, and a list of accessible paths.
    - **Solution:** SS takes input for the port number on which it wants to run (`REGISTER WITH NM`); then, SS sends its IP, port, and all directories present in its dir and files to NM.
  - **SS_2 to SS_n Initialization:** Follow the same procedure as SS_1.

- **NM Starts Accepting Client Requests:**
  - **Solution:** Begin accepting client requests once all Storage Servers are initialized and registered. Clients choose an ID, connect to NM, and establish a connection.

#### 1.2 On Storage Servers (SS)
- **Adding new storage servers:**
  - Dynamically add entries to the NM at any point during execution.
    - **Solution:** Use `SELECT` in NM to accept new connections of BOTH SS and CLIENT.

- **Commands Issued by NM:**
  - Create an Empty File/Directory.
    - **Solution:** SS receives the path of the respective File/Dir to be created along with the command **CREATE** or **CREATE_D** and then sends an ACK.
  - Delete a File/Directory.
    - **Solution:** SS receives the path of the respective File/Dir to be deleted along with the command **DELETE** and then sends an ACK.
  - Copy Files.
    - **Solution:** One SS receives the path of the respective File, reads data, sends it to NM; NM sends this data to another SS (if files are not in the same SS); the receiving SS creates a file, puts the data there, and sends an ACK. Command **COPY**
  - Copy Directory
    - **Solution:** One SS receives the path of the respective Dir, sends paths of all files and dirs to NM; NM sends a command to another SS to create that directory structure; NM starts copying files using the same procedure as maintained above. The receiving SS sends an ACK to NM, and NM to the client. Command **COPY_D**

- **Client Interactions:**
  - Read a File.
    - **Solution:** Client asks NM to read a file; NM sends IP and port of the respective SS to the client; the client establishes a connection with SS and sends the command + path to SS of the file; SS sends the data of the file to the client, and SS also sends an ACK to the client.
  - Write to a File.
    - **Solution:** Client asks NM to write to a file; NM sends IP and port of the respective SS to the client; the client establishes a connection with SS and sends the command + path and data to write to SS; SS writes that data in the file and sends an ACK to the client.
  - Get Size and Permissions.
    - **Solution:** Client asks NM to get information about a file; NM sends IP and port of the respective SS to the client; the client establishes a connection with SS and sends the command + path to SS of the file; SS sends the information of the file to the client, and SS also sends an ACK to the client.

#### 1.3 On Naming Server (NM)
- **Storing Storage Server Data:** Serve as the central repository for critical information provided by Storage Servers upon connection.
  - **Solution:** The naming server stores the data of every SS connected to it along with the accessible paths for each SS.
- **Client Task Feedback:** Provide timely and relevant feedback to clients upon completion of tasks.
  - **Solution:** The naming server sends an acknowledgment to the client upon completion of the tasks. The struct contains a special attribute to store which client acknowledgment should be sent.

### 2. Clients
- **Clients communication with the NM:**
    - **Solution:** Clients initiate communication by connecting to NM with a unique client ID. The client is then prompted for various commands it can perform.

- **Functionalities to Implement:**
  - Reading, Writing, and Retrieving Information about Files.
    - **Solution:** The client sends the respective command along with the path. NM searches in the cache first, followed by the trie (in case of cache miss), and sends the respective port and IP to the client. The client connects to the SS and receives an acknowledgment on completion.
  - Creating and Deleting Files and Folders.
    - **Solution:** The client sends the respective command along with the path. NM searches in the cache first, followed by the trie (in case of cache miss), and sends the message to the SS. The SS performs the task, sends an acknowledgment to NM, and NM then sends it to the client.
  - Copying Files/Directories Between Storage Servers.
    - **Solution:** The client sends the respective command along with the path. NM searches in the cache first, followed by the trie (in case of cache miss), and sends the message to the SS, which sends the list of paths for the directory to be copied or the file. NM creates and copies the contents in the other SS. Finally, an acknowledgment is sent to the client.

### 3. Other Features
#### 3.1 Multiple Clients
- **Concurrent Client Access:**
  - Respond to client requests with an initial ACK for concurrent access.
    - **Solution:** Use the `select` system call for multiclient access. The client receives an acknowledgment whenever the task is completed, either successfully or with failure.
  - Handle multiple requests between initial and final ACK from relevant Storage Server.
    - **Solution:** The client can perform multiple operations in the NFS. Multiple clients can connect at the same time, and new clients can connect whenever they arrive.

#### 3.2 Error Codes
- **Error Handling:**
  - Define clear and descriptive error codes for various client request scenarios.
    - **Solution:** Error codes for different operations are defined and sent to the client in case of failure.
    ```c
    #define ERROR_READ 100
    #define ERROR_WRITE 101
    #define ERROR_CREATE 102
    #define ERROR_CREATE_D 103
    #define ERROR_DELETE 104
    #define ERROR_COPY 105
    #define ERROR_INFO 106
    ```

#### 3.3 Search in Naming Servers
- **Efficient Search:**
  - Optimize the search process using efficient data structures (e.g., Tries, Hashmaps).
    - **Solution:** Implemented tries to search the path before any operation. Used functions to perform various operations like InsertPath, DeletePath.
- **LRU Caching:**
  - Implement Least Recently Used (LRU) caching for recent searches.
    - **Solution:** Used a linked list to store the path (20), and this cache remains active during the session only.

#### 3.4 Redundancy/Replication
- **Failure Detection:**
  - Equip NM to detect Storage Server failures.
    - **Solution:** The NM will get a message in the recv function, and the code searches for the server number and displays the message that the SS got disconnected.
- **Data Redundancy and Replication:**
  - Duplicate every file and folder in two other SS.
  - Allow read operations in case of SS failure.
- **SS Recovery:**
  - Match duplicated stores back to the original SS.
  - Prevent new entries during SS recovery.
- **Asynchronous Duplication:**
  - Duplicate write commands asynchronously.

#### 3.5 Bookkeeping
- **Logging and Message Display:**
  - Record every request or acknowledgment.
    - **Solution:** Every ack to the NM is stored in a list along with the command and the path, and this list can be displayed using the **LOG** statement.
  - Display relevant messages for each operation.
    - **Solution:** Messages are displayed in NM, SS for the message received.
- **IP Address and Port Recording:**
  - Include IP addresses and ports in the log for traceability.
    - **Solution:** IP addresses and ports involved in each operation are stored in the Naming Server.
## Assumptions

1. Until the client receives a response (either positive or negative), they cannot send another command.
2. When the client runs the code, it first asks for an ID. After running the code, this ID needs to be assigned to the client. Note that the same ID cannot be given to two clients.
3. Whatever is written to the file is sent by the client only once, at the beginning.
4. Before starting the storage server, create an empty directory, paste the file, and then start the server.
5. The file must have an extension (e.g., a.txt).
6. Do not read/write a file larger than the buffer size defined in header.h.
7. Since TCP is used, assume that the last transaction will be the only one, and an acknowledgment is sent along with the last action.
8. Cannot read an empty file.
9. Run SS in D1->D2->D3->D4....
## Resources
- [1st resource](https://chat.openai.com/share/5c5b5d7b-e497-4f62-8e28-98829f1c1662)
- [2nd resource](https://chat.openai.com/c/ce337eb1-4953-46d9-8756-ba3294fa46fe)
- [3rd resource](https://chat.openai.com/share/f37acea8-6bf3-4e18-9710-301f3f26948c)
- [4th resource](https://chat.openai.com/share/38b9f9d6-2720-4bf4-afdf-92e9372cfcf6)
- [5th resource](https://chat.openai.com/share/6856962d-db21-4361-94a2-b2abb20e3cb5)
- [6th resource](https://chat.openai.com/share/bdfbe495-1542-41ef-90a3-68cc72c48197)
- [7th resource](https://chat.openai.com/share/2ca09c48-5062-49b0-90fb-b953645fe90b)
- [8th resource](https://chat.openai.com/share/c025511c-8229-4cc9-8ad3-912b06bbb75d)
- [9th resource](https://chat.openai.com/share/374bc858-3fe8-49a3-99dc-5cdb6c7e9b33)
## Contributors

- [Harsh Gupta](https://www.linkedin.com/in/harsh-gupta-4ba4a7254) Mainly Contributed In Storage Server
- [Pratham Omkar Pattanayak](https://www.linkedin.com/in/pratham-omkar-pattanayak-34b7b1245/) Mainly Contributed In Naming Server
- [Mayank Mittal](https://www.linkedin.com/in/mayank-mittal-174a00254/) Mainly Contributed In Client, Implementing Tries, LRU caching, Book Keeping, Redudency Part

