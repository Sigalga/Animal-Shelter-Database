#ifndef ASHS_ANIMAL_SHELTER_SYSTEM_HPP
#define ASHS_ANIMAL_SHELTER_SYSTEM_HPP

// Standard C++ includes
#include <iostream>
#include <string>

// MySQL/C++ Connector includes
#include "mysql_driver.h"			// sql::mysql::MySQL_Driver
#include <cppconn/connection.h>     // sql::Connection

// ASHS includes
#include "pet_book.hpp"
#include "stmt_string_generator.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

class AnimalShelterSystem
{
public:
    AnimalShelterSystem();
    ~AnimalShelterSystem();

private:
    mysql::MySQL_Driver* driver;
    Connection* con;
    StmtStringGenerator* pbStringGen;
    PetBook petBook;

    Connection* SetConnection();
    Connection* SetConnection(const string password);

    void Init();
    void DisplayMenu();
};

} // namespace ashs

#endif // ASHS_ANIMAL_SHELTER_SYSTEM_HPP


