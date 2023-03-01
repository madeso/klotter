#pragma once

namespace klotter::colors
{

constexpr auto white = glm::vec3{1.0f, 1.0f, 1.0f};
constexpr auto black = glm::vec3{0.0f, 0.0f, 0.0f};

constexpr glm::vec3 rgb(int r, int g, int b)
{
	return
	{
		static_cast<float>(r) / 255.0f,
		static_cast<float>(g) / 255.0f,
		static_cast<float>(b) / 255.0f
	};
}


// https://www.nature.com/articles/nmeth.1618
constexpr glm::vec3 orange = rgb(230, 159, 0);
constexpr glm::vec3 blue_sky = rgb(86, 180, 233);
constexpr glm::vec3 green_bluish = rgb(0, 158, 115);
constexpr glm::vec3 yellow = rgb(240, 228, 66);
constexpr glm::vec3 blue = rgb(0, 114, 178);
constexpr glm::vec3 red_vermillion = rgb(213, 94, 0);
constexpr glm::vec3 purple_redish = rgb(204, 121, 167);

}
