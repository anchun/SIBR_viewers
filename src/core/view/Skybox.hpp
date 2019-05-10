
#ifndef __SIBR_VIEW_SKYBOX_HPP__
# define __SIBR_VIEW_SKYBOX_HPP__

# include "core/view/Config.hpp"
# include "core/graphics/Shader.hpp"
# include "core/graphics/Texture.hpp"
# include "core/graphics/Camera.hpp"

namespace sibr
{
	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT Skybox
	{
		SIBR_CLASS_PTR(Skybox);

	public:
		bool	load(const std::string& skyFolder);
		void	render(const Camera& eye, const sibr::Vector2u& imgSize);

	private:

		GLShader		_shader;
		GLParameter		_paramView;
		GLParameter		_paramAspect;

		TextureCubeMapRGB::Ptr	_cubemap = nullptr;

	};


} // namespace sibr

#endif // __SIBR_VIEW_SKYBOX_HPP__
