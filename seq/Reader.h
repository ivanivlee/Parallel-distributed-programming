#ifndef READER_H
#define READER_H
#include <vector>
#include <string>

using namespace std;

class Reader
{
public:
    Reader(); // konstruktor
    void Read(std::string s); //precti soubor
    vector<int> GetData();
   
private:
    vector<int> data;
};

#endif