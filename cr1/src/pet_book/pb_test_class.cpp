// Standard C++ includes
#include <iostream>
#include <vector>
#include <fstream>

#include "pb_test_class.hpp"

using namespace std;

namespace ashs
{

// cio pointers
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
static void PrtStrToTerm(ifstream* in, ofstream* out, const string& str);
static void CheckForErrorsToTerm (const string& testTitle,
            size_t errors, size_t* sumErrors, ifstream* in, ofstream* out);

// expected return values
static const string exitRetval = "exit";

static const vector<string> getAllQueries =
{
    "SELECT * FROM pets ORDER BY pet_id ASC"
};

static const vector<string> findByQueries =
{
    // single value tests
    "SELECT * FROM pets WHERE name LIKE '%Luna%'",  // input: name Luna
    "SELECT * FROM pets WHERE name=5",              // input: name 5
    "SELECT * FROM pets WHERE 6=2",                 // input: 6 2

    // value range tests
    // input: age_months -r 0 15
    "SELECT * FROM pets WHERE age_months BETWEEN 0 AND 15",

    // input: age_months -r 15 0
    "SELECT * FROM pets WHERE age_months BETWEEN 15 AND 0",

    // input: name -r b l
    "SELECT * FROM pets WHERE name BETWEEN 'b' AND 'l' OR name LIKE '%l%'"
};

static const vector<string> filterByQueries =
{
    // input: name Luna age_months 20
    "SELECT * FROM pets WHERE name LIKE '%Luna%' AND age_months=20",

    // input: status adopted name Luna
    "SELECT * FROM pets WHERE status LIKE '%adopted%' AND name LIKE '%Luna%'",

    // input: status adopted name Luna
    "SELECT * FROM pets WHERE status LIKE '%adopted%' AND name LIKE '%Lu%'",

    // input: status adopted name 00000
    "SELECT * FROM pets WHERE status LIKE '%adopted%' AND name=00000",

    // input: status adopted bad_param Luna
    "SELECT * FROM pets WHERE status LIKE '%adopted%' AND bad_param LIKE '%Luna%'"
};

static const vector<string> filterConcatQueries =
{
    // input: status adopted name i age_months 3 name a
    "SELECT * FROM pets WHERE status LIKE '%adopted%' AND name LIKE '%i%' AND age_months=3 AND name LIKE '%a%'"
};

static const vector<string> orderByQueries =
{
    // input: status adopted age_months DESC
    "SELECT * FROM pets WHERE status LIKE '%adopted%' ORDER BY age_months DESC"
};

static const vector<string> updateQueries =
{
    // input: pk = 3, col = status, val = inshelter
    "UPDATE pets SET status='inshelter' WHERE pet_id=3;",

    // input: pk = 3, col = status, val = adopted
    "UPDATE pets SET status='adopted' WHERE pet_id=3;",

    // input: pk = f, pk = 0, pk = abc, pk = 3, col = status, val = single
    "UPDATE pets SET status='single' WHERE pet_id=3;"
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

static const vector<vector<string> > removeChoiceQueries = 
{
    // datatable,   pk,     expected query

    // correct currId fed to instance
    { "pets",   "100",  "DELETE FROM pets WHERE pet_id=100;"    },

    // no currId fed to instance
    { "pets",   "",         ""  }
};

static const vector<string> removeQueries =
{
    // input: pk = 101
    "DELETE FROM pets WHERE pet_id=101;",

    // input: pk = 000, pk = abc, pk = 5
    "DELETE FROM pets WHERE pet_id=5;"
};

static const vector<string> chooseQueries =
{
    // input: pk = 2, operation = 1
    "SELECT * FROM pets WHERE pet_id=2",

    // input: pk = 2, operation = 2
    "SELECT * FROM adopters WHERE adopter_id=2",

    // input: pk = 2, operation = 3, col = age_months, val = 60
    "UPDATE pets SET age_months='60' WHERE pet_id=2;",

    // input: pk = , pk = abc, pk = 2, operation = 4
    "DELETE FROM pets WHERE pet_id=2;"
};

static const vector<vector<string> > FindJoinedQueries =
{
    // datatable,   FK,     expected query

    // choose a pet and find its joined adopter
    // input: datatable = 1, pk = 3
    { "pets",       "3",    "SELECT * FROM adopters WHERE adopter_id=2" },
    
    // choose an adopter and find its joined pet/s
    // input: datatable = 2, pk = 2
    { "adopters",   "2",    "SELECT * FROM pets WHERE adopter_id=2"     },

    // input: datatable = 1, pk = 0, pk = 0000, pk = abc, pk = 8
    { "pets",       "8",    "SELECT * FROM adopters WHERE adopter_id=4" },

    // input: datatable = 1, pk = 1 (an entry with FK = NULL)
    { "pets",       "1",    ""  }
};

static const vector<string> FindCurrIdQueries =
{
    // input: datatable = 2, pk = 2
    "SELECT * FROM adopters WHERE adopter_id=2",

    // input: datatable = 1, pk = 3
    "SELECT * FROM pets WHERE pet_id=3",

    // input: datatable = 1, pk = 0, pk = 000, pk = 1
    "SELECT * FROM pets WHERE pet_id=1"
};

static const vector<vector<string> > getRuleParams =
{
    //  col,        val,        val2,        expected rule    
    { "number",     "2",        "",         "number=2"                  },
    { "number",     "2",        "3",        "number BETWEEN 2 AND 3"    },
    { "text",       "hello",    "",         "text LIKE '%hello%'"       },
    { "text",       "hello",    "world",    "text LIKE '%hello%'"       },
    { "textnum",    "0678abc",  "",         "textnum LIKE '%0678abc%'"  }
};

// Tests ///////////////////////////////////////////////

void PbTestClass::PublicMethodsTest()
{
    cout << "-- PublicMethodsTest():" << endl;

	size_t errors = 0;

    cout << "---- StartTest(): " << endl;
    StartTest();

    cout << "---- StopTest(): " << endl;
    StopTest();

    cout << "---- SetDatabaseTest(): " << endl;
    SetDatabaseTest();

    cout << "-- Public Methods sum-up: ";
    CheckForErrors(errors, NULL);
    cout << endl;
}

void PbTestClass::PrivateMethodsTest()
{
    cout << "-- PrivateMethodsTest():" << endl;

	size_t errors = 0;

	cout << "---- MakeStringTest(): ";
	CheckForErrors(MakeStringTest(), &errors);

	// cout << "---- ExecutInputTest(): ";
	// CheckForErrors(ExecutInputTest(), &sumErrors);

    cout << "-- Private Methods sum-up: ";
    CheckForErrors(errors, NULL);
    cout << endl;
}

void PbTestClass::StringFuncsTest()
{
    cout << "-- StringFuncsTest(): " << endl;

    size_t errors = 0;

    cout << "---- ExitTest(): ";
    CheckForErrors(ExitTest(), &errors);

    cout << "---- ClearSearchTest(): ";
    CheckForErrors(ClearSearchTest(), &errors);

    // Initial queries

    cout << "---- FindByTest(): ";
    CheckForErrors(FindByTest(), &errors);

    cout << "---- GetAllTest(): ";
    CheckForErrors(GetAllTest(), &errors); 

    // Secondary queries

    cout << "---- FilterByTest(): ";
    CheckForErrors(FilterByTest(), &errors);

    cout << "---- OrderByTest(): ";
    CheckForErrors(OrderByTest(), &errors);

    cout << "---- ChooseEntryTest(): ";
    CheckForErrors(ChooseEntryTest(), &errors);

    cout << "---- FindJoinedTest(): ";
    CheckForErrors(FindJoinedTest(), &errors);
    
    // Editorial operations

    cout << "---- UpdateFieldTest(): ";
    CheckForErrors(UpdateFieldTest(), &errors);

    cout << "---- AddEntryTest(): ";
    CheckForErrors(AddEntryTest(), &errors);

    cout << "---- RemoveEntryTest(): ";
    CheckForErrors(RemoveEntryTest(), &errors);

    // Helper operations

    cout << "---- FindByCurrIdTest(): ";
    CheckForErrors(FindByCurrIdTest(), &errors); 

    cout << "---- FindByMaxIdTest(): ";
    CheckForErrors(FindByMaxIdTest(), &errors); 

    cout << "---- SelectDataTest(): ";
    CheckForErrors(SelectDataTest(), &errors); 

    cout << "---- SelectDataWhereTest(): ";
    CheckForErrors(SelectDataWhereTest(), &errors); 

    cout << "---- SelectDataAscTest(): ";
    CheckForErrors(SelectDataAscTest(), &errors); 

    cout << "---- GetRuleTest(): ";
    CheckForErrors(GetRuleTest(), &errors);

    cout << "-- StringFuncs sum-up: ";
    CheckForErrors(errors, NULL);
    cout << endl;
}

// Public Method Tests /////////////////////////////////
void PbTestClass::StartTest()
{
    ifstream in(START_IN);
	ofstream out(START_OUT);
    RedirectToFile(&in, &out);

	instance->Start();

	ResetToCio();
}

void PbTestClass::StopTest()
{
    instance->Stop();
}

void PbTestClass::SetDatabaseTest()
{
    instance->SetDatabase("test_db");

    instance->Start();
}

// Private Method Tests ////////////////////////////////
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

    // basic test
    // input: exit
    instance->currId = "old_leftovers";
    instance->MakeString();
    errors += (instance->currQuery != exitRetval);

    // new search test
    // input: 1 show_all exit
    instance->currId = "";
    instance->MakeString();
    errors += (instance->currQuery != getAllQueries[0]);
    instance->MakeString();
    errors += (instance->currQuery != exitRetval);

    // wrong input test (table choice)
    // input: 0 100 -1 [] text 1 exit
    instance->currId = "";
    instance->MakeString();
    errors += (instance->currQuery != exitRetval);

    // wrong input test (operation choice)
    // input: 1 flylikesuperman makewinefromwater exit
    instance->currId = "";
    instance->MakeString();
    errors += (instance->currQuery != exitRetval);

    // operation tests
    CheckForErrorsToTerm("\n------ MakeFindTest(): ",
        MakeFindTest(), &errors, &in, &out);

    CheckForErrorsToTerm("------ MakeFilterTest(): ",
        MakeFilterTest(), &errors, &in, &out);

    CheckForErrorsToTerm("------ MakeOrderTest(): ",
        MakeOrderTest(), &errors, &in, &out);

    CheckForErrorsToTerm("------ MakeUpdateTest(): ",
        MakeUpdateTest(), &errors, &in, &out);

    CheckForErrorsToTerm("------ MakeAddTest(): ",
        MakeAddTest(), &errors, &in, &out);

    CheckForErrorsToTerm("------ MakeRemoveTest(): ",
        MakeRemoveTest(), &errors, &in, &out);

    CheckForErrorsToTerm("------ MakeFindJoinedTest(): ",
        MakeFindJoinedTest(), &errors, &in, &out);

    CheckForErrorsToTerm("------ MakeFindCurrIdTest(): ",
        MakeFindCurrIdTest(), &errors, &in, &out);

    ResetToCio();

    // TO DO:
    // - order by concatenation
    cout << "---- MakeString() sum-up: ";
    return errors;
}

// MakeStringTest Internal Tests
size_t PbTestClass::MakeFindTest()
{
    size_t errors = 0;
    
    // input: 1 find <params>
    for (auto query : findByQueries)
    {
        instance->currId = "";
        instance->MakeString();
        errors += (query != instance->currQuery);
    }

    return errors;
}

size_t PbTestClass::MakeFilterTest()
{
    size_t errors = 0;

    // input: 1 find <params> filter <params>
    for (auto query : filterByQueries)
    {
        instance->currId = "";
        instance->currQuery = "";

        instance->MakeString();     // find
        instance->MakeString();     // filter
        errors += (query != instance->currQuery);
    }

    // filter concatenation
    // input: 1 find <params> filter <params> filter <params> filter <params>
    for (auto query : filterConcatQueries)
    {
        instance->currId = "";
        instance->currQuery = "";

        instance->MakeString();     // find
        instance->MakeString();     // filter
        instance->MakeString();     // filter
        instance->MakeString();     // filter
        errors += (query != instance->currQuery);
    }

    return errors;
}

size_t PbTestClass::MakeOrderTest()
{
    size_t errors = 0;

    // input: 1 find <params> order <params>
    for (auto query : orderByQueries)
    {
        instance->currId = "";
        instance->currQuery = "";

        instance->MakeString();     // find
        instance->MakeString();     // order
        errors += (query != instance->currQuery);
    }

    return errors;
}

size_t PbTestClass::MakeUpdateTest()
{
    size_t errors = 0;

    // input: 1 update <params>
    for (auto query : updateQueries)
    {
        instance->currId = "";
        instance->currQuery = "";

        instance->MakeString();
        errors += (query != instance->currQuery);
    }

    // parameters fed to instance by the "choose" operation
    //input: 1 choose <param> 3
    for (auto query : updateQueries)
    {
        instance->currId = "";
        instance->currQuery = "";

        instance->MakeString();
        errors += (query != instance->currQuery);
    }

    return errors;
}

size_t PbTestClass::MakeAddTest()
{
    size_t errors = 0;

    // input: 1 add_new <params>
    for (auto query : addQueries)
    {
        instance->currId = "";
        instance->currQuery = "";

        instance->MakeString();
        errors += (query != instance->currQuery);
    }

    return errors;
}

size_t PbTestClass::MakeRemoveTest()
{
    size_t errors = 0;

    // parameters fed to instance by the "choose" operation
    //input: 1 choose <param> 4
    for (auto query : removeQueries)
    {
        instance->currQuery = "";
        instance->currId = "";
        instance->MakeString();
        errors += (query != instance->currQuery);
    }

    // parameters received by direct user input
    // input: 1 delete <param>
    for (auto query : removeQueries)
    {
        instance->currQuery = "";
        instance->currId = "";
        instance->MakeString();
        errors += (query != instance->currQuery);
    }

    return errors;
}

size_t PbTestClass::MakeFindJoinedTest()
{
    size_t errors = 0;

    // params fed to instance by the "choose" operation
    for (auto test : FindJoinedQueries)
    {
        instance->currQuery = "";
        instance->currId = "";
        instance->MakeString();
        errors += (test.at(2) != instance->currQuery);
    }

    return errors;
}

size_t PbTestClass::MakeFindCurrIdTest()
{
    size_t errors = 0;

    for (auto query : FindCurrIdQueries)
    {
        instance->currQuery = "";
        instance->currId = "";
        instance->MakeString();
        errors += (query != instance->currQuery);
    }

    return errors;
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
        instance->currQuery = "";

        instance->currQuery = instance->FindBy();
        errors += (query != instance->FilterBy());
    }

