#include "pb_test_class.hpp"
#include <fstream>
#include <iostream>

namespace ashs
{

// cio pointers backup
streambuf *cinbuf = cin.rdbuf();
streambuf *coutbuf = cout.rdbuf();

// io test file paths
const string FIND_BY_IN("test_input/findbyin.txt");
const string FIND_BY_OUT("test_input/findbyout.txt");
const string FILTER_BY_IN("test_input/filterbyin.txt");
const string FILTER_BY_OUT("test_input/filterbyout.txt");
const string ORDER_BY_IN("test_input/orderbyin.txt");
const string ORDER_BY_OUT("test_input/orderbyout.txt");
const string UPDATE_IN("test_input/updatein.txt");
const string UPDATE_OUT("test_input/updateout.txt");
const string ADD_IN("test_input/addin.txt");
const string ADD_OUT("test_input/addout.txt");
const string JOINED_IN("test_input/findjoinedin.txt");
const string JOINED_OUT("test_input/findjoinedout.txt");
const string REMOVE_IN("test_input/removein.txt");
const string REMOVE_OUT("test_input/removeout.txt");
const string CHOOSE_IN("test_input/choosein.txt");
const string CHOOSE_OUT("test_input/chooseout.txt");

// Helper functions
static void CheckForErrors(size_t errors, size_t* sumErrors);
static void RedirectToFile(ifstream* in, ofstream* out);
static void RestartToCio();

// expected return values
const string findByQueries[] =
{
    "SELECT * FROM pets WHERE name LIKE '%Luna%'",  // input: name Luna
    "SELECT * FROM pets WHERE name=5",              // input: name 5
    "SELECT * FROM pets WHERE 6=2",                 // input: 6 2
};

const string filterByQueries[] =
{
    // input: name Luna age_months 20
    "SELECT * FROM pets WHERE name LIKE '%Luna%' AND age_months=20",

    // input: status adopted name Luna
    "SELECT * FROM pets WHERE status LIKE '%adopted%' AND name LIKE '%Luna%'"
};

const string orderByQueries[] =
{
    // input: name Luna age_months 20
    "SELECT * FROM pets WHERE status LIKE '%adopted%' ORDER BY age_months DESC"
};

const string updateQueries[] =
{
    // input: 3 status inshelter
    "UPDATE pets SET status='inshelter' WHERE pet_id=3;",

    // input: f 6 0
    "UPDATE pets SET 6='0' WHERE pet_id=f;"
};

const string addQueries[] =
{
    // input: 100 Shushu 5 inshelter
    "INSERT INTO pets (adopter_id, name, age_months, status) VALUES (100, 'Shushu', 5, 'inshelter')",
    
    // input: 0 0 0 0
    "INSERT INTO pets (adopter_id, name, age_months, status) VALUES (NULL, NULL, NULL, NULL)",
    
    // input: abc 123 xyz 456
    "INSERT INTO pets (adopter_id, name, age_months, status) VALUES ('abc', 123, 'xyz', 456)"
};

const string chooseQueries[] =
{
    // input: 2 1
    "SELECT * FROM pets WHERE pet_id=2",

    // input: 2 2
    "SELECT * FROM adopters WHERE adopter_id=2",

    // input: 2 3 age_months 60
    "UPDATE pets SET age_months='60' WHERE pet_id=2;",

    // input: 2 4
    "DELETE FROM pets WHERE pet_id=2;"
};
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

    cout << "------ ExitTest(): ";
    CheckForErrors(ExitTest(), &sumErrors);

    cout << "------ ClearSearchTest(): ";
    CheckForErrors(ClearSearchTest(), &sumErrors);

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

    cout << "------ OrderByTest(): ";
    CheckForErrors(OrderByTest(), &sumErrors);

    cout << "------ UpdateFieldTest(): ";
    CheckForErrors(UpdateFieldTest(), &sumErrors);

    cout << "------ AddEntryTest(): ";
    CheckForErrors(AddEntryTest(), &sumErrors);

    cout << "------ RemoveEntryTest(): ";
    CheckForErrors(RemoveEntryTest(), &sumErrors);

    cout << "------ FindJoinedTest(): ";
    CheckForErrors(FindJoinedTest(), &sumErrors);

    cout << "------ ChooseEntryTest(): ";
    CheckForErrors(ChooseEntryTest(), &sumErrors);
    
    return sumErrors;
}

// StringFuncs /////////////////////////////////////////
size_t PbTestClass::ExitTest()
{
    size_t errors = 0;

    instance->Exit();
    errors += (false != instance->isRunning);
    
    return errors;
}

size_t PbTestClass::ClearSearchTest()
{
    instance->currId = "test";
    instance->currQuery = "test";

    size_t errors = 0;
    errors += ("" != instance->ClearSearch());
    errors += ("" != instance->currId);
    errors += ("" != instance->currQuery);

    return errors;
}

// Initial queries
size_t PbTestClass::FindByTest()
{
    ifstream in(FIND_BY_IN);
	ofstream out(FIND_BY_OUT);
    RedirectToFile(&in, &out);

    // check return values
    size_t errors = 0;
    for (size_t i = 0; i < sizeof(findByQueries)/sizeof(string); i++)
    {
        errors += (findByQueries[i] != instance->FindBy());
    }

    RestartToCio();

    return errors;
}

