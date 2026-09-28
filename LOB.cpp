#include "LOB.h"

    double LOB::get_highest_buy(){
        if(! buy_map.empty()){
        return  buy_map.rbegin() -> first;}
        else{
            return -1;
        }
    }
    
    double LOB::get_lowest_sell(){
        if(!sell_map.empty()){
        return  sell_map.begin()-> first;}
        else{
            return -1;
        }
    }

    std::string LOB::get_equity_code(){
        return equity_code;
    }


    void LOB::add_buy_order(orderStruct order){
        if(!buy_map.contains(order.price)){
                buy_map.emplace(order.price,std::list<orderStruct>{} );
            }
            lookup.emplace(order.code, lookup_node{1,order.price,buy_map[order.price].insert(buy_map[order.price].end(),order)});


    }

    void LOB:: add_sell_order(orderStruct order){
        if(!buy_map.contains(order.price)){
                buy_map.emplace(order.price,std::list<orderStruct>{} );
            }
            lookup.emplace(order.code, lookup_node{0,order.price,sell_map[order.price].insert(sell_map[order.price].end(),order)});

    }


    void LOB::destroy_buy_order(std::string code){  // destroys buy order
                    buy_map[lookup[code].pricelevel].erase(lookup[code].iter);
            if(buy_map[lookup[code].pricelevel].empty()){
                buy_map.erase(lookup[code].pricelevel);
            }
    }

    void LOB::destroy_sell_order(std::string code){ // destroys sell order
                    sell_map[lookup[code].pricelevel].erase(lookup[code].iter);
            if(buy_map[lookup[code].pricelevel].empty()){
                buy_map.erase(lookup[code].pricelevel);
            }
    }

    orderStruct LOB::get_order(std::string code){ // returns an order by its code;
        return *(lookup[code].iter);
    }

    orderStruct LOB::get_sell_first_order(double pricelevel){ // returns the first order at a price level
            return sell_map[pricelevel].front();  //put in error handling in case the map is empty
        }

    orderStruct LOB::get_buy_first_order(double pricelevel){ // returns the first order at a price level
            return buy_map[pricelevel].front();
        }

    void LOB::edit_buy_order(orderStruct neworder){ // edits an order - if price changes or volume increases or buy/sell switches, then time priority is reset. If volume just decreases, then time priority is maintained.
        orderStruct oldorder = get_order(neworder.code);
        if(oldorder.price != neworder.price | neworder.quantity > oldorder.quantity | oldorder.buy != neworder.buy){
            destroy_buy_order(oldorder.code);
            add_buy_order(neworder);
        }else{
            (lookup[neworder.code].iter) -> quantity = neworder.quantity;
        }
    }

    void LOB::edit_sell_order(orderStruct neworder){
                orderStruct oldorder = get_order(neworder.code);
        if(oldorder.price != neworder.price | neworder.quantity > oldorder.quantity | oldorder.buy != neworder.buy){
            destroy_sell_order(oldorder.code);
            add_sell_order(neworder);
        }else{
            (lookup[neworder.code].iter) -> quantity = neworder.quantity;
        }
    }

    void LOB::partial_fill(std::string code, int quantfilled){ // updates an order that has been partially filled.
                (lookup[code].iter) -> quantity -= quantfilled;
    }



