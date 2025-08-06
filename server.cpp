
```cpp
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

// The main server class
class ChatServer {
private:
    int server_socket_;
    unordered_map<int, string> clients_; // Map socket FD to username
    mutex clients_mutex_;
    atomic<bool> running_{false};

public:
    ChatServer(int port) {
        server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_ == -1) {
            throw runtime_error("Failed to create socket");
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw runtime_error("Failed to bind socket");
        }

        if (listen(server_socket_, 10) < 0) {
            throw runtime_error("Failed to listen on socket");
        }
    }

    ~ChatServer() {
        running_ = false;
        close(server_socket_);
    }

    void start() {
        running_ = true;
        cout << "Server started on port. Waiting for connections..." << endl;

        while (running_) {
            int client_socket = accept(server_socket_, nullptr, nullptr);
            if (client_socket < 0) {
                cerr << "Failed to accept connection" << endl;
                continue;
            }
            // Create a new thread for each client
            thread(&ChatServer::handle_client, this, client_socket).detach();
        }
    }

private:
    void handle_client(int client_socket) {
        // First, get the username from the client
        char buffer[1024] = {0};
        int bytes_read = recv(client_socket, buffer, 1024, 0);
        if (bytes_read <= 0) {
            close(client_socket);
            return;
        }
        string username(buffer, bytes_read);

        // Add client to our map, protected by the mutex
        {
            lock_guard<mutex> lock(clients_mutex_);
            clients_[client_socket] = username;
        }

        cout << username << " has connected." << endl;
        string join_notification = "[SYSTEM]: " + username + " has joined the chat.";
        broadcast_message(join_notification, client_socket);

        // Loop to receive and broadcast messages
        while (true) {
            memset(buffer, 0, 1024);
            bytes_read = recv(client_socket, buffer, 1024, 0);

            if (bytes_read <= 0) {
                break; // Client disconnected
            }

            string message(buffer, bytes_read);
            string chat_message = "[" + username + "]: " + message;
            broadcast_message(chat_message, client_socket);
        }

        // Cleanup after client disconnects
        string disconnect_notification = "[SYSTEM]: " + username + " has left the chat.";
        cout << username << " has disconnected." << endl;
        {
            lock_guard<mutex> lock(clients_mutex_);
            clients_.erase(client_socket);
        }
        broadcast_message(disconnect_notification, -1); // Broadcast to all remaining clients
        close(client_socket);
    }

    void broadcast_message(const string& message, int sender_socket) {
        lock_guard<mutex> lock(clients_mutex_);
        for (const auto& pair : clients_) {
            if (pair.first != sender_socket) {
                send(pair.first, message.c_str(), message.length(), 0);
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }
    int port = atoi(argv[1]);

    try {
        ChatServer server(port);
        server.start();
    } catch (const runtime_error& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
