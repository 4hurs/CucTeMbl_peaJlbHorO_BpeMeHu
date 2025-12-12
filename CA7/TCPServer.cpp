#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <clocale>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
    setlocale(LC_ALL, "RU");

    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    if (WSAStartup(ver, &wsData) != 0) return;

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) return;

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));
    listen(listening, SOMAXCONN);

    cout << "TCP Сервер ждет подключений..." << endl;

    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        cout << host << " подключен на порт " << service << endl;
    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " подключен" << endl;
    }

    closesocket(listening);

    char buf[4096];

    while (true) {
        ZeroMemory(buf, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);

        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            cout << "Клиент отключился" << endl;
            break;
        }

        string msg(buf, 0, bytesReceived);
        cout << "Клиент: " << msg << endl;

        if (msg == "exit") {
            cout << "Получена команда выхода. Завершение работы." << endl;
            break;
        }

        send(clientSocket, buf, bytesReceived + 1, 0);
    }

    closesocket(clientSocket);
    WSACleanup();
}