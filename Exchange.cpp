
#include "LOB.h"

#include <random>
#include <unordered_map>
#include <thread>
#include "include/oneapi/tbb/concurrent_hash_map.h"
#include "include/oneapi/tbb/blocked_range.h"
#include "include/oneapi/tbb/parallel_for.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

using namespace oneapi::tbb;
//Concurrent hashmap
concurrent_hash_map<std::string,LOB> Exchange;
concurrent_hash_map<std::string,LOB>::accessor accessor_Exchange;
std::unordered_map<std::string,orderStruct> Receipts;


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



// 1 for new login  , 2 for login, 3 for get ask price, 4 for get bid price, 5 for get receipts 
    // error codes: -1 for invalid username, -2 for invalid password, -3 for invalid connection, 


bool valid_order(orderStruct order){
    if(order.price > 0 && order.quantity > 0 && order.resting == 0){
        return 1;
    }else{
        return 0;
    }
}


void Trade(orderStruct order1, LOB working_book){

    if(valid_order(order1)){
    matching_engine(order1,working_book);
    }
}

void process_order(orderStruct incoming_order){
    if(valid_order(incoming_order)){
        restart:
        if(Exchange.find(accessor_Exchange, incoming_order.intended_equity)){
        LOB& working_book = accessor_Exchange -> second;
        std::list receipts = matching_engine(incoming_order,working_book);
    
    }else{
        Exchange.insert(std::make_pair(incoming_order.intended_equity, LOB(incoming_order.intended_equity)));
        goto restart;
    }
    }
    else{
        std::cerr << "invalid order";
    }


}












int main(){

WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << "\n";
        return 1;
    }

    // 2. Create Listening Socket
    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // 3. Bind Socket
    sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = inet_addr("127.0.0.1"); // Or INADDR_ANY
    serverService.sin_port = htons(8080); // Matching your client's port

    if (bind(ListenSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 4. Listen for Connections
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 8080...\n";

    // 5. Server Main Loop
    while (true) {
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        // Buffer large enough to hold the largest structure (fill_details or orderStruct)
        char buffer[1024];
        int bytesReceived = recv(ClientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            double responseValue = -3.0; // Default error/failure response

            // Determine if it's an orderStruct or a data_request based on the size
            // (orderStruct size differs from data_request / login_request / fill_details)
            if (bytesReceived == sizeof(orderStruct)) {
                orderStruct* order = reinterpret_cast<orderStruct*>(buffer);
                std::cout << "[ORDER RECEIVED]\n"
                          << "  Equity: " << order->intended_equity << "\n"
                          << "  Buy/Sell: " << (order->buy ? "Buy" : "Sell") << "\n"
                          << "  Quantity: " << order->quantity << "\n"
                          << "  Price: " << order->price << "\n"
                          << "  Code: " << order->code << "\n";
                
                responseValue = 1.0; // Success response for order
            } 
            else {
                // It's a standard data_request (login, create_login, get_bid, get_ask)
                data_request* req = reinterpret_cast<data_request*>(buffer);
                
                switch (req->function) {
                    case 1: { // Create Login
                        fill_details* details = reinterpret_cast<fill_details*>(buffer);
                        std::cout << "[NEW LOGIN] Username: " << details->new_username << "\n";
                        responseValue = 1.0; // Success
                        break;
                    }
                    case 2: { // Login
                        login_request* login = reinterpret_cast<login_request*>(buffer);
                        std::cout << "[LOGIN REQUEST] Username: " << login->username << "\n";
                        responseValue = 1.0; // Success
                        break;
                    }
                    case 3: { // Get Bid Price
                        std::cout << "[BID PRICE REQUEST]\n";
                        responseValue = 150.25; // Dummy bid price example (> 0 means success)
                        break;
                    }
                    case 4: { // Get Ask Price
                        std::cout << "[ASK PRICE REQUEST]\n";
                        responseValue = 150.50; // Dummy ask price example (> 0 means success)
                        break;
                    }
                    default:
                        std::cout << "[UNKNOWN REQUEST] Function ID: " << req->function << "\n";
                        responseValue = -1.0;
                        break;
                }
            }

            // Send response back to the client (client expects a double)
            send(ClientSocket, reinterpret_cast<const char*>(&responseValue), sizeof(responseValue), 0);
        }

        // Close the client connection
        closesocket(ClientSocket);
    }

    // Cleanup (unreachable in this infinite loop example, but good practice)
    closesocket(ListenSocket);
    WSACleanup();



}

    // Socket programming to be written here - receive Order, then call matching engine on it - verify that it is a valid order. and then upload the receipts to the unordered_map for receipts.








