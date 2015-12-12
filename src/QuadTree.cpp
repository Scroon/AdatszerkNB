/*
 * QuadTree.cpp
 *
 *  Created on: 2015. nov. 19.
 *      Author: Abuh
 */

#include "QuadTree.hpp"

QuadTree::QuadTree(const int _capacity, const int _depth, const float _width, const float _height) : capacity(_capacity), max_depth(_depth) {
	area = new Area(Point2D(_width/2,_height/2),_width,_height,0);
}

QuadTree::~QuadTree() {
	area->~Area();
}

QuadTree::Area* QuadTree::container(Point2D p, Area * a) const {

	if( p.x < a->center.x) {
		if( p.y < a->center.y) return a->NW;
		else return a->SW;
	}
	else {
		if( p.y < a->center.y) return a->NE;
		else return a->SE;
	}
}

void QuadTree::write(Area * a, std::ofstream &o1, std::ofstream &o2) {

	o1 << a->center.x-a->width/2 << " " << a->center.y-a->height/2 << " " << a->width << " " << a->height << std::endl;

	if(!a->IsLeaf()) {
		write(a->NE,o1,o2);
		write(a->NW,o1,o2);
		write(a->SE,o1,o2);
		write(a->SW,o1,o2);
	}
	else {

		for(unsigned int i = 0; i < a->koords.size(); i++) {
			o2 << a->koords[i].x << " " << a->koords[i].y << std::endl;
		}
	}
}

void QuadTree::Insert(Point2D p) {
	insert(p,area);

	std::cout << "Pont beszúrása ... 100%" << std::endl;
}

void QuadTree::insert(Point2D p, Area * a) {

	if(a->IsFull(capacity)){
		if(a->IsLeaf()){
			if(a->depth < max_depth) {
				split(a);
				insert(p,a);
			}
			else a->koords.push_back(p);
		}
		else {
			insert(p, container(p,a));
		}
	}
	else {
		a->koords.push_back(p);
	}
}

void QuadTree::split(Area * a) {

	a->NW = new Area(Point2D(a->center.x-a->width/4,a->center.y-a->height/4),a->width/2,a->height/2,a->depth++);
	a->NE = new Area(Point2D(a->center.x+a->width/4,a->center.y-a->height/4),a->width/2,a->height/2,a->depth++);
	a->SW = new Area(Point2D(a->center.x-a->width/4,a->center.y+a->height/4),a->width/2,a->height/2,a->depth++);
	a->SE = new Area(Point2D(a->center.x+a->width/4,a->center.y+a->height/4),a->width/2,a->height/2,a->depth++);

	std::vector<Point2D> points = a->koords;
	a->koords.clear();

	for(int i = 0; i < capacity; i++) {
		insert(points[i],a);
	}
}

void QuadTree::Write(std::ofstream &o1, std::ofstream &o2) {
	write(area,o1,o2);

	std::cout << "Az adatszerkezet fájlba mentése ... 100%" << std::endl;
	std::cout << "Az adatok fájlba mentése ... 100%" << std::endl;
}

void QuadTree::Search (Point2D p, const int k, std::ofstream &o) {

	ComparePoint C(p);					//a viszonyitasi pont

	Area * buffer = NULL;				//aktualis terulet, seged

	std::priority_queue<Point2D, std::vector<Point2D>, QuadTree::ComparePoint> kNN(p);	//teruletek
	std::priority_queue<Area *, std::vector<Area *>, QuadTree::ComparePoint> Areas(p);	//pontok

	std::vector<Point2D> ret;

	ret.push_back(p);		//a viszonyitas pont mindig az elso a kimeneti fajlban

	Areas.push(area);

	while(!Areas.empty() && (int) ret.size() < k+1) {			//addig hogy meglegyen a kello hosszusagu tomb vagy ha elfogytak a teruletek

		if(Areas.top()->IsLeaf()) {								//ha levél terulet akkor kiszedem belole a pontokat

			for(unsigned int i = 0; i < Areas.top()->koords.size(); i++) {
				kNN.push(Areas.top()->koords[i]);
			}

			if( ret[0].x == kNN.top().x &&  ret[0].y == kNN.top().y) kNN.pop();					//ha a referencia szerepel a fában akkor ne adjuk hozza 2szer

			Areas.pop();

		}
		else {													//ha nem level akkor az alteruletekt veszem fel a sorba

			buffer = Areas.top();

			Areas.pop();

			Areas.push(buffer->NE);
			Areas.push(buffer->NW);
			Areas.push(buffer->SE);
			Areas.push(buffer->SW);
		}

		while(!kNN.empty() && (int) ret.size() < k+1) {										//meprobalom atszedni a pontakat a megoldas vektorba, addig amig van pontom vagy megvan a k darab
			if(Areas.empty() || C.distancePP(kNN.top()) <= C.distancePA(Areas.top())) {		//akkor tehetem meg ha a legközelebbi pontnal nincs kozelebbi terulet
				ret.push_back(kNN.top());
				kNN.pop();
			}
			else break;																		//egyébként a kozelebbi teruletben lehet még kozelebbi pont,ezért folytatodjon a kulso while
		}
	}

	for(unsigned int i = 0; i < ret.size(); i++) {
		o << ret[i].x << " " << ret[i].y << std::endl;
	}

	std::cout << "A " << k << "-'legközelebi szomszéd' keresés ... " <<  (int) (((ret.size()-1) / ((float) k)) * 100) << "%" << std:: endl;
}

void QuadTree::Read(std::ifstream &i) {

	if(i.fail()) {
		std::cout << "Nem talalható bemeneti fájl!" << std::endl;
		i.clear();
		return;
	}

	Point2D p(0,0);
	int h;

	i >> h;

	while(!i.eof()) {
		p.x = h;
		i >> p.y;
		i >> h;
		insert(p,area);
	}

	std::cout << "Kezdeti pontok beolvasása ... 100%" << std::endl;
}

void QuadTree::AddRandomPoints(const int db) {
	for(int i = 0; i < db; i++) {
		insert(Point2D(rand()%area->width,rand()%area->height),area);
	}
	std::cout << "Még " << db << " pont véletlenszerű elhelyezése ... 100%" << std::endl;
}
