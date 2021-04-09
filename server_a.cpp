#include <iostream>
#include <string>
#include <thread>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
void recMsg(SOCKET  ClientSocket) {
    char RxBuffer[128] = {};
    do {
        memset(RxBuffer, 0, sizeof(RxBuffer));
        recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);
        std::cout << "Buddy: " << RxBuffer << std::endl;
        if (strcmp(RxBuffer, "[q]") == 0) {
            std::cout << " Buddy left the chat!" << std::endl;

        }
    } while (strcmp(RxBuffer, "[q]") == 1);
}
void sendMsg(SOCKET ClientSocket) {
    std::string TxBuffer;
    std::cout << "Welcome!" << std::endl;
    while (true) {
        std::cout << "Enter a string ([q]):" << std::endl;
        std::getline(std::cin, TxBuffer);
        send(ClientSocket, TxBuffer.c_str(), TxBuffer.length(), 0);
        if (TxBuffer == "[q]")
            break;
    }
}
int main() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Could not start DLLs" << std::endl;
        return 0;
    }

    SOCKET ListenSocket;
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "Could not create socket" << std::endl;
        WSACleanup();
        return 0;
    }

    struct sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    SvrAddr.sin_port = htons(27000);
    if (bind(ListenSocket, (struct sockaddr*) & SvrAddr,
        sizeof(SvrAddr)) == SOCKET_ERROR) {
        std::cout << "Could not bind socket to address" << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 0;
    }

    if (listen(ListenSocket, 1) == SOCKET_ERROR) {
        std::cout << "Could not start to listen" << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 0;
    }

    std::cout << "Waiting for client connection" << std::endl;

    SOCKET ClientSocket;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "Failed to accept connection" << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 0;
    }

    closesocket(ListenSocket);
    std::cout << "Connection Established" << std::endl;
    std::thread tx, rx;
    rx = std::thread(recMsg, ClientSocket);
    rx.detach();
    tx = std::thread(sendMsg, ClientSocket);
    tx.join();

    WSACleanup();

    return 0;
}