#ifndef GRID_H
#define GRID_H
#include "State.h"
#include <vector>
using namespace std;

struct coordinates {
   int   x;
   int   y;
};

class Grid
{
public:
    Grid(vector<int> stream);
    coordinates ShiftPos(int x, int y, int length, int offset);
    vector<vector<int>> initGrid();
    int BranchMaxCost(int places);
    
    int alpha;
    int beta;
    int costAlpha;
    int costBeta;
    int costEmpty;
    int m;
    int n;
    int numberOfForbiddenPlaces;
    int forbiddenXs[100]; 
    int forbiddenYs[100];
    int offset;
    vector<int> types;
    vector<int> directions;
};

#endif

