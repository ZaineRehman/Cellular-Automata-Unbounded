#pragma once

#include <vector>
#include <iomanip>  // remove after debugging?
#include <iostream>  // ^^^


struct Point {
	int x, y;

	Point() = default;
	Point(int _x, int _y) : x(_x), y(_y) {}
	Point(size_t _x, size_t _y) : x(static_cast<int>(_x)), y(static_cast<int>(_y)) {}
	Point(const Point& other) = default;

	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}

	Point operator+(const Point& other) const {
		return Point{x+other.x, y+other.y};
	}
};

struct CellBias {
	std::vector<std::pair<Point, float>> points;
	size_t pairs;
};

inline void outputCellBias(const CellBias& cb) {
	std::cout << std::fixed << std::setprecision(2);

	std::cout << "{";
	for (auto& i : cb.points) {
		std::cout << "\t(" << i.first.x << "," << i.first.y << "), " << i.second << "\n";
	}
	std::cout << "}\n";
}


// custom hash function for Point
/*template <>
struct std::hash<Point> {
	std::size_t operator()(const Point& point) const {
		std::size_t h1 = std::hash<int>{}(point.x);
		std::size_t h2 = std::hash<int>{}(point.y);

		std::size_t seed = h1;
		seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

		return seed;
	}
};*/


class Cell {
public:
	//Point coords {};
	int state;

	Cell() = default;
	Cell(int _state) : state(_state) {}
	Cell(size_t _state) : state(static_cast<int>(_state)) {}
	//Cell(int _state/**/, const Point& _point) : state(_state), coords(_point) {}
	Cell(const Cell& other) = default;
};


#include "sim.hpp"

float getNeighboringValues(const Point& point, const std::vector<Cell>& cells, const Rules& rules, size_t WIDTH, size_t HEIGHT, const CellBias& toCheck);

// points to check to return the value of surrounding cells (getNeighboringValues)
namespace Neighborhood {
	// standard surrounding
	CellBias simple_calc(int n, bool falloff, float vmax = 1.0f, float vmin = 0.5f);
	// ignore diagonals
	CellBias vonNeumann_calc(int n, bool falloff);
	// bias sides 1.5x more, diagonals 1.5x less
	CellBias carved_calc(int n, bool falloff);

	// standard Moore Neighborhood
	inline CellBias moore = simple_calc(1,false);
	// standard falloff, n=2
	inline CellBias falloff2 = simple_calc(2,true);
	// standard falloff, weighted less, n=2
	inline CellBias falloff2_less = simple_calc(2,true, 1.0f, 0.25f);

	// standard Von Neumann neighborhood
	inline CellBias vonNeumann = vonNeumann_calc(2, false);
	
	// sides are biased higher than diagonals, n=1
	inline CellBias carved = carved_calc(1,true);
}