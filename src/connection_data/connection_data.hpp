#ifndef ASHS_CONNECTION_DATA_HPP
#define ASHS_CONNECTION_DATA_HPP

// Standard C++ includes
#include <iostream>
#include <string>

using namespace std;

namespace ashs
{

struct ConnectionData
{
    const string url;
    const string user;
    const string password;
};

// default values
const string urlDef("tcp://127.0.0.1:3306");
const string userDef("root");

} // namespace ashs

#endif // ASHS_CONNECTION_DATA_HPP