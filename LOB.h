#ifndef LOB_import
#define LOB_import

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <iterator>
#include <chrono>

struct orderStruct{
    float price;
    bool buy;
    int quantity;
    std::string timestamp;
    std::string code; 
    bool resting;
};

struct lookup_node{
    bool buy;
    float pricelevel;
    std::list<orderStruct>::iterator iter;
};

class LOB{
    private: 
    std::map<double, std::list<orderStruct>> buy;
    std::map<double, std::list<orderStruct>> sell;
    std::map<std::string, lookup_node> lookup;

    public:
    void add_order(orderStruct order);
    void destroy_order(std::string code);
    orderStruct get_order(std::string code);
    orderStruct get_sell_first_order(double pricelevel);
    orderStruct get_buy_first_order(double pricelevel);
    void edit_order(orderStruct neworder);
    void partial_fill(std::string code, int quantfilled);
    double get_highest_buy();
    double get_lowest_sell();
};

#endif
