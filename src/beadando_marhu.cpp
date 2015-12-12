//============================================================================
// Name        : beadando_marhu.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <fstream>

#include "QuadTree.hpp"

using namespace std;

int main() {

	srand(time(NULL));

	ofstream pointsOUT, areas, kNN;
	ifstream pointsIN;

	pointsOUT.open("pointsOUT.txt");
	pointsIN.open("pointsIN.txt");
	areas.open("areas.txt");
	kNN.open("kNN.txt");

	QuadTree * QT = new QuadTree(100,10,800,800);	//100 kapacitasú, 10 max mélységű, 800*800 as alapteruletu fa

	QT->Read(pointsIN);						//kezdeti pontok beolvasasa
	QT->AddRandomPoints(100);				//véletlen pontok hozzáadása
	QT->Write(areas, pointsOUT);			//szerkezet és benne levő adatok kiirása (külön fájlba)
	QT->Search(Point2D(192,342),20,kNN);	//kNN keresés, a viszonyitás pont teljesen fuggetlen a tároltaktol

	kNN.close();
	areas.close();
	pointsOUT.close();
	pointsIN.close();

	cout << "A program futása befejeződött!" << endl;

	return 0;
}
