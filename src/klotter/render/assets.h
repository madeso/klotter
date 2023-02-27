#pragma once

#include <memory>

#include "klotter/render/texture.h"


namespace klotter
{

struct Assets
{
    std::shared_ptr<Texture> get_dark_grid();
    std::shared_ptr<Texture> get_white();

private:
    std::shared_ptr<Texture> dark_grid;
    std::shared_ptr<Texture> white;
};

}