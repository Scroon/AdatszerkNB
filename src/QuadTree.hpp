/*
 * QuadTree.hpp
 *
 *  Created on: 2015. nov. 19.
 *      Author: Abuh
 */

#ifndef QUADTREE_HPP_
#define QUADTREE_HPP_

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <queue>
#include <cstdlib>

struct Point2D {
		int x, y;

		Point2D(const int _x, const int _y) : x(_x), y(_y) {}
	};

class QuadTree {

protected:

	struct Area { //teruletek

		Point2D center;

		int width;
		int height;
		int depth;

		Area * NE;
		Area * NW;
		Area * SE;
		Area * SW;

		std::vector<Point2D> koords;

		Area(Point2D _center, int _width, int _height, int _depth) : center(_center), width(_width), height(_height), depth(_depth),
			NE(NULL), NW(NULL), SE(NULL), SW(NULL) {}

		~Area() {
			if(IsLeaf()) delete this;
			else {
				NE->~Area();
				NW->~Area();
				SE->~Area();
				SW->~Area();
			}
		}

		bool IsEmpty() {
			return koords.size() == 0;
		}
		bool IsLeaf() {
			return NE==NULL;
		}
		bool IsFull(int capacity) {
			return (int) koords.size() == capacity || !IsLeaf();
		}
		bool IsIn(Point2D p) {	//a p pont benne van ebben a teruletben?
			if(p.x > center.x-width/2 && p.y > center.y-height/2 && p.x < center.x+width/2 && p.y < center.y+height/2) return true;
			else return false;
		}
	};

	struct ComparePoint {	//kNN kereseshez, priority_queue-khoz

		Point2D q;	//a viszonyitási pont

		ComparePoint(Point2D _p) : q(_p) {}

		bool operator() (const Point2D& lhs, const Point2D& rhs) {		//q-lhs távolsaga nagyobb mint q-rhs? pontokkal
			if(distancePP(lhs) > distancePP(rhs)) return true;
			else return false;
		}
		bool operator() (QuadTree::Area * lhs, QuadTree::Area * rhs) { 	//q-lhs távolsaga nagyobb mint q-rhs? teruletekkel
			if(distancePA(lhs) > distancePA(rhs)) return true;
			else return false;
		}
		float distancePP(Point2D p) {									// q és p távolsaga
			return sqrt(pow((q.x-p.x),2)+pow((q.y-p.y),2));
		}
		float distancePA(Area * a) {									//q és a minimalis tavolsaga

			float dist = 0;

			if(a->IsIn(q)) return dist;									//ha rajta vagok akkor 0 a tavolsag
			else {

				std::priority_queue<Point2D, std::vector<Point2D>, ComparePoint> corners(q);	//csokkenoben a 4 csúcs és q tavolsaga

				corners.push(Point2D(a->center.x-a->width/2,a->center.y-a->height/2));
				corners.push(Point2D(a->center.x-a->width/2,a->center.y+a->height/2));
				corners.push(Point2D(a->center.x+a->width/2,a->center.y-a->height/2));
				corners.push(Point2D(a->center.x+a->width/2,a->center.y+a->height/2));

				Point2D one = corners.top();		//a két leg kisebb altal meghatarozott egyenesen lesz a meroleges
				corners.pop();
				Point2D two = corners.top();
				corners.pop();

				if(one.x == two.x) {	// a tengely függöleges
					if(abs(q.y-one.y) < abs(two.y-one.y) && abs(q.y-two.y) < abs(two.y-one.y)) dist=abs(one.x-q.x);			//ha a q a két csucs koze esik a tengely mentén, akkor a meroleges a legkisebb távolsaguk
					else distancePP(one) < distancePP(two) ?  dist=distancePP(one) : dist=distancePP(two);					// egyebken a kozelebbi csucs
				}
				else if(one.y == two.y) { // a tengely viszintes
					if(abs(q.x-one.x) < abs(two.x-one.x) && abs(q.x-two.x) < abs(two.x-one.x)) dist=abs(one.y-q.y); 		// ugyanaz csak a más tengelyre nezve
					else distancePP(one) < distancePP(two) ?  dist=distancePP(one) : dist=distancePP(two);
				}
				else std::cout << "Hiba: QuadTree::ComparePoint::DistancePA(...)" << std::endl;								// nem igazan fordulhatna elo más eset, da ha megis
			}

			return dist;
		}
	};

	int capacity;	//egy terulet maximalis kapacitasa
	int max_depth; 	//a fa maximalis mélysége

	Area * area;	// a fa alapterulete

	void insert(Point2D p, Area * a);	// belsok
	void split(Area * a);
	void write(Area * a, std::ofstream &o1, std::ofstream &o2);

	Area* container(Point2D p, Area * a) const;	//visszaadja, hogy p a-nak melyik negyedébe esik

public:

	QuadTree(const int _capacity, const int _depth, const float _width, const float _height);

	virtual ~QuadTree();

	void Insert(Point2D);
	void Write(std::ofstream &o1, std::ofstream &o2);
	void Read(std::ifstream &i);
	void Search(Point2D p, const int k, std::ofstream &o);
	void AddRandomPoints(const int db);
};


#endif /* QUADTREE_HPP_ */
