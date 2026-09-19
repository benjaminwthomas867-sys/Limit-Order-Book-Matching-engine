#include "LOB.h"

    double LOB::get_highest_buy(){
        return  buy.rbegin() -> first;
    }
    
    double LOB::get_lowest_sell(){
        return  sell.begin()-> first;
    }

    void LOB::add_order(orderStruct order){ // adds an order to the LOB
        order.resting = 1;
        if(order.buy = 1){
            if(!buy.contains(order.price)){
                buy.emplace(order.price,std::list<orderStruct>{} );
            }
            lookup.emplace(order.code, lookup_node{1,order.price,buy[order.price].insert(buy[order.price].end(),order)});

        }else{
            if(!buy.contains(order.price)){
                buy.emplace(order.price,std::list<orderStruct>{} );
            }
            lookup.emplace(order.code, lookup_node{0,order.price,sell[order.price].insert(sell[order.price].end(),order)});
        }
    }

    void LOB::destroy_order(std::string code){ // destroys an object in the LOB
        if(lookup[code].buy){
            buy[lookup[code].pricelevel].erase(lookup[code].iter);
            if(buy[lookup[code].pricelevel].empty()){
                buy.erase(lookup[code].pricelevel);
            }
        }else{
            sell[lookup[code].pricelevel].erase(lookup[code].iter);
            if(buy[lookup[code].pricelevel].empty()){
                buy.erase(lookup[code].pricelevel);
            }
        }
        lookup.erase(code);
    }

    orderStruct LOB::get_order(std::string code){ // returns an order by its code;
        return *(lookup[code].iter);
    }

    orderStruct LOB::get_sell_first_order(double pricelevel){ // returns the first order at a price level
            return sell[pricelevel].front();  //put in error handling in case the map is empty
        }

    orderStruct LOB::get_buy_first_order(double pricelevel){ // returns the first order at a price level
            return buy[pricelevel].front();
        }

    void LOB::edit_order(orderStruct neworder){ // edits an order - if price changes or volume increases or buy/sell switches, then time priority is reset. If volume just decreases, then time priority is maintained.
        orderStruct oldorder = get_order(neworder.code);
        if(oldorder.price != neworder.price | neworder.quantity > oldorder.quantity | oldorder.buy != neworder.buy){
            destroy_order(oldorder.code);
            add_order(neworder);
        }else{
            (lookup[neworder.code].iter) -> quantity = neworder.quantity;
        }
    }

    void LOB::partial_fill(std::string code, int quantfilled){ // updates an order that has been partially filled.
                (lookup[code].iter) -> quantity -= quantfilled;
    }
