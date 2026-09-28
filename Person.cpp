#include "Person.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <any>
#pragma comment(lib, "ws2_32.lib")

// Structs kept exactly as requested
struct orderStruct {
    std::string person_id;
    float price;
    bool buy;
    int quantity;
    long long timestamp;
    std::string code; 
    bool resting = 0;
    std::string intended_equity;
};

struct data_request {
    int function;
};

struct fill_details : public data_request {
    char new_username[64];
    char hashed_password[256];
    char DOB[32];
    char First_name[32];
    char Second_name[32];
};

struct login_request : public data_request {
    char username[64];
    char password[256];
};

class Client {
private:
    std::string person_id;
    std::string username;
    std::string password;

public:
    const int port = 8080;

    std::string get_person_id() { return person_id; }
    std::string get_username() { return username; }
    std::string get_password() { return password; }

    template <typename T>
    std::any make_socket_data_connection_send(const char* serverIp, int port, const T& details) {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            std::cerr << "WSAStartup failed: " << iResult << "\n";
            return -3.0;
        }

        SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
            WSACleanup();
            return -3.0;
        }

        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        inet_pton(AF_INET, serverIp, &clientService.sin_addr);
        clientService.sin_port = htons(port);

        iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Unable to connect to server.\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return -3.0;
        }

        iResult = send(ConnectSocket, reinterpret_cast<const char*>(&details), sizeof(T), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return -3.0;
        } else {
            std::cout << "Successfully sent " << iResult << " bytes of data.\n";
        }

        double serverResponse = 0;
        int bytesReceived = recv(ConnectSocket, reinterpret_cast<char*>(&serverResponse), sizeof(serverResponse), 0);
        
        if (bytesReceived == 0) {
            std::cerr << "Connection closed by the server.\n";
            serverResponse = -3.0;
        } else if (bytesReceived < 0) {
            std::cerr << "recv failed: " << WSAGetLastError() << "\n";
            serverResponse = -3.0;
        }

        closesocket(ConnectSocket);
        WSACleanup();
        return serverResponse;
    }

    double create_login() {
        bool valid_username = false;
        fill_details new_request{};
        new_request.function = 1;     

        while (!valid_username) {
            std::string new_password;
            std::cout << "Input a username (greater than 8 chars, unique), enter q to cancel: ";
            std::cin >> new_request.new_username;
            
            if (std::string(new_request.new_username) == "q") {
                return -3;
            }

            std::cout << "Input a password: ";
            std::cin >> new_password;
    
            new_password = standard_hash(new_password);
            
            std::strncpy(new_request.hashed_password, new_password.c_str(), sizeof(new_request.hashed_password) - 1);
            new_request.hashed_password[sizeof(new_request.hashed_password) - 1] = '\0';

            std::cout << "Input your DOB in dd/mm/yyyy format: ";
            std::cin >> new_request.DOB;
            std::cout << "Input your first name: ";
            std::cin >> new_request.First_name;
            std::cout << "Input your last name: ";
            std::cin >> new_request.Second_name;

            std::any response = make_socket_data_connection_send("127.0.0.1", port, new_request);
            double valid_result = std::any_cast<double>(response);
            return valid_result;
        }
        return -3;
    }

    double login() {
        login_request new_login{};
        std::cout << "Input your username: ";
        std::cin >> new_login.username;
        std::cout << "Input your password: ";
        std::cin >> new_login.password;
        new_login.function = 2;
        
        std::any response = make_socket_data_connection_send("127.0.0.1", port, new_login);
        return std::any_cast<double>(response);
    }

    double get_bid_price() {
        data_request new_request;
        new_request.function = 3;
        std::any response = make_socket_data_connection_send("127.0.0.1", port, new_request);
        return std::any_cast<double>(response);
    }

    double get_ask_price() {
        data_request new_request;
        new_request.function = 4;
        std::any response = make_socket_data_connection_send("127.0.0.1", port, new_request);
        return std::any_cast<double>(response);
    }

    bool order(const char* serverIp, const orderStruct& order1) {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            std::cerr << "WSAStartup failed: " << iResult << "\n";
            return false;
        }

        SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
            WSACleanup();
            return false;
        }

        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        inet_pton(AF_INET, serverIp, &clientService.sin_addr);
        clientService.sin_port = htons(port);

        iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Unable to connect to server.\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return false;
        }

        iResult = send(ConnectSocket, reinterpret_cast<const char*>(&order1), sizeof(orderStruct), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return false;
        } else {
            std::cout << "Successfully sent " << iResult << " bytes of data.\n";
        }

        double serverResponse = -3;
        int bytesReceived = recv(ConnectSocket, reinterpret_cast<char*>(&serverResponse), sizeof(serverResponse), 0);
        
        if (bytesReceived <= 0) {
            std::cerr << "recv failed or connection closed: " << WSAGetLastError() << "\n";
        }

        closesocket(ConnectSocket);
        WSACleanup();
        return bytesReceived > 0;
    }
};

Client client1;

int main() {
    int run1 = 0;
    do {
        std::cout << "Input your choice:\n"<< "1 for new login\n"<< "2 for login\n"<< "3 for get bid price\n"<< "4 for get ask price\n"<< "5 for get receipts\n"<< "9 to exit\n ";
        std::cin >> run1;

        switch (run1) {
            case 1: {
                double result = client1.create_login();
                if (result == -1) {
                    std::cerr << "Invalid username\n";
                } else if (result == -3) {
                    std::cerr << "Error in connection\n";
                }
                break;
            }
            case 2: {
                double result = client1.login();
                if (result == -1) {
                    std::cerr << "Invalid username\n";
                } else if (result == -2) {
                    std::cerr << "Invalid password\n";
                } else if (result == -3) {
                    std::cerr << "Problem with connection\n";
                }
                break;
            }
            case 3: {
                double result = client1.get_bid_price();
                if (result > 0) {
                    std::cout << "Current bid price is: " << result << "\n";
                } else {
                    std::cerr << "Invalid connection or error code: " << result << "\n";
                }
                break;
            }
            case 4: {
                double result = client1.get_ask_price();
                if (result > 0) {
                    std::cout << "Current ask price is: " << result << "\n";
                } else {
                    std::cerr << "Invalid connection or error code: " << result << "\n";
                }
                break;
            }
            case 9:
                break;
            default:
                std::cout << "Invalid choice, please try again.\n";
                break;
        }
    } while (run1 != 9);
    return 0;
}