#include "animal_shelter_system.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

// default values
static const string URL_DEF("tcp://127.0.0.1:3306");
static const string USER_DEF("root");

// Methods ////////////////////////////////////////////////////////////////////////

AnimalShelterSystem::AnimalShelterSystem()
    :   driver(sql::mysql::get_mysql_driver_instance()),
        con(SetConnection()),
        petBook(con)
{
    Start();
}

Connection* AnimalShelterSystem::SetConnection()
{
    string pass;
    cout << "Please enter password: ";
    cin >> pass;
    return driver->connect(URL_DEF, USER_DEF, pass);
}

Connection* AnimalShelterSystem::SetConnection(const string& password)
{
    return driver->connect(URL_DEF, USER_DEF, password);
}

void AnimalShelterSystem::Start()
{
    DisplayMenu();

    string query;
    while ("q" != query)
    {
        getline(cin, query);

        if ("p" == query)
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

} // namespace ashs