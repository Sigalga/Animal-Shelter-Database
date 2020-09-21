#include <bits/stdc++.h> // std::sort
#include <vector>

using namespace std;

void printVector(vector<int> v);
vector<int> v = {1, 2, 3};

int main()
{
    sort(v.begin(),
        v.end(),
        [](const int& a, const int& b) -> bool { return a > b; });
    printVector(v);
    return 0;
}


// Function to print vector 
void printVector(vector<int> v) 
{ 
    // lambda expression to print vector 
    for_each(v.begin(), v.end(), [](int i) 
    { 
        std::cout << i << " "; 
    }); 
    cout << endl; 
} 