#include "LOB.h"

#include <random>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <cstdint>
#include <iostream>
#include <string>
#include <list>
#include "include/oneapi/tbb/concurrent_hash_map.h"
#include "include/oneapi/tbb/blocked_range.h"
#include "include/oneapi/tbb/parallel_for.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

using namespace oneapi::tbb;

concurrent_hash_map<std::string, LOB> Exchange;


std::unordered_map<std::string, Receipt> Receipts;
std::mutex Receipts_mutex;


std::mutex cout_mutex;

struct EquityGate {
    std::mutex m;
    std::condition_variable cv;
    std::uint64_t next_ticket = 0; 
    std::uint64_t now_serving = 0;  
};

std::mutex gates_mutex;
std::unordered_map<std::string, std::unique_ptr<EquityGate>> gates;


class Turn {
    EquityGate* gate;
    std::uint64_t number;
public:
    Turn(EquityGate* g, std::uint64_t n) : gate(g), number(n) {}
    Turn(const Turn&) = delete;
    Turn& operator=(const Turn&) = delete;

    void wait() {
        std::unique_lock<std::mutex> lk(gate->m);
        gate->cv.wait(lk, [&] { return gate->now_serving == number; });
    }

    ~Turn() {
        {
            std::lock_guard<std::mutex> lk(gate->m);
            ++gate->now_serving;
        }
        gate->cv.notify_all();
    }
};


Turn take_ticket(const std::string& equity) {
    std::lock_guard<std::mutex> lk(gates_mutex);
    auto& slot = gates[equity];
    if (!slot) slot = std::make_unique<EquityGate>();
    return Turn(slot.get(), slot->next_ticket++);
}




bool valid_order(orderStruct order) {
    return order.price > 0 && order.quantity > 0 && order.resting == 0;
}


void Trade(orderStruct order1, LOB& working_book) {
    if (valid_order(order1)) {
        matching_engine(order1, working_book);
    }
}


std::string equity_key(const orderStruct& order) {
    return std::string(order.intended_equity,
                       strnlen(order.intended_equity, sizeof(order.intended_equity)));
}


bool process_order(orderStruct incoming_order) {
    if (!valid_order(incoming_order)) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "invalid order\n";
        return false;
    }

    const std::string equity = equity_key(incoming_order);

    Turn turn = take_ticket(equity);

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[ORDER RECEIVED]\n"
                  << "  Equity: " << equity << "\n"
                  << "  Buy/Sell: " << (incoming_order.buy ? "Buy" : "Sell") << "\n"
                  << "  Quantity: " << incoming_order.quantity << "\n"
                  << "  Price: " << incoming_order.price << "\n"
                  << "  Code: " << incoming_order.code << "\n";
    }

    turn.wait();

    std::list<Receipt> receipts;
    {

        concurrent_hash_map<std::string, LOB>::accessor acc;
        Exchange.insert(acc, std::make_pair(equity, LOB(equity)));

        LOB& working_book = acc->second;
        receipts = matching_engine(incoming_order, working_book);
    } 

    if (!receipts.empty()) {
        std::lock_guard<std::mutex> lock(Receipts_mutex);
        for (const Receipt& r : receipts) {
            Receipts.emplace(r.receipt_id, r);
        }
    }
    return true;
} 

void handle_client(SOCKET ClientSocket) {

    char buffer[1024] = {};
    int bytesReceived = recv(ClientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
        double responseValue = -3.0; 

        if (bytesReceived == sizeof(orderStruct)) {
            orderStruct order;
            std::memcpy(&order, buffer, sizeof(orderStruct));

            order.person_id[sizeof(order.person_id) - 1] = '\0';
            order.code[sizeof(order.code) - 1] = '\0';
            order.intended_equity[sizeof(order.intended_equity) - 1] = '\0';

            responseValue = process_order(order) ? 1.0 : -1.0;
        }
        else {
            data_request* req = reinterpret_cast<data_request*>(buffer);

            std::lock_guard<std::mutex> lock(cout_mutex);
            switch (req->function) {
                case 1: {
                    fill_details* details = reinterpret_cast<fill_details*>(buffer);
                    std::cout << "[NEW LOGIN] Username: " << details->new_username << "\n";
                    responseValue = 1.0;
                    break;
                }
                case 2: { 
                    login_request* login = reinterpret_cast<login_request*>(buffer);
                    std::cout << "[LOGIN REQUEST] Username: " << login->username << "\n";
                    responseValue = 1.0;
                    break;
                }
                case 3: { 
                    std::cout << "[BID PRICE REQUEST]\n";
                    responseValue = 150.25;
                    break;
                }
                case 4: { 
                    std::cout << "[ASK PRICE REQUEST]\n";
                    responseValue = 150.50; 
                    break;
                }
                default:
                    std::cout << "[UNKNOWN REQUEST] Function ID: " << req->function << "\n";
                    responseValue = -1.0;
                    break;
            }
        }
        send(ClientSocket, reinterpret_cast<const char*>(&responseValue), sizeof(responseValue), 0);
    }

    closesocket(ClientSocket);
}


int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << "\n";
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serverService.sin_port = htons(8080);

    if (bind(ListenSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 8080...\n";

    while (true) {
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        try {
            std::thread(handle_client, ClientSocket).detach();
        } catch (const std::system_error& e) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cerr << "Failed to create thread: " << e.what() << "\n";
            closesocket(ClientSocket);
        }
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}






