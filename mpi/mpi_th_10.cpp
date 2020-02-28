#include <iostream>
#include <chrono>
#include <omp.h>
#include <vector>
#include <queue>
#include <mpi.h>
#include "Marker.h"
#include "Reader.h"
#include "State.h"
#include "Grid.h"

using namespace std;
int maxCost;
vector<vector<int>> bestGrid;
queue<State> Q;
queue<State> R;

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
            //cout << " max " << maxCost << endl;
            //cout << " placed alphas: " << state.placedA;
            //cout << " places betas: " << state.placedB << endl;
            //state.PrintState(g.m, g.n);
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
        //cout << " max " << maxCost << endl;
        //cout << " placed alphas: " << state.placedA;
        //cout << " places betas: " << state.placedB;
        //cout << "" << endl;
        //state.PrintState(g.m, g.n);
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

int main(int argc, char *argv[])
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
   
    
    //MPI verze
    int my_rank;
    int count_processes;
    int index;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count_processes);

    
    
    //uvodni fronta, kterou si nageneruji vsichni
    int count = 0;
    int enough = 5;
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

    if (my_rank == 0) //master proces
    {  
        cout << "Number of processes: " << count_processes << endl;
        auto start = chrono::steady_clock::now(); // timing start
        index = 0;
        int result = 0;
        int winner = 0;
        MPI_Status status;
        for(int i = 0; i < Qvector.size(); i++)
        {
            if (i + 1 < count_processes) {
                MPI_Send(&index, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
                
            }
            else
            {
                
                MPI_Recv(&maxCost, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                MPI_Send(&index, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                if (maxCost > result) {
                    result = maxCost;
                    winner = status.MPI_SOURCE;
                }   
            }
            index++;  
        }
        for(int i = 1; i < count_processes; i++)
        {
            MPI_Recv(&maxCost, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (maxCost > result) {
                result = maxCost;
                winner = status.MPI_SOURCE;
            } 
            int end = -1;
            MPI_Send(&end, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);  
        }
        cout << maxCost << endl;
        auto end = chrono::steady_clock::now(); //timing end

	    cout << "Elapsed time in milliseconds : " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
        cout << "Elapsed time in seconds : " << chrono::duration_cast<chrono::seconds>(end - start).count() << " sec" << endl;
    
        //int print = -2;
        //MPI_Send(&print, 1, MPI_INT, , 0, MPI_COMM_WORLD); 

        
    }
    else //slave proces
    {
        bool end = false;
        while (!end)
        {
            MPI_Recv(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (index == -1)
            {
                MPI_Send(&maxCost, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                end = true;

                State best(bestGrid, 1, 1, 0, 0, 0, -1, -1, 0); //best state
                best.PrintState(g.m, g.n);
                
                
            }
            else
            {
                //nova fronta R
                int count = 0;
                int enough = 40;
                State firstState = Qvector[index];
                R.push(firstState);

                while (count < enough)
                {
                    BFS(g, m); //updatuje frontu
                    //cout << "bfs" << endl;
                    count++;
                }

                vector<State> Rvector;
                for(int i = 0; i < R.size(); i++)
                {
                    Rvector.push_back(R.front());
                    R.pop();
                }   
        

                int i; int j;

                #pragma omp parallel for num_threads(10) default ( shared ) private (i, j ) 
                for (i = 0; i < Rvector.size(); i++) {
                    std::cout << "Thread " << omp_get_thread_num() << " out of " << omp_get_num_threads() << std::endl;
                    State state = Rvector[i];
                    j = Solve(state, g, m);
                    #pragma omp critical
                    {
                        if ( j > maxCost) maxCost = j;
                        //cout << maxCost << endl;
                    }
                    }
                MPI_Send(&maxCost, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }  
        } 
            
        //State best(bestGrid, 1, 1, 0, 0, (g.m*g.n - g.numberOfForbiddenPlaces), -1, -1, 0); //best grid
        //best.PrintState(g.m, g.n);
    }
    
    

    
    MPI_Finalize();

    return 0;
}