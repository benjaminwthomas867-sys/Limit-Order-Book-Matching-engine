#include "LOB.h"
Receipt produceReceipt(int quantity, double price_level, std::string resting_id, std::string aggressive_id); 

std::list<Receipt> matching_engine(orderStruct new_order,LOB& working_book){
    const int sell = 0;
    const int buy = 1;

    std::list <Receipt> listofreceipts;  // making list of transaction receipts

    if(new_order.buy){ 
        double lowest_sell = working_book.get_lowest_sell();
        while(new_order.price >= lowest_sell && new_order.quantity > 0 && lowest_sell > 0 ){// change so that only need to fetch lowest sell once per loop  
            orderStruct resting_order = working_book.get_sell_first_order(lowest_sell); // Fetching lowest sell order which is most recent
            
            if(resting_order.quantity-new_order.quantity > 0){
                resting_order.quantity -= new_order.quantity;
                working_book.edit_order(resting_order);
                 listofreceipts.push_back(Receipt(resting_order.price,resting_order.code,new_order.code,new_order.quantity));
                 return listofreceipts;
            }else if (resting_order.quantity-new_order.quantity == 0){
                working_book.destroy_sell_order(resting_order.code);
                 listofreceipts.push_back(Receipt(resting_order.price,resting_order.code,new_order.code,new_order.quantity));
                return listofreceipts;
            }else{
                working_book.destroy_sell_order(resting_order.code);
                new_order.quantity-= resting_order.quantity;
                 listofreceipts.push_back(Receipt(resting_order.price,resting_order.code,new_order.code,new_order.quantity));
            }
            lowest_sell = working_book.get_lowest_sell();
        }
        working_book.add_buy_order(new_order);
                return listofreceipts;
    }else if(!new_order.buy){
        double highest_buy = working_book.get_highest_buy();
                while(new_order.price <= highest_buy && new_order.quantity > 0 && highest_buy > 0){// change so that only need to fetch lowest sell once per loop  
            orderStruct resting_order = working_book.get_buy_first_order(highest_buy); // Fetching lowest sell order which is most recent

            if(resting_order.quantity-new_order.quantity > 0){
                resting_order.quantity -= new_order.quantity;
                working_book.edit_order(resting_order);
                 listofreceipts.push_back(Receipt(resting_order.price,resting_order.code,new_order.code,new_order.quantity));
                 return listofreceipts;
            }else if((resting_order.quantity-new_order.quantity) == 0){
                working_book.destroy_buy_order(resting_order.code);
                 listofreceipts.push_back(Receipt(resting_order.price,resting_order.code,new_order.code,new_order.quantity));
                 return listofreceipts;
            }else{
                working_book.destroy_buy_order(resting_order.code);
                new_order.quantity-= resting_order.quantity;
                 listofreceipts.push_back(Receipt(resting_order.price,resting_order.code,new_order.code,new_order.quantity));
            }
            highest_buy = working_book.get_highest_buy();
        }
        working_book.add_sell_order(new_order);
        return listofreceipts;
    }else{return listofreceipts;}
    }








