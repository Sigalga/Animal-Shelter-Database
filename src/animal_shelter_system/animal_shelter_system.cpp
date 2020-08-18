#include "animal_shelter_system.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

// default values
static const string urlDef("tcp://127.0.0.1:3306");
static const string userDef("root");

AnimalShelterSystem::AnimalShelterSystem()
    :   driver(sql::mysql::get_mysql_driver_instance()),
        con(SetConnection()),
        pbStringGen(new StmtStringGenerator),
        petBook(con, pbStringGen)
{
    Init();
}

Connection* AnimalShelterSystem::SetConnection()
{
    string pass;
    cout << "Please enter password: ";
    cin >> pass;
    return driver->connect(urlDef, userDef, pass);
}

Connection* AnimalShelterSystem::SetConnection(const string& password)
{
    return driver->connect(urlDef, userDef, password);
}

void AnimalShelterSystem::Init()
{
    Start();
}

void AnimalShelterSystem::Start()
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
	delete pbStringGen;
    delete con;
}

} // namespace ashs