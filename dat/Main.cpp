#include <iostream>
#include <chrono>
#include <omp.h>
#include <vector>
#include <queue>
#include "Marker.h"
#include "Reader.h"
#include "State.h"
#include "Grid.h"

using namespace std;
int maxCost;
vector<vector<int>> bestGrid;
queue<State> Q;

int Solve(State state, Grid g, Marker m)
{
    //dno rekurze
    if (state.IsTheEnd(g.m, g.n)) //testovaci input / kratsi
    {
        int cost = state.Cost(g.costAlpha, g.costBeta, g.costEmpty, state.empty);
        //cout << "cost of this state: " << cost << endl;
        if (cost > maxCost) {
            #pragma omp critical
            {
            if (cost > maxCost) {    
            maxCost = cost; //aktualizujeme cenu
            //cout << " max " << maxCost;
            //cout << " placed alphas: " << state.placedA;
            //cout << " places betas: " << state.placedB << endl;
            bestGrid = state.grid;
            }
            }
        }
        //state.PrintState(g.m, g.n);
        return cost;
    }

    //orezani levnych vetvi
    int branchCost = g.BranchMaxCost(state.empty - state.left);
    int cost = state.Cost(g.costAlpha, g.costBeta, g.costEmpty, state.left);
    int realCost = state.Cost(g.costAlpha, g.costBeta, g.costEmpty, state.empty);
    int limit = cost + branchCost;
    if (realCost > maxCost)
    {
        #pragma omp critical
        {
        if (realCost > maxCost)
        {
        maxCost = realCost;
        //cout << " max " << maxCost;
        //cout << " placed alphas: " << state.placedA;
        //cout << " places betas: " << state.placedB << endl;
        bestGrid = state.grid;
        }
        }
    }
    if (limit < maxCost ) {
        return cost; 
    }

    //prochazeni do listu - zkusi polozit
    if (state.SquareIsEmpty()) 
    {
        for(int i = 0; i < 4; i++)
        {
            int type = g.types[i];
            int direction = g.directions[i];
            //try - vyzkousi a vrati jde/nejde
            if (state.Try(direction, type) == 1) //jde umistit
            {
                vector<vector<int>> newGrid; 
                newGrid = state.Place(state.grid, type, direction, m.GetMarker()); //umisti dlazdici
                int empty = state.empty - type;
                int placedA = state.placedA;
                int placedB = state.placedB;
                if (type == g.alpha) {placedA++;}
                else {placedB++;}
                m.IncMarker();
                coordinates pos = g.ShiftPos(state.x, state.y, g.m, g.offset);
                vector<vector<int>> gr = g.initGrid(); //initialize the grid
                State newState(newGrid, pos.x, pos.y, placedA, placedB, empty, type, direction, state.left); //new state
                Solve(newState, g, m);
            }
            
        }
        state.left++; //misto nechame volne - nic neslo umistit
    }
    //else //neni prazdne
    
    //misto je uz plne nebo nelze nic umistit
    coordinates pos = g.ShiftPos(state.x, state.y, g.m, g.offset);

    State newState(state.grid, pos.x, pos.y, state.placedA, state.placedB, state.empty, state.lastType, state.lastDirection, state.left); //new state
    Solve(newState, g, m);
    
    return cost;
}

void BFS(Grid g, Marker m)
{
    State state = Q.front(); //posledni stav z fronty
    Q.pop();
    //prochazeni do listu - vytvori stavy
    if (state.SquareIsEmpty()) 
    {
        for(int i = 0; i < 4; i++)
        {
            int type = g.types[i];
            int direction = g.directions[i];
            //try - vyzkousi a vrati jde/nejde
            if (state.Try(direction, type) == 1) //jde umistit
            {
                vector<vector<int>> newGrid; 
                newGrid = state.Place(state.grid, type, direction, m.GetMarker()); //umisti dlazdici
                int empty = state.empty - type;
                int placedA = state.placedA;
                int placedB = state.placedB;
                if (type == g.alpha) {placedA++;}
                else {placedB++;}
                m.IncMarker();
                coordinates pos = g.ShiftPos(state.x, state.y, g.m, g.offset);
                vector<vector<int>> gr = g.initGrid(); //initialize the grid
                State newState(newGrid, pos.x, pos.y, placedA, placedB, empty, type, direction, state.left); //new state
                Q.push(newState);
            }
            
        }
        state.left++; //misto nechame volne - nic neslo umistit
    }
    //else //neni prazdne
    
    //misto je uz plne nebo nelze nic umistit
    coordinates pos = g.ShiftPos(state.x, state.y, g.m, g.offset);

    State newState(state.grid, pos.x, pos.y, state.placedA, state.placedB, state.empty, state.lastType, state.lastDirection, state.left); //new state
    Q.push(newState);

}

int main(int argc, char *argv[] )
{
    //global variables:
    maxCost = 0;
    Marker m;
    Reader r;

    r.Read(argv[1]);

    Grid g(r.GetData()); //data ze souboru jsou uchovavany jako verejne atributy Gridu g
    vector<vector<int>> gr = g.initGrid(); //initialize the grid
    State first(gr, 1, 1, 0, 0, (g.m*g.n - g.numberOfForbiddenPlaces), -1, -1, 0); //first empty state
    maxCost = (g.m*g.n - g.numberOfForbiddenPlaces) * g.costEmpty; // lower bound
    //cout << "max cost: " << maxCost << endl; 
    auto start = chrono::steady_clock::now(); // timing start
    
    
    //Datovy paralelismus
    int count = 0;
    int enough = 40; //dostatecny pocet
    Q.push(first);
    
    while (count < enough)
    {
        BFS(g, m); //updatuje frontu
        //cout << "bfs" << endl;
        count++;
    }

    vector<State> Qvector;
    for(int i = 0; i < Q.size(); i++)
    {
        Qvector.push_back(Q.front());
        Q.pop();
    }
    
    int i; int j;

    #pragma omp parallel for num_threads(8) default ( shared ) private (i, j ) 
    for (i = 0; i < Qvector.size(); i++) {
        std::cout << "Thread " << omp_get_thread_num() << " out of " << omp_get_num_threads() << std::endl;            
        State state = Qvector[i];
        j = Solve(state, g, m);
        #pragma omp critical
        {
        if ( j > maxCost) maxCost = j;
        }
        }   

    cout << "Number of threads: " << 8 << endl; 
    cout << "max cost: " << maxCost << endl; 
    State best(bestGrid, 1, 1, 0, 0, 0, -1, -1, 0); //winning state
    best.PrintState(g.m, g.n);
    auto end = chrono::steady_clock::now(); //timing end

	cout << "Elapsed time in milliseconds : " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    cout << "Elapsed time in seconds : " << chrono::duration_cast<chrono::seconds>(end - start).count() << " sec" << endl;

    return 0;
}