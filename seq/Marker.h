#ifndef MARKER_H
#define MARKER_H

using namespace std;

class Marker
{
public:
    void IncMarker(); //posunout marker
    Marker(); // konstruktor
    int GetMarker(); //precti, jaky je ted zrovna marker
private:
    int marker; //aktualni marker
};

#endif

