#include <iostream>
#include <WS2tcpip.h>
#include <string>
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

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

    cout << "UDP Клиент готов. Введите сообщение:" << endl;

    string s;
    while (true) {
        cout << "> ";
        getline(cin, s);

        int sendOk = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));
        if (sendOk == SOCKET_ERROR) {
            cout << "Ошибка отправки!" << endl;
        }
    }

    closesocket(out);
    WSACleanup();
}