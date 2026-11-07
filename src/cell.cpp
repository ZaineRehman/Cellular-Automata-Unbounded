#include "cell.hpp"

#include <vector>
#include <cmath>

#include "util.hpp"
#include "sim.hpp"


float getNeighboringValues(const Point& point, const std::vector<Cell>& cells, const Rules& rules, size_t WIDTH, size_t HEIGHT, const CellBias& toCheck) {
	float total = 0;

	for (size_t i = 0; i < toCheck.pairs; ++i) {
		// if cell exists
		int newY = point.y + toCheck.points[i].first.y, newX = point.x + toCheck.points[i].first.x;

		if (newX >= 0 && newX < static_cast<int>(WIDTH) && newY >= 0 && newY < static_cast<int>(HEIGHT)) {
			int cstate = cells[newY*static_cast<int>(WIDTH) + newX].state;
			total += rules.surrounding[cstate]*toCheck.points[i].second;
		} else {
			// out of bounds
			total += rules.outOfBounds;
		}
	}

	return total;
}


CellBias Neighborhood::simple_calc(int n, bool falloff) {
	CellBias built {{}, 0};

	for (int y = -n; y <= n; ++y) {
		for (int x = -n; x <= n; ++x) {
			if (y == 0 && x == 0) continue;
			built.points.push_back({{x,y}, falloff ? 0.5f*n*abs(min(y,x)) : 1.0f});
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