#pragma once


#include "../common.h"

#include <string>
#include <stdexcept>


// using is the C++11 version of typedef
using Comment = std::string;
using Equation = EQ;
using Identity = IDT;
using List = std::string;
using Scalar = SCL;
using Table = TBL;
using Variable = IODE_REAL;


class KDBAbstract
{
protected:
    EnumIodeType type;
    std::string type_name;

protected:
    // Cannot define a KDB *kdb member set to K_WS[type] in the constructor because the pointer contained in 
    // K_WS[type] may change in the course of the program (when loading files for example)
    KDB* getKDB() const
    {
        KDB* kdb = K_WS[type];
        if (kdb == NULL) throw std::runtime_error("There is currently no " + type_name + " in memory.");
        return kdb;
    }

    KOBJ getObject(const int pos) const
    {
        KDB* kdb = getKDB();
        throwPositionException(pos);
        return kdb->k_objs[pos];
    }

    int getPosition(const std::string name) const
    {
        KDB* kdb = getKDB();
        int pos = K_find(kdb, const_cast<char*>(name.c_str()));
        throwPositionException(pos);
        return pos;
    }

    void throwPositionException(const int pos) const
    {
        int nb_objs = count();
        if (pos < 0 || pos > nb_objs) throw std::runtime_error(type_name + " at position " + std::to_string(pos) + " does not exist.");
    }

public:
    KDBAbstract(EnumIodeType type) : type(type), type_name(vIodeTypes[type])
    {
        if (K_WS[type] == NULL) IodeInit();
    }

    int getIODEType() const { return type; }

    int count() const { return K_WS[type] != NULL ? K_WS[type]->k_nb : 0; }

    std::string getName(const int pos) const
    {
        KOBJ obj = getObject(pos);
        std::string name = std::string(convert_oem_to_utf8(obj.o_name));
        return name;
    }

    int setName(const int pos, const std::string new_name)
    {
        std::string old_name = getName(pos);
        int new_pos = rename(old_name, new_name);
        return new_pos;
    }

    int rename(const std::string old_name, const std::string new_name)
    {
        if (new_name.size() > K_MAX_NAME) throw std::runtime_error("Iode names cannot exceed " + std::to_string(K_MAX_NAME) + " characters." + new_name + " : " + std::to_string(new_name.size()));
        KDB* kdb = getKDB();
        char* c_old_name = const_cast<char*>(old_name.c_str());
        char* c_new_name = const_cast<char*>(new_name.c_str());
        int pos = K_ren(kdb, c_old_name, c_new_name);
        // see K_ren documentation
        if (pos < 0)
        {
            std::string msg = type_name + " cannot be renamed as " + new_name + ".";
            if (pos == -1) throw std::runtime_error("Name " + old_name + " does not exist.\n" + msg);
            else if (pos == -2) throw std::runtime_error(type_name + " with name " + new_name + " already exists.\n" + msg);
            else throw std::runtime_error("Something went wrong.\n" + msg);
        }
        return pos;
    }

    void load(std::string filepath)
    {
        char* c_filepath = const_cast<char*>(filepath.c_str());

        int res = B_WsLoad(c_filepath, type);
        if (res != EXIT_SUCCESS)
            throw std::runtime_error("Something went wrong when trying to import " + type_name + " from file " + filepath);
    }

    void save(std::string filepath)
    {
        char* c_filepath = const_cast<char*>(filepath.c_str());

        int res = B_WsSave(c_filepath, type);
        if (res != EXIT_SUCCESS)
            throw std::runtime_error("Something went wrong when trying to save " + type_name + " to file " + filepath);
    }

    void clear()
    {
        int res = B_WsClear("", type);
        if (res != EXIT_SUCCESS)
            throw std::runtime_error("Something went wrong when trying to clear objets of type " + type_name);
    }

    // TODO : overload subscript operator 
    //T operator[](const char* name) { ... }

    // TODO : overload subscript operator 
    //T operator[](const int pos) { ... }
};
