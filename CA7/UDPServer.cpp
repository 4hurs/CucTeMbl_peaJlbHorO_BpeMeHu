#include <iostream>
#include <WS2tcpip.h>
#include <clocale>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
    setlocale(LC_ALL, "RU");

    WSADATA data;
    WORD version = MAKEWORD(2, 2);
    int wsOk = WSAStartup(version, &data);
    if (wsOk != 0) {
        cout << "Не удалось запустить Winsock!" << wsOk;
        return;
    }

    SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in serverHint;
    serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(54000);

    if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR) {
        cout << "Не удалось привязать сокет!" << endl;
        return;
    }

    sockaddr_in client;
    int clientLength = sizeof(client);
    char buf[1024];

    cout << "UDP Сервер запущен на порту 54000..." << endl;

    while (true) {
        ZeroMemory(&client, clientLength);
        ZeroMemory(buf, 1024);

        int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
        if (bytesIn == SOCKET_ERROR) {
            cout << "Ошибка при получении данных" << endl;
            continue;
        }

        char clientIp[256];
        ZeroMemory(clientIp, 256);
        inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

        cout << "Сообщение от " << clientIp << ": " << buf << endl;
    }

    closesocket(in);
    WSACleanup();
}