#pragma once

#include <vector>


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
	CellBias simple_calc(int n, bool falloff);

	inline CellBias moores = simple_calc(1,false);

	// diagonals weighted less, sides weighted more
	
}