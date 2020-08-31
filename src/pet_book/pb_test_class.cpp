#include "pb_test_class.hpp"
#include <fstream>
#include <iostream>

namespace ashs
{

// cio pointers backup
streambuf *cinbuf = cin.rdbuf();
streambuf *coutbuf = cout.rdbuf();

// Helper functions
static void CheckForErrors(size_t errors, size_t* sumErrors);

///////////////////////////////

void PbTestClass::ExecutInputTest()
{
    instance->isRunning = true;
    instance->ExecuteInput();
}

void PbTestClass::MakeStringTest()
{
    instance->MakeString();
}

size_t PbTestClass::StringFuncsTest()
{
    size_t errors = 0, sumErrors = 0;

    cout << "------ FindByCurrIdTest(): ";
    CheckForErrors(FindByCurrIdTest(), &sumErrors); 

    cout << "------ FindByMaxIdTest(): ";
    CheckForErrors(FindByMaxIdTest(), &sumErrors); 

    cout << "------ GetAllTest(): ";
    CheckForErrors(GetAllTest(), &sumErrors); 

    cout << "------ FindByTest(): ";
    CheckForErrors(FindByTest(), &sumErrors);

    cout << "------ FilterByTest(): ";
    CheckForErrors(FilterByTest(), &sumErrors);

    return sumErrors;
}

size_t PbTestClass::FindByCurrIdTest()
{
    instance->currId = "2";
    return ("SELECT * FROM pets WHERE pet_id=2" != instance->FindByCurrId());
}

size_t PbTestClass::FindByMaxIdTest()
{
    return ("SELECT * FROM pets WHERE pet_id = (SELECT MAX(pet_id) FROM pets)" !=
            instance->FindByMaxId());
}

size_t PbTestClass::GetAllTest()
{
    instance->SetDataTable("pets");
    return ("SELECT * FROM pets ORDER BY pet_id ASC" != instance->GetAll());
}

size_t PbTestClass::FindByTest()
{
    ifstream in("test_input/findbyin.txt");
	ofstream out("test_input/findbyout.txt");

    cin.rdbuf(in.rdbuf());
    cout.rdbuf(out.rdbuf());

    const char* queries[] =
    {
        "SELECT * FROM pets WHERE name LIKE '%Luna%'",  // name Luna
        "SELECT * FROM pets WHERE name=5",              // name 5
        "SELECT * FROM pets WHERE 6=2",                 // 6 2
    };
    
    size_t errors = 0;
    for (size_t i = 0; i < 3; i++)
    {
        errors += (queries[i] != instance->FindBy());
    }

    cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);

    return errors;
}

size_t PbTestClass::FilterByTest()
{
    ifstream in("test_input/filterbyin.txt");
	ofstream out("test_input/filterbyout.txt");

    cin.rdbuf(in.rdbuf());
    cout.rdbuf(out.rdbuf());

    instance->currQuery = instance->FindBy();

    const char* queries[] =
    {
        "SELECT * FROM pets WHERE name LIKE '%Luna%' AND age_months=20",  // name Luna
    };
    
    size_t errors = 0;
    for (size_t i = 0; i < 1; i++)
    {
        errors += (queries[i] != instance->FilterBy());
    }

    cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);

    return errors;
}

////////////////////////////////////////////////////

static void CheckForErrors(size_t errors, size_t* sumErrors)
{
    if (errors)
    {
        cout << errors << " errors";
        ++(*sumErrors);
    }
    else
    {
        cout << "success";
    }
    
    cout << endl;
}

} // namespace ashs