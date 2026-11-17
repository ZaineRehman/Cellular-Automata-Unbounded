#include "cell.hpp"

#include <vector>
#include <cmath>

#include "settings.hpp"
#include "util.hpp"
#include "sim.hpp"


float getNeighboringValues(const Point& point, const Rules& rules) {
	float total = 0;

	for (size_t i = 0; i < SimState::neighborhoodType.pairs; ++i) {
		// if cell exists
		int newY = point.y + SimState::neighborhoodType.points[i].first.y, 
			newX = point.x + SimState::neighborhoodType.points[i].first.x;

		if (newX >= 0 && newX < static_cast<int>(SimState::cellsWidth) && newY >= 0 && newY < static_cast<int>(SimState::cellsHeight)) {
			// in bounds
			int cstate = SimState::cellMap[newY*static_cast<int>(SimState::cellsWidth) + newX].state;
			if (SimState::simMode == 0) {
				total += rules.surrounding[cstate]*SimState::neighborhoodType.points[i].second;
			} else {
				total += static_cast<float>(rules.sv_surrounding[cstate])*SimState::neighborhoodType.points[i].second;
			}
		} else {
			// out of bounds
			total += rules.outOfBounds;
		}
	}

	return total;
}


CellBias Neighborhood::simple_calc(int n, bool falloff, float vmax, float vmin) {
	if (n == 1) falloff = false;

	CellBias built {{}, 0};

	for (int y = -n; y <= n; ++y) {
		for (int x = -n; x <= n; ++x) {
			if (y == 0 && x == 0) continue;

			built.points.push_back({{x,y}, falloff ? vmin + ((vmax-vmin)/(n-1.0f))*(n-max(abs(x),abs(y))) : 1.0f});
			built.pairs++;
		}
	}

	return built;
}

CellBias Neighborhood::vonNeumann_calc(int n, bool falloff) {
	if (n == 1) falloff = false;
	
	CellBias built {{}, 0};

	for (int y = -n; y <= n; ++y) {
		for (int x = -n; x <= n; ++x) {
			if (!(y == 0 || x == 0)) continue;
			if (y == 0 && x == 0) continue;

			built.points.push_back({{x,y}, falloff ? 1.5f - 0.5f*(n-1.0f)*max(abs(x),abs(y)) : 1.0f});
			built.pairs++;
		}
	}

	return built;
}

CellBias Neighborhood::carved_calc(int n, bool falloff) {
	if (n == 1) falloff = false;

	CellBias built {{}, 0};

	for (int y = -n; y <= n; ++y) {
		for (int x = -n; x <= n; ++x) {
			if (y == 0 && x == 0) continue;

			float bias = (y == 0 || x == 0) ? 1.5f : 0.5f;

			built.points.push_back({{x,y}, (falloff ? (1.5f - 0.5f*(n-1.0f)*max(abs(x),abs(y))) : 1.0f) * bias});
			built.pairs++;
		}
	}

	return built;
}


//static const CellBias toCheck = {
//	{{-1, -1}, 0.5f}, {{-1, 0}, 1.5f}, {{-1, 1}, 0.5f},  // left row
//	{{0, -1}, 1.5f}, {{0, 1}, 1.5f},                     // middle row
//	{{1, -1}, 0.5f}, {{1, 0}, 1.5f}, {{1, 1}, 0.5f}      // right row
//};