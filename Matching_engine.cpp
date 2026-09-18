#include "LOB.h"
LOB working_book;

struct Receipt{
        std::string timestamp;
        double price_level;
        bool buy;
};
struct transaction_Receipt: public Receipt{
    int quantity_exchanged;

};

Receipt produceReceipt(int quantity, double price_level, std::string buy_id, std::string sell_id);

std::list<Receipt> matching_engine(orderStruct new_order){
    const int sell = 0;
    const int buy = 1;
    std::list <Receipt> listofreceipts;
    if(new_order.buy){ 
        while(new_order.price >= working_book.get_lowest_sell() & new_order.quantity > 0){// change so that only need to fetch lowest sell once per loop  
            orderStruct resting_order = working_book.get_sell_first_order(working_book.get_lowest_sell()); // Fetching lowest sell order which is most recent
            
            if(resting_order.quantity-new_order.quantity > 0){
                resting_order.quantity -= new_order.quantity;
                working_book.edit_order(resting_order);
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,new_order.code,resting_order.code));
                 return listofreceipts;
            }else if (resting_order.quantity-new_order.quantity == 0){
                working_book.destroy_order(resting_order.code);
                listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,new_order.code,resting_order.code));
                return listofreceipts;
            }else{
                working_book.destroy_order(resting_order.code);
                new_order.quantity-= resting_order.quantity;
            }
        }
        working_book.add_order(new_order);
                return listofreceipts;
    }else if(!new_order.buy){
                while(new_order.price <= working_book.get_highest_buy() & new_order.quantity > 0){// change so that only need to fetch lowest sell once per loop  
            orderStruct resting_order = working_book.get_buy_first_order(working_book.get_highest_buy()); // Fetching lowest sell order which is most recent
            
            if(resting_order.quantity-new_order.quantity > 0){
                resting_order.quantity -= new_order.quantity;
                working_book.edit_order(resting_order);
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,new_order.code,resting_order.code));
                 return listofreceipts;
            }else if((resting_order.quantity-new_order.quantity) == 0){
                working_book.destroy_order(resting_order.code);
                 listofreceipts.push_back(produceReceipt(new_order.quantity,resting_order.price,new_order.code,resting_order.code));
                 return listofreceipts;
            }else{
                working_book.destroy_order(resting_order.code);
                new_order.quantity-= resting_order.quantity;
                listofreceipts.push_back(produceReceipt(resting_order.quantity,resting_order.price,new_order.code,resting_order.code));
            }
            
        }
        
        working_book.add_order(new_order);
        return listofreceipts;
    }else{return listofreceipts;}
    }





    
    










