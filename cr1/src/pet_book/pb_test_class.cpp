// Standard C++ includes
#include <iostream>
#include <vector>
#include <fstream>

#include "pb_test_class.hpp"

using namespace std;

namespace ashs
{

// cio pointers backup
streambuf *cinbuf = cin.rdbuf();
streambuf *coutbuf = cout.rdbuf();

// io test file paths
const string START_IN("test_io/startin.txt");
const string START_OUT("test_io/startout.txt");
const string EXEC_INP_IN("test_io/execinpin.txt");
const string EXEC_INP_OUT("test_io/execinpout.txt");
const string MAKE_STR_IN("test_io/makestrin.txt");
const string MAKE_STR_OUT("test_io/makestrout.txt");
const string FIND_BY_IN("test_io/findbyin.txt");
const string FIND_BY_OUT("test_io/findbyout.txt");
const string FILTER_BY_IN("test_io/filterbyin.txt");
const string FILTER_BY_OUT("test_io/filterbyout.txt");
const string ORDER_BY_IN("test_io/orderbyin.txt");
const string ORDER_BY_OUT("test_io/orderbyout.txt");
const string UPDATE_IN("test_io/updatein.txt");
const string UPDATE_OUT("test_io/updateout.txt");
const string ADD_IN("test_io/addin.txt");
const string ADD_OUT("test_io/addout.txt");
const string JOINED_IN("test_io/findjoinedin.txt");
const string JOINED_OUT("test_io/findjoinedout.txt");
const string REMOVE_IN("test_io/removein.txt");
const string REMOVE_OUT("test_io/removeout.txt");
const string CHOOSE_IN("test_io/choosein.txt");
const string CHOOSE_OUT("test_io/chooseout.txt");

// Helper functions
static void CheckForErrors(size_t errors, size_t* sumErrors);
static void RedirectToFile(ifstream* in, ofstream* out);
static void ResetToCio();

// expected return values
static const vector<string> findByQueries =
{
    "SELECT * FROM pets WHERE name LIKE '%Luna%'",  // input: name Luna
    "SELECT * FROM pets WHERE name=5",              // input: name 5
    "SELECT * FROM pets WHERE 6=2",                 // input: 6 2
};

static const vector<string> filterByQueries =
{
    // input: name Luna age_months 20
    "SELECT * FROM pets WHERE name LIKE '%Luna%' AND age_months=20",

    // input: status adopted name Luna
    "SELECT * FROM pets WHERE status LIKE '%adopted%' AND name LIKE '%Luna%'"
};

static const vector<string> orderByQueries =
{
    // input: name Luna age_months 20
    "SELECT * FROM pets WHERE status LIKE '%adopted%' ORDER BY age_months DESC"
};

static const vector<string> updateQueries =
{
    // input: 3 status inshelter
    "UPDATE pets SET status='inshelter' WHERE pet_id=3;",

    // input: f 6 0
    "UPDATE pets SET 6='0' WHERE pet_id=f;"
};

static const vector<string> addQueries =
{
    // input: 100 Shushu 5 inshelter
    "INSERT INTO pets (adopter_id, name, age_months, status) VALUES (100, 'Shushu', 5, 'inshelter')",
    
    // input: 0 0 0 0
    "INSERT INTO pets (adopter_id, name, age_months, status) VALUES (NULL, NULL, NULL, NULL)",
    
    // input: abc 123 xyz 456
    "INSERT INTO pets (adopter_id, name, age_months, status) VALUES ('abc', 123, 'xyz', 456)"
};

static const vector<string> chooseQueries =
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

// const size_t N_GETRULE_TESTS = 5;
// static const string getRuleParams[N_GETRULE_TESTS][4] =

static const vector<vector<string> > getRuleParams =
{
    { "number", "2", "", "number=2" },
    { "number", "2", "3", "number BETWEEN 2 AND 3" },
    { "text", "hello", "", "text LIKE '%hello%'" },
    { "text", "hello", "world", "text LIKE '%hello%'" },
    { "textnum", "0678abc", "", "textnum LIKE '%0678abc%'" },
};
// Public Method Tests ////////////////////////////////

void PbTestClass::StartTest()
{
    cout << "-- StartTest(): " << endl;

    ifstream in(START_IN);
	ofstream out(START_OUT);
    RedirectToFile(&in, &out);

	instance->Start();

	ResetToCio();
}

// Private Method Tests ////////////////////////////////
void PbTestClass::PrivateMethodsTest()
{
    cout << "-- PrivateMethodsTest():" << endl;

	size_t sumErrors = 0;

	cout << "---- ExecutInputTest(): " << endl;
	CheckForErrors(ExecutInputTest(), &sumErrors);

	cout << "---- MakeStringTest(): " << endl;
	CheckForErrors(MakeStringTest(), &sumErrors);

	cout << "---- StringFuncsTest(): " << endl;
    CheckForErrors(StringFuncsTest(), &sumErrors);
}

size_t PbTestClass::ExecutInputTest()
{
    ifstream in(EXEC_INP_IN);
	ofstream out(EXEC_INP_OUT);
    RedirectToFile(&in, &out);

    size_t errors = 0;

    // input: 1 exit
    instance->isRunning = true;
    instance->currQuery = "";
    instance->ExecuteInput();
    errors += ("exit" != instance->currQuery);

    // input: 2 exit
    instance->isRunning = true;
    instance->currQuery = "";
    instance->ExecuteInput();
    errors += ("exit" != instance->currQuery);

    // input:3
    instance->isRunning = true;
    instance->currQuery = "";
    instance->ExecuteInput();
    errors += ("exit" != instance->currQuery);

    ResetToCio();

    return errors;
}

size_t PbTestClass::MakeStringTest()
{
    ifstream in(MAKE_STR_IN);
	ofstream out(MAKE_STR_OUT);
    RedirectToFile(&in, &out);

    size_t errors = 0;

    // input: 1 exit
    instance->MakeString();
    errors += (instance->currQuery != instance->Exit());

    // new search, input: 1 show_all
    instance->currId = "";
    instance->MakeString();
    errors += (instance->currQuery != instance->GetAll());

    // TO DO:
    // - find
    // - update
    // - add_new
    // - delete
    // - choose
    // - clear

    ResetToCio();

    return errors;
}

size_t PbTestClass::StringFuncsTest()
{
    size_t sumErrors = 0;

    cout << "------ ExitTest(): ";
    CheckForErrors(ExitTest(), &sumErrors);

    cout << "------ ClearSearchTest(): ";
    CheckForErrors(ClearSearchTest(), &sumErrors);

    // Initial queries

    cout << "------ FindByTest(): ";
    CheckForErrors(FindByTest(), &sumErrors);

    cout << "------ GetAllTest(): ";
    CheckForErrors(GetAllTest(), &sumErrors); 

    // Secondary queries

    cout << "------ FilterByTest(): ";
    CheckForErrors(FilterByTest(), &sumErrors);

    cout << "------ OrderByTest(): ";
    CheckForErrors(OrderByTest(), &sumErrors);

    cout << "------ ChooseEntryTest(): ";
    CheckForErrors(ChooseEntryTest(), &sumErrors);

    cout << "------ FindJoinedTest(): ";
    CheckForErrors(FindJoinedTest(), &sumErrors);
    
    // Editorial operations

    cout << "------ UpdateFieldTest(): ";
    CheckForErrors(UpdateFieldTest(), &sumErrors);

    cout << "------ AddEntryTest(): ";
    CheckForErrors(AddEntryTest(), &sumErrors);

    cout << "------ RemoveEntryTest(): ";
    CheckForErrors(RemoveEntryTest(), &sumErrors);

    // Helper operations

    cout << "------ FindByCurrIdTest(): ";
    CheckForErrors(FindByCurrIdTest(), &sumErrors); 

    cout << "------ FindByMaxIdTest(): ";
    CheckForErrors(FindByMaxIdTest(), &sumErrors); 

    cout << "------ SelectDataTest(): ";
    CheckForErrors(SelectDataTest(), &sumErrors); 

    cout << "------ SelectDataWhereTest(): ";
    CheckForErrors(SelectDataWhereTest(), &sumErrors); 

    cout << "------ SelectDataAscTest(): ";
    CheckForErrors(SelectDataAscTest(), &sumErrors); 

    cout << "------ GetRuleTest(): ";
    CheckForErrors(GetRuleTest(), &sumErrors); 

    cout << "---- ";

    return sumErrors;
}

// StringFuncs Tests ///////////////////////////////////
size_t PbTestClass::ExitTest()
{
    size_t errors = 0;

    instance->Exit();
    errors += (false != instance->isRunning);
    errors += ("exit" != instance->currQuery);
    
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
    for (auto query : findByQueries)
    {
        errors += (query != instance->FindBy());
    }

    ResetToCio();

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
    for (auto query : filterByQueries)
    {
        instance->currQuery = instance->FindBy();
        errors += (query != instance->FilterBy());
        instance->currQuery = ""; 
    }

    ResetToCio();

    return errors;
}

size_t PbTestClass::OrderByTest()
{
    ifstream in(ORDER_BY_IN);
	ofstream out(ORDER_BY_OUT);
    RedirectToFile(&in, &out);
    
    // check return values
    size_t errors = 0;
    for (auto query : orderByQueries)
    {
        instance->currQuery = instance->FindBy();
        errors += (query != instance->OrderBy());
        instance->currQuery = "";
    }

    ResetToCio();

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
    for (auto query : chooseQueries)
    {
        instance->SetDataTable("pets");
        errors += (query != instance->ChooseEntry());
    }

    ResetToCio();

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

    ResetToCio();

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
    for (auto query : updateQueries)
    {
        errors += (query != instance->UpdateField());
    }

    ResetToCio();

    return errors;
}

size_t PbTestClass::AddEntryTest()
{
    ifstream in(ADD_IN);
	ofstream out(ADD_OUT);
    RedirectToFile(&in, &out);
    
    // check return values
    size_t errors = 0;
    for (auto query : addQueries)
    {
        errors += (query != instance->AddEntry());
    }

    ResetToCio();

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

    ResetToCio();

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

size_t PbTestClass::SelectDataTest()
{
    instance->currTable = "pets";
    return ("SELECT * FROM pets" != instance->SelectData());
}

size_t PbTestClass::SelectDataWhereTest()
{
    instance->currTable = "pets";
    return ("SELECT * FROM pets WHERE " !=
            instance->SelectDataWhere());
}

size_t PbTestClass::SelectDataAscTest()
{
    instance->currTable = "pets";
    return ("SELECT * FROM pets ORDER BY pet_id ASC" !=
            instance->SelectDataAsc());
}

size_t PbTestClass::GetRuleTest()
{
    size_t errors = 0;
    string col, val, val2, rule;
    for (auto test : getRuleParams)
    {
        col = test.at(0);
        val = test.at(1);
        val2 = test.at(2);
        rule = test.at(3);

        errors += (rule != instance->GetRule(col, val, val2));
    }
    
    return 0;
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

static void ResetToCio()
{
    cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);
}

} // namespace ashs  