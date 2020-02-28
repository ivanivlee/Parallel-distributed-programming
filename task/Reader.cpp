#include "Reader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

Reader::Reader(void) 
{
    this->data.resize(100);
}

void Reader::Read(std::string s)
{
    cout << "reading file..." << endl;
    ifstream inFile; int x; int i = 0;
    string name = "poi" + s + ".txt";
    inFile.open(name); // add choice of the number
    if (!inFile) {
        cerr << "Unable to open file poi" + s + ".txt";
        exit(1);   // call system to stop
    }
    while (inFile >> x) {
       data[i] = x;
       i++;
    }
    inFile.close();

    return;
}

vector<int> Reader::GetData()
{
    return data;
}