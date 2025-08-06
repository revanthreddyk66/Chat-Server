# Chat-Server
This project is a straightforward and robust multi-threaded chat server built in C++. It is designed to be a clear demonstration of core networking and concurrency concepts, including socket programming, thread management, and the use of mutexes for thread safety.

---

## 🚀 Core Features

* **Multi-threaded Architecture**: Employs a "thread-per-client" model to handle multiple simultaneous connections concurrently, ensuring real-time communication.
* **Thread-Safe Client Management**: Uses a `std::mutex` to protect the shared list of connected clients, preventing race conditions and ensuring data integrity.
* **Socket Programming from Scratch**: Built using the fundamental C++ TCP socket library to demonstrate a solid understanding of network communication.
* **Simple & Clean Design**: Focuses purely on the core task of broadcasting messages, making the codebase clean and easy to understand.

---

## 🛠️ Tech Stack

* **Language**: C++17
* **Concurrency**: `std::thread`, `std::mutex`
* **Networking**: TCP/IP Sockets
* **Build System**: `make`
