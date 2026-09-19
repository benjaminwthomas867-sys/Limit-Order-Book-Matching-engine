# include "matching_engine.h"


LOB working_book;
   // float price;
   // bool buy;
   // int quantity;
   // std::string timestamp;
   // std::string code; 
  //  bool resting;



int main(){
    
    orderStruct new_order1 = {50.5,1,60,make_timestamp(),"dsfsd",0};
    working_book.add_order(new_order1);
    new_order1.buy = 0;
    new_order1.code = "sdklfj";
    new_order1.price = 50;
    new_order1.quantity = 50;
    std::list<Receipt> receipt =  matching_engine(new_order1,working_book);
    std::cout << receipt.front().price_level << '\n' << receipt.front().quantity;
    std::cout << receipt.front().aggresive_order_id;
    

    return 0;
}

