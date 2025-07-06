#include "messages.hpp"

extern "C" 
{
    char* get_iode_message(int val) 
    {
        std::map<int, std::string>::const_iterator it;
        it = iode_msg_map.find(val);
        if (it != iode_msg_map.end()) 
            return const_cast<char*>(it->second.c_str());
        else
            return NULL;
    };
}
