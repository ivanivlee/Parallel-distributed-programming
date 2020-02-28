#ifndef STATE_H
#define STATE_H
#include <vector>


using namespace std;

class State
{
public:
    State(vector<vector<int>> grid, int x, int y, int pA, int pB, int e, int lT, int lD, int left);
    void PrintState(int sizeX, int sizeY);
    int Cost(int cAlpha, int cBeta, int cEmpty, int places);
    int Empty(int m, int n);
    int IsTheEnd(int m, int n);
    int SquareIsEmpty();
    int Try(int direction, int type);
    vector<vector<int>> Place(vector<vector<int>> grid, int type, int direction, int marker);

    int x; //my current position, coordinate x
    int y; //my current position, coordinate y
    int cost; //cost of this state
    int placedA; //number of placed tiles type A
    int placedB; //numer of placed tiles type B
    int empty; //number of total empty squares
    int lastType; //type of last placed tile
    int lastDirection; //direction of last placed tile
    int left; //number of empty squares left to be empty
    vector<vector<int>> grid;
};

#endif

