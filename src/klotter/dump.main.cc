#include <iostream>

#include "klotter/dump.h"

int main()
{
    std::vector<glm::vec2> house = {
        {0.0f, 0.0f},   // bottom left
        {4.0f, 0.0f},   // bottom right
        {4.0f, 3.0f},   // top right
        {2.0f, 5.0f},   // roof peak
        {0.0f, 3.0f},   // top left
        {0.0f, 0.0f}    // close the base
    };

    Svg{}
        .add_line(house, svg::orange)
        .write("sample.html", 20.0f);

    std::cout << "Plot points dumped.\n";
    return 0;
}
