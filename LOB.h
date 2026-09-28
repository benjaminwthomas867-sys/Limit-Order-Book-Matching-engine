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

struct orderStruct{
    std::string person_id;
    float price;
    bool buy;
    int quantity;
    long long timestamp;
    std::string code; 
    bool resting = 0;
    std::string intended_equity;
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
    float pricelevel;
    std::list<orderStruct>::iterator iter;
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