size_t PbTestClass::GetAllTest()
{
    instance->SetDataTable("pets");
    return ("SELECT * FROM pets ORDER BY pet_id ASC" != instance->GetAll());
}

// Secondary queries
size_t PbTestClass::FilterByTest()
{
    ifstream in(FILTER_BY_IN);
	ofstream out(FILTER_BY_OUT);
    RedirectToFile(&in, &out);
    
    // check return values
    size_t errors = 0;
    for (size_t i = 0; i < sizeof(filterByQueries)/sizeof(string); i++)
    {
        instance->currQuery = instance->FindBy();
        errors += (filterByQueries[i] != instance->FilterBy());
        instance->currQuery = "";
    }

    RestartToCio();

    return errors;
}

size_t PbTestClass::OrderByTest()
{
    ifstream in(ORDER_BY_IN);
	ofstream out(ORDER_BY_OUT);
    RedirectToFile(&in, &out);
    
    // check return values
    size_t errors = 0;
    for (size_t i = 0; i < sizeof(orderByQueries)/sizeof(string); i++)
    {
        instance->currQuery = instance->FindBy();
        errors += (orderByQueries[i] != instance->OrderBy());
        instance->currQuery = "";
    }

    RestartToCio();

    return errors;
}

size_t PbTestClass::ChooseEntryTest()
{
    ifstream in(CHOOSE_IN);
	ofstream out(CHOOSE_OUT);
    RedirectToFile(&in, &out);

    instance->SetDataTable("pets");
    instance->currQuery = "";

    // check return values
    size_t errors = 0;
    for (size_t i = 0; i < sizeof(chooseQueries)/sizeof(string); i++)
    {
        instance->SetDataTable("pets");
        errors += (chooseQueries[i] != instance->ChooseEntry());
    }

    RestartToCio();

    return errors;
}

size_t PbTestClass::FindJoinedTest()
{
    ifstream in(JOINED_IN);
	ofstream out(JOINED_OUT);
    RedirectToFile(&in, &out);

    size_t errors = 0;

    // correct parameters fed to instance:
    // choose a pet and find its joined adopter
    instance->SetDataTable("pets");
    instance->currQuery = "choose";
    instance->currId = "3";
    errors += ("SELECT * FROM adopters WHERE adopter_id=2" != instance->FindJoined());
    errors += instance->currId != "query";
    
    // choose an adopter and find its joined pet/s
    instance->SetDataTable("adopters");
    instance->currQuery = "choose";
    instance->currId = "2";
    errors += ("SELECT * FROM pets WHERE adopter_id=2" != instance->FindJoined());
    errors += instance->currId != "query";
    
    // perameter from user input: 2
    instance->SetDataTable("adopters");
    instance->currQuery = "";
    instance->currId = "";
    errors += ("SELECT * FROM pets WHERE adopter_id=2" != instance->FindJoined());
    errors += instance->currId != "query";

    RestartToCio();

    return errors;
}

// Editorial operations
size_t PbTestClass::UpdateFieldTest()
{
    ifstream in(UPDATE_IN);
	ofstream out(UPDATE_OUT);
    RedirectToFile(&in, &out);
    
    // check return values
    size_t errors = 0;
    for (size_t i = 0; i < sizeof(updateQueries)/sizeof(string); i++)
    {
        errors += (updateQueries[i] != instance->UpdateField());
    }

    RestartToCio();

    return errors;
}

size_t PbTestClass::AddEntryTest()
{
    ifstream in(ADD_IN);
	ofstream out(ADD_OUT);
    RedirectToFile(&in, &out);
    
    // check return values
    size_t errors = 0;
    for (size_t i = 0; i < sizeof(addQueries)/sizeof(string); i++)
    {
        errors += (addQueries[i] != instance->AddEntry());
    }

    RestartToCio();

    return errors;
}

size_t PbTestClass::RemoveEntryTest()
{
    ifstream in(REMOVE_IN);
	ofstream out(REMOVE_OUT);
    RedirectToFile(&in, &out);

    size_t errors = 0;

    // correct parameters fed to instance
    instance->SetDataTable("pets");
    instance->currQuery = "choose";
    instance->currId = "100";
    errors += ("DELETE FROM pets WHERE pet_id=100;" != instance->RemoveEntry());

    // incorrect parameters fed to instance
    instance->currQuery = "choose";
    instance->currId = "";
    errors += ("DELETE FROM pets WHERE pet_id=;" != instance->RemoveEntry());

    // user input: 101
    instance->currQuery = "";
    instance->currId = "";
    errors += ("DELETE FROM pets WHERE pet_id=101;" != instance->RemoveEntry());

    RestartToCio();

    return errors;
}

// Helper operations
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

static void RedirectToFile(ifstream* in, ofstream* out)
{
    cin.rdbuf(in->rdbuf());
    cout.rdbuf(out->rdbuf());
}

static void RestartToCio()
{
    cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);
}

} // namespace ashs  