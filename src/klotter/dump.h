#pragma once

namespace svg
{
    constexpr std::string_view black = "black";
}

struct SvgLine
{
    std::string_view color;
    std::vector<glm::vec2> points;
};


struct Svg
{
    std::vector<SvgLine> lines;

    Svg& add_line(const std::vector<glm::vec2>& points, std::string_view color);

    void write(const std::string& file_path, float spacing);
};
