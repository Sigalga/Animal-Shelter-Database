#ifndef ASHS_ANIMAL_SHELTER_SYSTEM_HPP
#define ASHS_ANIMAL_SHELTER_SYSTEM_HPP

// Standard C++ includes
#include <string>

// MySQL/C++ Connector includes
#include "mysql_driver.h"			// sql::mysql::MySQL_Driver
#include <cppconn/connection.h>     // sql::Connection

// ASHS includes
#include "pet_book.hpp"
#include "stmt_string_generator.hpp"

namespace ashs
{

class AnimalShelterSystem
{
public:
    AnimalShelterSystem();

private:
    sql::mysql::MySQL_Driver* driver;
    std::shared_ptr<sql::Connection> con;   // connection to an SQL server database
    PetBook petBook;

    // Establishes and returns an SQL database connection
    // using defult parameters and server password
    sql::Connection* SetConnection(const std::string& password);

    // Establishes and returns an SQL database connection
    // using defult parameters and user input server password
    sql::Connection* SetConnection();

    // Starts the interactive ui loop,
    // which may be stopped in response to user input
    void Start();

    // Prints a welcome message and an operation menu
    // for the interactive ui
    void DisplayMenu();
};

} // namespace ashs

#endif // ASHS_ANIMAL_SHELTER_SYSTEM_HPP