    for (auto query : filterConcatQueries)
    {
        instance->currQuery = "";

        instance->currQuery = instance->FindBy();
        instance->FilterBy();
        instance->FilterBy();
        errors += (query != instance->FilterBy());
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

    // currId fed to instance by the "choose" operation
    for (auto test : FindJoinedQueries)
    {
        instance->SetDataTable(test.at(0));
        instance->currQuery = "choose";
        instance->currId = test.at(1);
        errors += (test.at(2) != instance->FindJoined());
    }

    // currId received by direct user input
    for (auto test : FindJoinedQueries)
    {
        instance->SetDataTable(test.at(0));
        instance->currQuery = "";
        instance->currId = "";
        errors += (test.at(2) != instance->FindJoined());
    }

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
        instance->SetDataTable("pets");
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
        instance->SetDataTable("pets");
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

    // parameters fed to instance by the "choose" operation
    for (auto test : removeChoiceQueries)
    {
        instance->SetDataTable(test.at(0));
        instance->currQuery = "choose";
        instance->currId = test.at(1);
        errors += (test.at(2) != instance->RemoveEntry());
    }

    // parameters received by direct user input
    for (auto query : removeQueries)
    {
        instance->currQuery = "";
        instance->currId = "";
        errors += (query != instance->RemoveEntry());
    }

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

// Testing Helper functions /////////////////////////////////////////////////
static void CheckForErrors(size_t errors, size_t* sumErrors)
{
    if (errors)
    {
        cout << errors << " errors";

        if (NULL != sumErrors)
        {
            ++(*sumErrors);
        }
    }
    else
    {
        cout << "-";
    }
    
    cout << endl;
}

static void CheckForErrorsToTerm (const string& testTitle,
        size_t errors, size_t* sumErrors, ifstream* in, ofstream* out)
{
    if (errors)
    {
        PrtStrToTerm(in, out, testTitle + (errors + " errors"));
        ++(*sumErrors);
    }
    else
    {
        PrtStrToTerm(in, out, testTitle + "-");
    }
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

static void PrtStrToTerm(ifstream* in, ofstream* out, const string& str)
{
    ResetToCio();
    cout << str << endl;
    RedirectToFile(in, out);
}

} // namespace ashs