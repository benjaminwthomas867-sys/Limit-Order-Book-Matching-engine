#ifndef LOB_import
#define LOB_import

#include <string>

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <iterator>
#include <chrono>
#include "matching_engine.h"

struct orderStruct {
    char person_id[64];
    double price;
    bool buy;
    int quantity;
    long long timestamp;
    char code[64]; 
    bool resting = 0;
    char intended_equity[32];


    orderStruct(char person_id1[64], double price1, bool buy1, int quantity1, char intended_equity1[32]){
        std::strcpy(person_id,person_id1); // convert froms string to char[]
        price = price1;
        buy = buy1;
        quantity = quantity1;
        std::strcpy(intended_equity,intended_equity1);
        timestamp = make_timestamp();
        std::string hash_string = std::format("{}{}{}{}",person_id1,price, buy,quantity,timestamp);
        hash_string = standard_hash(hash_string);
        std::strcpy(code, hash_string.c_str());

    }
};

long long make_timestamp();
std::string standard_hash(std::string string1); // string hash


struct Receipt{
        long long timestamp;
        double price_level;
        std::string resting_order_id;
        std::string aggresive_order_id;
        int quantity;
        std::string receipt_id;

        Receipt(double price_level,std::string resting_order_id, std::string aggressive_order_id,int quantity){
            this -> price_level = price_level;
            this -> resting_order_id = resting_order_id;
            this -> aggresive_order_id = aggresive_order_id;
            this -> quantity = quantity;
            this -> timestamp = make_timestamp();
            this -> receipt_id = standard_hash(std::format("{}{}{}{}{}",price_level,resting_order_id,aggresive_order_id,quantity,this-> timestamp));
        }
};



struct lookup_node{
    bool buy;
    double pricelevel;
    std::list<orderStruct>::iterator iter;
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


class LOB{
    private: 
    std::map<double, std::list<orderStruct>> buy_map;
    std::map<double, std::list<orderStruct>> sell_map;
    std::map<std::string, lookup_node> lookup;
    std::string equity_code;


    public:

    LOB(std::string code_assigned){
        equity_code = code_assigned;
    }
    std::string get_equity_code();


    void add_buy_order(orderStruct order);
    void add_sell_order(orderStruct order);

    void destroy_sell_order(std::string code);
    void destroy_buy_order(std::string code);

    orderStruct get_order(std::string code);
   
    orderStruct get_sell_first_order(double pricelevel);
    orderStruct get_buy_first_order(double pricelevel);
    void edit_order(orderStruct neworder);
    void edit_buy_order(orderStruct neworder);
    void edit_sell_order(orderStruct neworder);

    void partial_fill(std::string code, int quantfilled);
    double get_highest_buy();
    double get_lowest_sell();


};

// misc functions
std::list<Receipt> matching_engine(orderStruct new_order,LOB& working_book);


#endif
