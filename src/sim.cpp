#include "sim.hpp"

#include <string>
#include <vector>
#include <fstream>

#include "settings.hpp"
#include "util.hpp"
#include "cell.hpp"
#include "render.hpp"


// cell calculation
void doCellCalc(
	size_t yStart, size_t yEnd, 
	size_t xStart, size_t xEnd
) {
	for (size_t y = yStart; y < yEnd; ++y) {
		for (size_t x = xStart; x < xEnd; ++x) {
			SimState::newMap[y*SimState::cellsWidth + x] = SimState::cellMap[y*SimState::cellsWidth + x];
			const Point point {x, y};

			// check if cell
			Cell c = SimState::cellMap[y*SimState::cellsWidth + x];
			if (c.state != 0) {
				const Rules& local = SimState::rules[c.state];
				// get surrounding cell outcome
				float outcome = getNeighboringValues(point, local);

				if (
					local.explicitJump != -1 && 
					(
						(SimState::simMode == 0 && (outcome < local.minToSurvive || outcome >= local.minToDie)) ||
						(SimState::simMode == 1 && (!isIn(local.sv_survivalValues, (int)outcome)))
					)
				) {
					// explicit jump activated
					SimState::newMap[y*SimState::cellsWidth + x].state = local.explicitJump;
				} else if (
					(SimState::simMode == 0 && (outcome < local.minToSurvive)) ||
					(SimState::simMode == 1 && (!isIn(local.sv_survivalValues, (int)outcome)))
				) {
					// not in survival range; lower state
					SimState::newMap[y*SimState::cellsWidth + x].state--;
				} else if (SimState::simMode == 0 && outcome >= local.minToDie) {
					// raise state (only possible in range-based sim mode)
					SimState::newMap[y*SimState::cellsWidth + x].state++;
					// state too big? kill it
					if (SimState::newMap[y*SimState::cellsWidth + x].state == static_cast<int>(SimState::rules.size())) {
						SimState::newMap[y*SimState::cellsWidth + x].state = 0;
					}
				}
			} else {
				// no cell, check if life
				float outcome = getNeighboringValues(point, SimState::rules[0]);
				
				// cells regenerate preferring higher states
				for (size_t r = 1; r < SimState::rules.size(); ++r) {
					if (SimState::simMode == 0) {
						if (SimState::rules[r].minForReproduction <= outcome && outcome <= SimState::rules[r].maxForReproduction) {
							// alive!
							SimState::newMap[y*SimState::cellsWidth + x] = Cell{r};
						}
					} else if (SimState::simMode == 1) {
						if (isIn(SimState::rules[r].sv_birthValues, (int)outcome)) {
							// alive!
							SimState::newMap[y*SimState::cellsWidth + x] = Cell{r};
						}
					}
				}
			}

			// calculate render
			for (size_t py = 0; py < SimState::pixelsPerCell; ++py) {
				for (size_t px = 0; px < SimState::pixelsPerCell; ++px) {
					SimState::screen[
						(SimState::pixelsPerCell*y+py)*SimState::screenWidth + (SimState::pixelsPerCell*x+px)
					] = SimState::stateColors[SimState::newMap[y*SimState::cellsWidth + x].state];
				}
			}
		}
	}
}

// returns the number of surrounding cells
// TODO: DEPRICATED
int getSurrounding(const Point& point, const std::vector<Cell>& cells, const Rules& rules, size_t screenWidth, size_t screenHeight) {
	int total = 0;

	static const Point toCheck[] = {
		{-1, -1}, {-1, 0}, {-1, 1},  // left row
		{0, -1}, {0, 1},             // middle row
		{1, -1}, {1, 0}, {1, 1}      // right row
	};

	for (size_t i = 0; i < 8; ++i) {
		// if cell exists
		int newY = point.y + toCheck[i].y, newX = point.x + toCheck[i].x;

		if (newX >= 0 && newX < static_cast<int>(screenWidth) && newY >= 0 && newY < static_cast<int>(screenHeight)) {
			int cstate = cells[newY*static_cast<int>(screenWidth) + newX].state;
			total += rules.surrounding[cstate];
		} else {
			// out of bounds
			total += rules.outOfBounds;
		}
	}

	return total;
}


void readFileIntoRules(std::vector<Rules>& rules, const std::string& path) {
	/*
	 * -1,-1,-1,-1 ~ 0,1,1 _   0,  0,  0
	 *  2, 5, 4, 4 ~ 0,2,0 _ 255,  0,  0
	 *  4, 5, 3, 3 ~ 0,1,1 _ 255,255,255
	**/

	std::ifstream file(path);

	for (std::string line; std::getline(file, line);) {

	}
}