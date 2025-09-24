#include "klotter/vdebug.h"

#include "klotter/str.h"

#include <glm/glm.hpp>
#include <random>
#include <vector>


using Pair = std::pair<glm::vec2, glm::vec2>;

// Returns an array containing the points nearest to one another out of the given set
Pair FindClosestPairOfPoints(const std::vector<glm::vec2>& points)
{
	auto vd = VisualDebugging::VisualDebug{};

    vd.BeginFrame("All points", true);
    vd.DrawPoints(points, 5);
    Pair closestPointPair;
    float bestDst = std::numeric_limits<float>::max();

    for (std::size_t i = 0; i < points.size(); i++)
    {
        for (std::size_t j = i + 1; j < points.size(); j++)
        {
            float dst = (points[i] - points[j]).length();
            if (dst < bestDst)
            {
                bestDst = dst;
                closestPointPair.first = points[i];
                closestPointPair.second = points[j];
            }
            vd.BeginFrame("Compare dst", true);
			vd.SetColor(VisualDebugging::Colors::lightRed, VisualDebugging::Colors::veryDarkGrey);
            vd.DrawPoint(points[i], 6);
            vd.DrawArrow(points[i], points[j], 10);
            vd.DontShowNextElementWhenFrameIsInBackground();
            vd.SetColor(VisualDebugging::Colors::lightGreen);
			vd.DrawLineSegmentWithLabel(closestPointPair.first, closestPointPair.second, Str {}  << bestDst);
        }
    }

    vd.BeginFrame("Finished");
    vd.SetColor(VisualDebugging::Colors::lightGreen);
	vd.DrawPoints({closestPointPair.first, closestPointPair.second}, 7);
	vd.DrawLineSegmentWithLabel(closestPointPair.first, closestPointPair.second, Str{} << bestDst);
	vd.Save("closest points");
    return closestPointPair;
}

std::vector<glm::vec2> GeneratePoints()
{
    int numPoints = 8;
    float radius = 100.0f;
    
    std::random_device r;
	std::uniform_real_distribution uniform(0.0f, radius);

    std::vector<glm::vec2> points;
    points.reserve(numPoints);
    for (int i = 0; i < numPoints; i++)
    {
		points.emplace_back(glm::vec2{uniform(r), uniform(r)});
    }
    return points;
}

int main(int, char*[])
{
    FindClosestPairOfPoints(GeneratePoints());
}

