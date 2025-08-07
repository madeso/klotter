#pragma once

#include "klotter/render/texture.h"

#include <memory>

namespace klotter
{

/// A temporary collection of assets that can be used in the example.
/// This should be replaced with something way better that this yolo crap.
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
	std::shared_ptr<TextureCubemap> get_skybox();

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
	std::shared_ptr<TextureCubemap> skybox;
};

}  //  namespace klotter
