
#ifndef __SIBR_VIEW_RENDERINGMODE_HPP__
# define __SIBR_VIEW_RENDERINGMODE_HPP__

# include "core/graphics/Camera.hpp"
# include "core/graphics/Viewport.hpp"
# include "core/graphics/Texture.hpp"
# include "core/view/Config.hpp"
# include "core/view/ViewBase.hpp"
# include "core/graphics/Image.hpp"
# include "core/graphics/Shader.hpp"
# include "core/assets/InputCamera.hpp"

namespace sibr
{
	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT IRenderingMode
	{
		SIBR_CLASS_PTR(IRenderingMode);

	public:
		typedef RenderTargetRGB RenderTarget;
	public:
		virtual ~IRenderingMode( void ) { }

		virtual void	render( 
			ViewBase& view, const sibr::Camera& eye, const sibr::Viewport& viewport, 
			IRenderTarget* optDest = nullptr) = 0;

		/** Get current rendered image in the view port */
		virtual void destRT2img( sibr::ImageRGB& current_img ) = 0;

	protected:
		// prev RT to link renderers across different views in multipass
		std::unique_ptr<RenderTargetRGB>	_prevL, _prevR;
	public:
		// Clear true by default, false when using prev
		bool _clear;

		// Set prev to link renderers across views in multipass -- use L for mono
		void	setPrev(const std::unique_ptr<RenderTargetRGB>& p) { std::cerr<<"ERROR " << std::endl; } //_prevL = std::move(p); }
		void	setPrevLR(const std::unique_ptr<RenderTargetRGB>& pl, const std::unique_ptr<RenderTargetRGB>& pr) { std::cerr<<"ERROR " << std::endl;} // _prevL = std::move(pl), _prevR = std::move(pr); }
		virtual const std::unique_ptr<RenderTargetRGB>&	lRT() = 0;
		virtual const std::unique_ptr<RenderTargetRGB>&	rRT() = 0;

	};

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT MonoRdrMode : public IRenderingMode
	{
	public:

		MonoRdrMode( void );

		void	render( ViewBase& view, const sibr::Camera& eye, const sibr::Viewport& viewport, IRenderTarget* optDest = nullptr);

		void destRT2img( sibr::ImageRGB& current_img )
		{
			_destRT->readBack(current_img);
			return;
		}

		// same for mono
		virtual const std::unique_ptr<RenderTargetRGB>&	lRT() { return _destRT; }
		virtual const std::unique_ptr<RenderTargetRGB>&	rRT() { return _destRT; }
	private:
		sibr::GLShader							_quadShader;
		std::unique_ptr<RenderTarget>		_destRT;
	};

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT StereoAnaglyphRdrMode : public IRenderingMode
	{
	public:

		StereoAnaglyphRdrMode( void );

		void	render( ViewBase& view, const sibr::Camera& eye, const sibr::Viewport& viewport, IRenderTarget* optDest = nullptr);

		void	setFocalDist(float focal) { _focalDist = focal; }
		void	setEyeDist(float iod) { _eyeDist = iod; }

		float	focalDist()	{ return _focalDist; }
		float	eyeDist()	{ return _eyeDist; }

		// Empty
		void destRT2img( sibr::ImageRGB& current_img ){};

		virtual const std::unique_ptr<RenderTargetRGB>&	lRT() { return _leftRT; } /// \todo TODO -- generalize
		virtual const std::unique_ptr<RenderTargetRGB>&	rRT() { return _rightRT; } /// \todo TODO -- generalize

	private:
		sibr::GLShader		_stereoShader;
		RenderTarget::UPtr	_leftRT, _rightRT;
		float				_focalDist, _eyeDist;
	};

	///// DEFINITIONS /////

} // namespace sibr

#endif // __SIBR_VIEW_RENDERINGMODE_HPP__
