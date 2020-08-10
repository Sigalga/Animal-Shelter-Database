#ifndef ASHS_ANIMAL_SHELTER_SYSTEM_HPP
#define ASHS_ANIMAL_SHELTER_SYSTEM_HPP

// Standard C++ includes
#include <iostream>
#include <string>

// MySQL/C++ Connector includes
#include "mysql_driver.h"			// sql::mysql::MySQL_Driver
#include "mysql_connection.h"		// sql::mysql::MySQL_Connection
#include <cppconn/connection.h>     // sql::Connection

// ASHS includes
#include "../../include/pet_book.hpp"
#include "../../include/stmt_string_generator.hpp"

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
    void Init();
    void DisplayMenu();
};

AnimalShelterSystem::AnimalShelterSystem()
    :   driver(sql::mysql::get_mysql_driver_instance()),
        con(SetConnection()),
        pbStringGen(new StmtStringGenerator),
        petBook(con, pbStringGen)
{
    Init();
}

// default values
const string urlDef("tcp://127.0.0.1:3306");
const string userDef("root");

Connection* AnimalShelterSystem::SetConnection()
{
    string pass;
    cout << "Please enter password: ";
    cin >> pass;
    return driver->connect(urlDef, userDef, pass);
}

void AnimalShelterSystem::Init()
{
    DisplayMenu();

    string query;
    while (0 != query.compare("q"))
    {
        getline(cin, query);

        if (0 == query.compare("p"))
        {
            petBook.Start();
            DisplayMenu();
        }
    }
}

void AnimalShelterSystem::DisplayMenu()
{
    cout << "\nWelcome to the Animal Shelter System\n"
    << "To exit, press q\n"
    << "To access the PetBook, press p"
    << endl;
}

AnimalShelterSystem::~AnimalShelterSystem()
{
	delete con;
}


} // namespace ashs


#endif // ASHS_ANIMAL_SHELTER_SYSTEM_HPP


