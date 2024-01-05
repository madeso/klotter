#pragma once

#include <memory>

#include "klotter/render/texture.h"

namespace klotter
{

struct Assets
{
	std::shared_ptr<Texture2d> get_black();
	std::shared_ptr<Texture2d> get_white();

	std::shared_ptr<Texture2d> get_cookie();
	std::shared_ptr<Texture2d> get_dark_grid();
	std::shared_ptr<Texture2d> get_light_grid();

	std::shared_ptr<Texture2d> get_container_diffuse();
	std::shared_ptr<Texture2d> get_container_specular();
	std::shared_ptr<Texture2d> get_matrix();
	std::shared_ptr<Texture2d> get_glass();
	std::shared_ptr<Texture2d> get_grass();

   private:

	std::shared_ptr<Texture2d> black;
	std::shared_ptr<Texture2d> white;

	std::shared_ptr<Texture2d> cookie;
	std::shared_ptr<Texture2d> dark_grid;
	std::shared_ptr<Texture2d> light_grid;

	std::shared_ptr<Texture2d> container_diffuse;
	std::shared_ptr<Texture2d> container_specular;
	std::shared_ptr<Texture2d> matrix;
	std::shared_ptr<Texture2d> glass;
	std::shared_ptr<Texture2d> grass;
};

}  //  namespace klotter
