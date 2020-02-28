#include "Grid.h"
#include "State.h"
#include <iostream>
#include <vector>

using namespace std;

Grid::Grid(vector<int> stream)
{
    this->offset = 1;
    
    this->n = stream[0];
    this->m = stream[1];
    this->alpha = stream[2];
    this->beta = stream[3];
    this->costAlpha = stream[4];
    this->costBeta = stream[5];
    this->costEmpty = stream[6];
    this->numberOfForbiddenPlaces = stream[7];

    for(int i = 0; i < (2*numberOfForbiddenPlaces); i++) {
        this->forbiddenXs[i] = stream[2*i + 8];
        forbiddenYs[i] = stream[2*i+1 + 8];
    }

    this->types.resize(4);
    this->directions.resize(4);
    this->types[0] = beta;
    this->types[1] = beta;
    this->types[2] = alpha;
    this->types[3] = alpha;
    this->directions[0] = 0;
    this->directions[1] = 1;
    this->directions[2] = 0;
    this->directions[3] = 1;
}

vector<vector<int>>  Grid::initGrid()
{
    //filling with zeros
    vector<vector<int>> grid;
    grid.resize(m + 2);
    for(int i = 0; i < m + 2; i++)
    {
        grid[i].resize(n + 2);
    }
    
    for(int i = 0; i < m + 2; i++){
        for(int j = 0; j < n + 2; j++){
            grid[i][j] = 0;
        }  
    }
    //printGrid(grid, m, n);
    // filling with ones - places which are allowed
    for(int i = 1; i < m + 1; i++){
        for(int j = 1; j < n + 1; j++){
            grid[i][j] = 1;
        }
    }
    //printGrid(grid, m, n);
    // forbidden shores
    for(int i = 0; i < numberOfForbiddenPlaces; i++){
        grid[forbiddenXs[i]+1][forbiddenYs[i]+1] = 0;
        //printGrid(grid, m, n);
    }
    //printGrid(grid, m, n);
    return grid;
}

coordinates Grid::ShiftPos(int x, int y, int length, int offset)
{
    coordinates newPos;
    if (x < length) {
        newPos.x = x + 1;
        newPos.y = y;
    }
    if (x >= length){
        newPos.x = offset;
        newPos.y = y + 1;
    }
    
    return newPos;
}

int Grid::BranchMaxCost(int places)
{
  // returns the maximal price for the "number" of unsolved squares
    int x;
    int max = costBeta * (places / beta);
    int r = places % beta;
    
    max += costAlpha * (r / alpha);
    r = r % alpha;
    max += r * costEmpty;
    
    for (int i = 0; i < (places/beta); i++) {
    x = costBeta * i;
    r = places - i * beta;
    x += costAlpha * (r/alpha);
    r = r % alpha;
    x += r * costEmpty;
    if (x > max) max = x;
    }
    return max;
}