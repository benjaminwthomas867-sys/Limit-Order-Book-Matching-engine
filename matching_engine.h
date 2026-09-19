#ifndef Match_import
#define Match_import

#include "LOB.h"
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <iterator>
#include <chrono>


struct Receipt{
        std::string timestamp;
        double price_level;
        std::string resting_order_id;
        std::string aggresive_order_id;
        int quantity;
};

Receipt produceReceipt(int quantity, double price_level, std::string buy_id, std::string sell_id);
std::string make_timestamp();

std::list<Receipt> matching_engine(orderStruct new_order,LOB& working_book);

#endif