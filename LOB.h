#ifndef LOB_import
#define LOB_import

#include <iostream>
#include <string>
#include <list>
#include <unordered_map>
#include <iterator>
#include <map>


class LOB{
    private: 
    std::unordered_map<double, std::list<orderStruct>> buy;
    std::unordered_map<double, std::list<orderStruct>> sell;
    std::unordered_map<std::string, lookup_node> lookup;


    public:

    void add_order(orderStruct order);
    void destroy_order(std::string code);
    orderStruct get_order(std::string code);
    orderStruct get_first_order(double pricelevel, bool buytrue);
    void edit_order(orderStruct neworder);
    void partial_fill(std::string code, int quantfilled);
    double get_highest_buy();
    double get_lowest_sell();


};

struct orderStruct{
    float price;
    bool buy;
    int quantity;
    std::string timestamp;
    std::string code; 

};

struct lookup_node{
    bool buy;
    float pricelevel;
    std::list<orderStruct>::iterator iter;
};

#endif