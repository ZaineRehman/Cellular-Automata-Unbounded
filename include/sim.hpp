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
	// explicit value to jump to when dead
	// if -1, uses default up/down rules
	int explicitJump;
	// surrounding values taken from each cell state
	// index 0 = for empty cells, index 1 = state 1, etc. 
	std::vector<float> surrounding;
	
	// set-value mode survival values
	std::vector<int> sv_survivalValues;
	// set-value mode birth values
	std::vector<int> sv_birthValues;
	// set-value mode surrounding values
	std::vector<int> sv_surrounding;
	

	Rules() = default;
	Rules(
		float _minToSurvive, float _minToDie, 
		float _minForReproduction, float _maxForReproduction, 
		float _outOfBounds, 
		int _explicitJump, 
		const std::vector<float>& _surrounding
	) : minToSurvive(_minToSurvive), minToDie(_minToDie), 
		minForReproduction(_minForReproduction), maxForReproduction(_maxForReproduction), 
		outOfBounds(_outOfBounds), 
		explicitJump(_explicitJump), 
		surrounding(_surrounding) 
	{
		sv_survivalValues = {2,3};
		sv_birthValues = {3};
		sv_surrounding = {};

		sv_surrounding.push_back(0);
		for (size_t i = 0; i < surrounding.size()-1; ++i) {
			sv_surrounding.push_back(1);
		}
	}
	Rules(const Rules& other) = default;
};


#include "cell.hpp"


// cell calculation
void doCellCalc(
	size_t yStart, size_t yEnd, 
	size_t xStart, size_t xEnd
);


void readFileIntoRules(std::vector<Rules>& rules, const std::string& path);