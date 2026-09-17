#include <iostream>
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <iterator>
#include "LOB.h"

//struct for buy,sell orderes
struct orderStruct{
    float price;
    bool buy;
    int quantity;
    std::string timestamp;
    std::string code; 

};
//struct for doubly linked list
struct lookup_node{
    bool buy;
    float pricelevel;
    std::list<orderStruct>::iterator iter;
};
class LOB{
    private: // list because it allows you to delete from index 0 with O(0) - reading is O(n)
    std::map<double, std::list<orderStruct>> buy;
    std::map<double, std::list<orderStruct>> sell;
    std::map<std::string, lookup_node> lookup;

    public:
    double get_highest_buy(){
        return  buy.rbegin() -> first;
    }
    double get_lowest_sell(){
        return  sell.begin()-> first;
    }
    void add_order(orderStruct order){ // adds an order to the LOB
        if(order.buy = 1){
            lookup.emplace(order.code, lookup_node{1,order.price,buy[order.price].insert(buy[order.price].end(),order)});
        }else{
            lookup.emplace(order.code, lookup_node{0,order.price,sell[order.price].insert(sell[order.price].end(),order)});
        }
    }
    void destroy_order(std::string code){ // destroys an object in the LOB
        if(lookup[code].buy){
            buy[lookup[code].pricelevel].erase(lookup[code].iter);
            
        }else{
            sell[lookup[code].pricelevel].erase(lookup[code].iter);
        }
        lookup.erase(code);
    }
    orderStruct get_order(std::string code){ // returns an order by its code;
        return *(lookup[code].iter);
    }
    orderStruct get_first_order(double pricelevel, bool buytrue){ // returns the first order at a price level
        switch(buytrue){
            case 1: 
            return buy[pricelevel].front();
            case 0: 
            return sell[pricelevel].front();
        }
    }
    void edit_order(orderStruct neworder){ // edits an order - if price changes or volume increases or buy/sell switches, then time priority is reset. If volume just decreases, then time priority is maintained.
        orderStruct oldorder = get_order(neworder.code);
        if(oldorder.price != neworder.price | neworder.quantity > oldorder.quantity | oldorder.buy != neworder.buy){
            destroy_order(oldorder.code);
            add_order(neworder);
        }else{
            (lookup[neworder.code].iter) -> quantity = neworder.quantity;
        }
    }
    void partial_fill(std::string code, int quantfilled){ // updates an order that has been partially filled.
                (lookup[code].iter) -> quantity -= quantfilled;
    }
};
