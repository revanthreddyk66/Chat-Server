#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

atomic<bool> running{true};

void receive_messages(int client_socket) {
    char buffer[1024];
    while (running) {
        memset(buffer, 0, 1024);
        int bytes_read = recv(client_socket, buffer, 1024, 0);
        if (bytes_read <= 0) {
            cout << "Disconnected from server." << endl;
            running = false;
            break;
        }
        cout << string(buffer, bytes_read) << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <username> <server_ip> <port>" << endl;
        return 1;
    }

    string username = argv[1];
    string ip = argv[2];
    int port = atoi(argv[3]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        cerr << "Failed to create socket" << endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed" << endl;
        return 1;
    }

    // Send username to server once upon connecting
    send(client_socket, username.c_str(), username.length(), 0);

    // Start a thread to listen for incoming messages
    thread receiver(receive_messages, client_socket);

    cout << "Connected! Type your message and press Enter to chat." << endl;

    string line;
    while (running && getline(cin, line)) {
        if (!running) break;
        if (!line.empty()) {
            send(client_socket, line.c_str(), line.length(), 0);
        }
    }

    running = false;
    receiver.join();
    close(client_socket);

    return 0;
}
