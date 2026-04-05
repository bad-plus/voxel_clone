#include "circle_around_point.h"
#include <algorithm>

std::vector<std::pair<int, int>> Utils::circleAroundPoint(int cx, int cz, int radius)
{
	std::vector<std::pair<int, int>> out;

	for (int x = -radius; x <= radius; x++) {
		for (int z = -radius; z <= radius; z++) {
			if (x * x + z * z <= radius * radius) {
				out.emplace_back(cx + x, cz + z);
			}
		}
	}

	std::sort(out.begin(), out.end(), [cx, cz](const auto& a, const auto& b) {
		int dist_a = (a.first - cx) * (a.first - cx) + (a.second - cz) * (a.second - cz);
		int dist_b = (b.first - cx) * (b.first - cx) + (b.second - cz) * (b.second - cz);
		return dist_a < dist_b;
		});

	return out;
}