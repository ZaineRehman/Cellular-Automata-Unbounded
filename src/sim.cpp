#include "sim.hpp"

#include <string>
#include <vector>
#include <fstream>

#include "cell.hpp"
#include "render.hpp"


// cell calculation
void doCellCalc(
	size_t yStart, size_t yEnd, 
	size_t xStart, size_t xEnd, 
	size_t cellsHeight, size_t cellsWidth, 
	const std::vector<Cell>& cellMap, std::vector<Cell>& newMap, 
	const std::vector<Rules>& rules, const std::vector<RGBA_t>& stateColors, 
	std::vector<RGBA_t>& screen, 
	size_t pixelsPerCell, size_t WIDTH, size_t HEIGHT
) {
	for (size_t y = yStart; y < yEnd; ++y) {
		for (size_t x = xStart; x < xEnd; ++x) {
			newMap[y*cellsWidth + x] = cellMap[y*cellsWidth + x];
			const Point point {x, y};

			// check if cell
			Cell c = cellMap[y*cellsWidth + x];
			if (c.state != 0) {
				const Rules& local = rules[c.state];
				// get surrounding cell outcome
				float outcome = getNeighboringValues(point, cellMap, local, cellsWidth, cellsHeight, Neighborhood::moores);

				if (outcome < local.minToSurvive) {
					// lower state
					newMap[y*cellsWidth + x].state--;
				} else if (outcome >= local.minToDie) {
					// raise state
					newMap[y*cellsWidth + x].state++;
					// state too big? kill it
					if (newMap[y*cellsWidth + x].state == static_cast<int>(rules.size())) {
						newMap[y*cellsWidth + x].state = 0;
					}
				}
			} else {
				// no cell, check if life
				float outcome = getNeighboringValues(point, cellMap, rules[0], cellsWidth, cellsHeight, Neighborhood::moores);

				// cells regenerate preferring higher states
				for (size_t r = 1; r < rules.size(); ++r) {
					if (rules[r].minForReproduction <= outcome && outcome <= rules[r].maxForReproduction) {
						// alive!
						newMap[y*cellsWidth + x] = Cell{r};
					}
				}
			}

			// calculate render
			for (size_t py = 0; py < pixelsPerCell; ++py) {
				for (size_t px = 0; px < pixelsPerCell; ++px) {
					screen[(pixelsPerCell*y+py)*WIDTH + (pixelsPerCell*x+px)] = stateColors[newMap[y*cellsWidth + x].state];
				}
			}
		}
	}
}

// returns the number of surrounding cells
int getSurrounding(const Point& point, const std::vector<Cell>& cells, const Rules& rules, size_t WIDTH, size_t HEIGHT) {
	int total = 0;

	static const Point toCheck[] = {
		{-1, -1}, {-1, 0}, {-1, 1},  // left row
		{0, -1}, {0, 1},             // middle row
		{1, -1}, {1, 0}, {1, 1}      // right row
	};

	for (size_t i = 0; i < 8; ++i) {
		// if cell exists
		int newY = point.y + toCheck[i].y, newX = point.x + toCheck[i].x;

		if (newX >= 0 && newX < static_cast<int>(WIDTH) && newY >= 0 && newY < static_cast<int>(HEIGHT)) {
			int cstate = cells[newY*static_cast<int>(WIDTH) + newX].state;
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