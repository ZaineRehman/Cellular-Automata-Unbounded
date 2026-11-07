#include "cell.hpp"

#include <vector>
#include <cmath>

#include "sim.hpp"


float Neighborhood::standard(const Point& point, const std::vector<Cell>& cells, const Rules& rules, size_t WIDTH, size_t HEIGHT) {
	float total = 0;

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

float Neighborhood::sidesBias(const Point& point, const std::vector<Cell>& cells, const Rules& rules, size_t WIDTH, size_t HEIGHT) {
	float total = 0;

	static const std::pair<Point, float> toCheck[] = {
		{{-1, -1}, 0.5f}, {{-1, 0}, 1.5f}, {{-1, 1}, 0.5f},  // left row
		{{0, -1}, 1.5f}, {{0, 1}, 1.5f},             // middle row
		{{1, -1}, 0.5f}, {{1, 0}, 1.5f}, {{1, 1}, 0.5f}      // right row
	};

	for (size_t i = 0; i < 8; ++i) {
		// if cell exists
		int newY = point.y + toCheck[i].first.y, newX = point.x + toCheck[i].first.x;

		if (newX >= 0 && newX < static_cast<int>(WIDTH) && newY >= 0 && newY < static_cast<int>(HEIGHT)) {
			int cstate = cells[newY*static_cast<int>(WIDTH) + newX].state;
			total += rules.surrounding[cstate]*toCheck[i].second;
		} else {
			// out of bounds
			total += rules.outOfBounds;
		}
	}

	return total;
}