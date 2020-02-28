#include "State.h"
#include <iostream>


using namespace std;

State::State(vector<vector<int>> grid, int x, int y, int pA, int pB, int e, int lT, int lD, int left) 
{
    this->x = x;
    this->y = y;
    this->placedA = pA;
    this->placedB = pB;
    this->empty = e;
    this->lastType = lT;
    this->lastDirection = lD;
    this->left = left;
    this->grid = grid;
    //cout << "State has been constructed: " << x << " " << y << " " << placedA << " " << placedB << " " << endl;
}

void State::PrintState(int sizeX, int sizeY)
{
    unsigned char a = 65;
    for(int i = 0; i < sizeX + 2; i++){
        for(int j = 0; j < sizeY + 2; j++){
            if (grid[i][j] == 0) {
                cout << "#" << " ";
            }
            if (grid[i][j] == 1) {
                cout << "-" << " ";
            }
            if (grid[i][j] > 1) {
                a = grid[i][j];
                cout << a << " ";
            }
            
        }  
        cout << "" << endl;
    }
    cout << "" << endl;
    return;
}

int State::Cost(int cAlpha, int cBeta, int cEmpty, int places)
{
    int c;
    c = (cAlpha*placedA) + (cBeta*placedB) + (cEmpty*places);
    return c;
}

int State::Empty(int m, int n)
{
    int result = 0;
    for(int i = 1; i < m + 1; i++) //upraveno, nemusi davat dobre vysledky
    {
        //cout << "i" << i;
        //cout << endl;
        for(int j = 1; j < n + 1 ; j++) //upraveno, nemusi davat dobre vysledky / puvodne n + 1
        {
            //cout << "j" << j;
            //cout << endl;
            if (grid[i][j] == 1){
                result++;
            }
        }
        
    }
    //cout << " empty " << result << endl;
    return result;
}

int State::IsTheEnd(int m, int n)
{
    if (this->x >= m && this->y >= n){return 1;}
    else {return 0;}
}

int State::SquareIsEmpty()
{
    if (grid[this->x][this->y] == 1) {return 1;}
    else {return 0;}
}

int State::Try(int direction, int type)
{
    int placement = 1;
    switch (direction)
    {
        case 0: // right
            for(int i = 0; i < type; i++){  
                if (grid[x ][y + i] != 1) {
                    placement = -1; 
                    break;
                }                                            
            }
            break;
        
        default: // down
            for(int i = 0; i < type; i++){          
                if (grid[x + i][y] !=1) {
                    placement = -1; 
                    break;
                }                                            
            }
            break;

    }
    if (placement == -1) {return 0;}
    else {return 1;}
}

vector<vector<int>> State::Place(vector<vector<int>> grid, int type, int direction, int marker)
{
    switch (direction)
    {
        case 0: // right
            {
                for(int i = 0; i < type; i++){
                    grid[x ][y + i] = marker;
                }
            } 
            break;
        
        default: // down
            {
                for(int i = 0; i < type; i++){
                    grid[x + i][y] = marker; 
                    
                }
            } 
            break;

    }
    return grid;
}