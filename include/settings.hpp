#pragma once

#include <vector>

#include "render.hpp"
#include "cell.hpp"


struct SimState {
	// min value for cell rules
	inline static const float minRuleVal = -16.0f;
	// max value for cell rules
	inline static const float maxRuleVal = 16.0f;

	// default CGOL
	inline static const Rules defaultCgol {2.0f,4.0f,  3.0f,3.0f,  0.0f,  {}};

	inline static size_t screenWidth = 1000;
	inline static size_t screenHeight = 1000;
	inline static double fps = 60.0;
	inline static size_t numThreads = 16;
	inline static size_t pixelsPerCell = 3;

	inline static bool presentRules = true;

	// -1 = unpaused
	//  0 = paused
	//  n = move forward n generations
	inline static int paused = -1;

	// simulation mode
	// 0 = range-based
	// 1 = set values
	// 2 = ants
	inline static int simMode = 0;

	// delay for cell calc per frame
	inline static int frameDelay = 0;
	inline static int maxFrameDelay = 0;

	// show rules to terminal?
	inline static bool running = true;
	// index in rules vec
	inline static int edit_stateIndex = 0;
	// which specific rule
	inline static int edit_ruleIndex = 0;

	// radius of mouse press
	// 0 = point
	inline static int mouseRadius = 1;
	// mouse press fillin method
	// 0 = highest state
	// 1 = random states, no empty
	// 2 = random states, with empty
	inline static int mouseFillMethod = 0;

	inline static std::vector<RGBA_t> screen{SimState::screenHeight*SimState::screenWidth, RGBA_t{0,0,0,255}};

	inline static size_t cellsWidth = SimState::screenWidth/SimState::pixelsPerCell;
	inline static size_t cellsHeight = SimState::screenHeight/SimState::pixelsPerCell;

	//std::unordered_map<Point, Cell> cellMap, newMap {};
	inline static std::vector<Cell> cellMap = std::vector<Cell>(cellsWidth*cellsHeight, Cell{0}), 
									newMap  = std::vector<Cell>(cellsWidth*cellsHeight, Cell{0});

	inline static std::vector<Rules> rules {
		Rules{-1.0f,-1.0f,  -1.0f,-1.0f,  0.0f,  {0.0f,0.0f,1.0f}}, 
		Rules{ 2.0f, 3.0f,  -1.0f,-1.0f,  0.0f,  {0.0f,1.0f,2.0f}}, 
		Rules{ 2.0f, 5.0f,   4.0f, 4.0f,  0.0f,  {0.0f,0.0f,1.0f}}, 
	};
	inline static std::vector<RGBA_t> stateColors {
		RGBA_t{  0,   0,   0, 255}, 
		RGBA_t{255,   0,   0, 255}, 
		RGBA_t{255, 255, 255, 255}, 
	};

	inline static CellBias neighborhoodType = Neighborhood::moore;
};