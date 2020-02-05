
#ifndef __SIBR_EXP_RENDEDER_POISSONRENDERER_HPP___
# define __SIBR_EXP_RENDEDER_POISSONRENDERER_HPP___

# include "Config.hpp"

# include <iostream>
# include <vector>
# include <memory>

# include <core/graphics/Config.hpp>
# include <core/graphics/Texture.hpp>
# include "core/graphics/Shader.hpp"

namespace sibr { 

	/**
	* Hole filling by poisson synthesis on an input textures;
	* contains all shaders, render targets and render passes.
	* All black pixels on the input texture are considered holes
	* and Poisson synthesis affects these pixels only, all
	* other pixels are treated at Dirichlet boundary conditions.
	* \ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT PoissonRenderer
	{
	public:
		typedef std::shared_ptr<PoissonRenderer>	Ptr;

	public:

		/**
		* Initialize Poisson solvers render targets and shaders.
		* \param w width of highest (or highest resolution) multigrid level
		* \param h height of highest (or highest resolution) multigrid level
		*/
		PoissonRenderer ( uint w, uint h );

		void	process(
			/*input*/	const RenderTargetRGBA::Ptr& src,
			/*ouput*/	RenderTargetRGBA::Ptr& dst );
		void	process(
			/*input*/	uint texID,
			/*ouput*/	RenderTargetRGBA::Ptr& dst );

		/**
		* Return the size used for in/out textures (defined in ctor)
		*/
		const Vector2i&		getSize( void ) const;

		bool & enableFix() { return _enableFix; }

	private:
		/**
		* Render the full Poisson synthesis on the holes in texture 'tex'.
		* \param tex OpenGL texture handle of input texture
		* \returns OpenGL texture handle of texture containing POisson synthesis solution
		*/
		uint render( uint tex );

		/** Size defined in the ctor */
		Vector2i		_size;

		/** Shader to perform Jacobi relaxations */
		sibr::GLShader	_jacobiShader;

		/** Shader to downsample input texture and boundary conditions from
		* higher multigrid level to next lower level */
		sibr::GLShader	_restrictShader;

		/** Shader to interpolate Poisson synthesis solution from
		* lower multigrid level to next higher level */
		sibr::GLShader	_interpShader;

		/** Shader to compute divergence (second derivative) field of input texture */
		sibr::GLShader	_divergShader;

		/** Render target to store Poisson synthesis result */
		RenderTargetRGBA::Ptr  _poisson_RT;

		/** Helper render target for \p _poisson_RT to
		* perform ping-pong render passes during Jacobi relaxations */
		RenderTargetRGBA::Ptr  _poisson_tmp_RT;

		/** Dirichlet constraints for each multigrid level */
		std::vector<RenderTargetRGBA::Ptr> _poisson_div_RT;

		sibr::GLParameter _jacobi_weights, _jacobi_scale, _restrict_scale;
		sibr::GLParameter _interp_scale;

		bool _enableFix;
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDEDER_POISSONRENDERER_HPP___
