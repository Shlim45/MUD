#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

int main()
{
    // Initialize Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        std::cerr << "Can't initialize winsock!  Quitting..." << std::endl;
        return -1;
    }
   
    // Create a Socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Can't create a socket!  Quitting..." << std::endl;
        return -2;
    }

    // Bind IP/port to socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // could also use inet_pton

    bind(serverSocket, (sockaddr*)&hint, sizeof(hint));
    
    // Tell Winsock the socket is for listening
    listen(serverSocket, SOMAXCONN);

    // Wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET)
    {
        // Do something
        std::cerr << "Client socket invalid!" << std::endl;
    }

    char host[NI_MAXHOST];      // client's remote name
    char service[NI_MAXSERV];   // service (ie port) the client is connected on

    ZeroMemory(host, NI_MAXHOST);   //memset(host, 0, NI_MAXHOST);    // if on linux
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        std::cout << host << " connected on port " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port " <<
            ntohs(client.sin_port) << std::endl;
    }

    // Close listening socket UNTIL implementing multiple clients
    closesocket(serverSocket);

    // While loop:  accept and echo message back to client
    char buf[4096]; // should not limit buffer, read until end

    while (true)
    {
        ZeroMemory(buf, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Error in recv().  Quitting..." << std::endl;
            break;
        }

        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);  // returns size

    }

    // Close the socket
    closesocket(clientSocket);

    // Cleanup Winsock
    WSACleanup();
}
