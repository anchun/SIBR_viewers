
#ifndef __SIBR_VIEW_SKYBOX_HPP__
# define __SIBR_VIEW_SKYBOX_HPP__

# include "core/view/Config.hpp"
# include "core/graphics/Shader.hpp"
# include "core/graphics/Texture.hpp"
# include "core/graphics/Camera.hpp"

namespace sibr
{
	/** A skybox object for rendering a cubemap texture.
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT Skybox
	{
		SIBR_CLASS_PTR(Skybox);

	public:

		/** Load skybox faces from a directory. The files should be named: {right, left, top, bottom, forward, back}.jpg
		\param skyFolder directory path
		\return a success boolean 
		*/
		bool	load(const std::string& skyFolder);

		/** Render in the current RT.
		\param eye current viewpoint
		\param imgSize the destination RT size
		*/
		void	render(const Camera& eye, const sibr::Vector2u& imgSize);

	private:

		GLShader		_shader; ///< Skybox shader.
		GLParameter		_paramView; ///< VP parameter.
		GLParameter		_paramAspect; ///< Aspect ratio parameter.

		TextureCubeMapRGB::Ptr	_cubemap = nullptr; ///< Cubemap texture.

	};


} // namespace sibr

#endif // __SIBR_VIEW_SKYBOX_HPP__
