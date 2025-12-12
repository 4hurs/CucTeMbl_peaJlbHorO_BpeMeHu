#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <clocale>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main() {
    setlocale(LC_ALL, "RU");

    string ipAddress = "127.0.0.1";
    int port = 54000;

    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    if (connect(sock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        cout << "Не удалось подключиться к серверу" << endl;
        WSACleanup();
        return;
    }

    cout << "Подключено к TCP серверу! (Напишите 'exit' для выхода)" << endl;

    char buf[4096];
    string userInput;

    do {
        cout << "> ";
        getline(cin, userInput);

        if (userInput.size() > 0) {
            int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);

            if (userInput == "exit") {
                break;
            }

            if (sendResult != SOCKET_ERROR) {
                ZeroMemory(buf, 4096);
                int bytesReceived = recv(sock, buf, 4096, 0);
                if (bytesReceived > 0) {
                    cout << "СЕРВЕР: " << string(buf, 0, bytesReceived) << endl;
                }
            }
        }

    } while (userInput.size() > 0);

    closesocket(sock);
    WSACleanup();
}