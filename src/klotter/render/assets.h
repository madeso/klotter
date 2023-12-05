#pragma once

#include <memory>

#include "klotter/render/texture.h"

namespace klotter
{

struct Assets
{
	std::shared_ptr<Texture> get_cookie();
	std::shared_ptr<Texture> get_container_diffuse();
	std::shared_ptr<Texture> get_container_specular();
	std::shared_ptr<Texture> get_dark_grid();
	std::shared_ptr<Texture> get_light_grid();
	std::shared_ptr<Texture> get_matrix();
	std::shared_ptr<Texture> get_black();
	std::shared_ptr<Texture> get_white();

   private:

	std::shared_ptr<Texture> cookie;
	std::shared_ptr<Texture> dark_grid;
	std::shared_ptr<Texture> light_grid;
	std::shared_ptr<Texture> container_diffuse;
	std::shared_ptr<Texture> container_specular;
	std::shared_ptr<Texture> matrix;
	std::shared_ptr<Texture> black;
	std::shared_ptr<Texture> white;
};

}  //  namespace klotter
