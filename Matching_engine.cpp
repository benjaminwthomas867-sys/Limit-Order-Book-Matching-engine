#include "matching_engine.h"

Receipt produceReceipt(int quantity, double price_level, std::string resting_id, std::string aggressive_id); 
std::string make_timestamp();

std::list<Receipt> matching_engine(orderStruct new_order,LOB& working_book){
    const int sell = 0;
    const int buy = 1;
    std::list <Receipt> listofreceipts;  // making list of transaction receipts

    if(new_order.buy){ 
        while(new_order.price >= working_book.get_lowest_sell() && new_order.quantity > 0){// change so that only need to fetch lowest sell once per loop  
            orderStruct resting_order = working_book.get_sell_first_order(working_book.get_lowest_sell()); // Fetching lowest sell order which is most recent
            
            if(resting_order.quantity-new_order.quantity > 0){
                resting_order.quantity -= new_order.quantity;
                working_book.edit_order(resting_order);
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,resting_order.code,new_order.code));
                 return listofreceipts;
            }else if (resting_order.quantity-new_order.quantity == 0){
                working_book.destroy_order(resting_order.code);
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,resting_order.code,new_order.code));
                return listofreceipts;
            }else{
                working_book.destroy_order(resting_order.code);
                new_order.quantity-= resting_order.quantity;
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,resting_order.code,new_order.code));
            }
        }
        working_book.add_order(new_order);
                return listofreceipts;
    }else if(!new_order.buy){
        std::cout <<"hi";
                while(new_order.price <= working_book.get_highest_buy() && new_order.quantity > 0){// change so that only need to fetch lowest sell once per loop  
            orderStruct resting_order = working_book.get_buy_first_order(working_book.get_highest_buy()); // Fetching lowest sell order which is most recent
            
            if(resting_order.quantity-new_order.quantity > 0){
                resting_order.quantity -= new_order.quantity;
                working_book.edit_order(resting_order);
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,resting_order.code,new_order.code));
                 return listofreceipts;
            }else if((resting_order.quantity-new_order.quantity) == 0){
                working_book.destroy_order(resting_order.code);
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,resting_order.code,new_order.code));
                 return listofreceipts;
            }else{
                working_book.destroy_order(resting_order.code);
                new_order.quantity-= resting_order.quantity;
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,resting_order.code,new_order.code));
            }
            
        }
        
        working_book.add_order(new_order);
        return listofreceipts;
    }else{return listofreceipts;}
    }

Receipt produceReceipt(int quantity, double price_level, std::string resting_id, std::string aggressive_id){ // receipt creation function
    Receipt new_receipt;
    new_receipt.quantity = quantity;
    new_receipt.price_level  = price_level;
    new_receipt.resting_order_id = resting_id;
    new_receipt.aggresive_order_id = aggressive_id;
    new_receipt.timestamp = make_timestamp();
    return new_receipt;
}

std::string make_timestamp(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    return std::string(std::ctime(&now_c));
}

    
    










