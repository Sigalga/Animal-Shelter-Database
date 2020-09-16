#include "stmt_string_generator.hpp"

using namespace std;

namespace ashs
{

void StmtStringGenerator::AddStringFunc(const Key key, StringFunc func)
{ 
    stringFuncs.insert(pair<Key, StringFunc>(key, func));
}

void StmtStringGenerator::AddStringFunc(const char* cStr, StringFunc func)
{
    const string str(cStr);
    stringFuncs.insert(pair<Key, StringFunc>(str, func));
}

void StmtStringGenerator::RemoveStringFunc(const Key key)
{
    stringFuncs.erase(key);
}

vector<StmtStringGenerator::Key> StmtStringGenerator::GetKeys()
{
    vector<Key> keysVec;
    for (FuncMap::iterator it = stringFuncs.begin();
        it != stringFuncs.end();
        ++it)
    {
        keysVec.push_back(it->first);
    }

    return keysVec;
}

//! Prefer `find` to `at`.
//! https://en.cppreference.com/w/cpp/container/map/find
//! It's a bit more code, but avoiding the exceptions here is considered a best practice.
const string& StmtStringGenerator::GenerateString(const Key key)
{
    try
    {
        return stringFuncs.at(key)();
    }
    catch(const std::out_of_range& e)
    {
        return string("");
    }
}


} // namespace ashs