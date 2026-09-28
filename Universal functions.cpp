# include "LOB.h"
# include "Person.h"
long long make_timestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               now.time_since_epoch()
           ).count();
}

std::string standard_hash(std::string string1){

    size_t hash_value = std::hash<std::string>{}(string1);
    
    return std::format("{}", hash_value);

}