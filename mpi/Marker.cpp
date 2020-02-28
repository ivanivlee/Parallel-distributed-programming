#include "Marker.h"
#include <iostream>

using namespace std;

Marker::Marker(void) 
{
    marker = 65;
}

int Marker::GetMarker()
{
    return marker;
}

void Marker::IncMarker()
{
    marker++;
    if (marker > 89)
    {
        marker = 65;
    }
    return;
}