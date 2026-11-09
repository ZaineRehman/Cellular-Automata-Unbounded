#pragma once

#include <vector>

#include "render.hpp"


// rules for a given cell state
struct Rules {
	// minimum surrounding to survivwe
	float minToSurvive;
	// minimum surrounding for overpopulation death
	float minToDie;
	// min amount of surrounding for reproduction
	float minForReproduction;
	// max amount of surrounding for reproduction
	float maxForReproduction;
	// value taken from out of bounds areas
	float outOfBounds;
	// surrounding values taken from each cell state
	// index 0 = for empty cells, index 1 = state 1, etc. 
	std::vector<float> surrounding;

	Rules() = default;
	Rules(float _minToSurvive, float _minToDie, float _minForReproduction, float _maxForReproduction, float _outOfBounds, const std::vector<float>& _surrounding) 
		: minToSurvive(_minToSurvive), minToDie(_minToDie), minForReproduction(_minForReproduction), maxForReproduction(_maxForReproduction), outOfBounds(_outOfBounds), surrounding(_surrounding) {}
	Rules(const Rules& other) = default;
};


#include "cell.hpp"


// cell calculation
void doCellCalc(
	size_t yStart, size_t yEnd, 
	size_t xStart, size_t xEnd, 
	size_t cellsHeight, size_t cellsWidth, 
	const std::vector<Cell>& cellMap, std::vector<Cell>& newMap, 
	const std::vector<Rules>& rules, const std::vector<RGBA_t>& stateColors, 
	std::vector<RGBA_t>& screen, 
	size_t pixelsPerCell, size_t WIDTH, size_t HEIGHT, 
	const CellBias& neighborhoodType
);


void readFileIntoRules(std::vector<Rules>& rules, const std::string& path);