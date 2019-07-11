
#ifndef __SIBR_VIEW_ViewBase_HPP__
# define __SIBR_VIEW_ViewBase_HPP__

# include <type_traits>

# include "core/graphics/Texture.hpp"
# include "core/graphics/Camera.hpp"
# include "core/view/Config.hpp"
//# include "core/view/IBRScene.hpp"
#include "core/graphics/Input.hpp"
#include "core/graphics/Window.hpp"

namespace sibr
{

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT ViewBase
	{
	public:

		typedef std::shared_ptr<ViewBase> Ptr;
	
		ViewBase( const unsigned int w=720, const unsigned int h=480);

		virtual ~ViewBase() = default;

		virtual void	onUpdate(Input& /*input*/) { }
		virtual void	onRender( Window& /*win*/ )		{ }
		virtual void	onRenderIBR(IRenderTarget& /*dst*/, const Camera& /*eye*/) {};
		virtual void	onGUI() { }
		/* I had to add these two for when we are not rendering directly into a window (fi MultiViewManager). (SR)*/
		virtual void	onRender(const Viewport & vpRender) {  }
		virtual void	onUpdate(Input& input, const Viewport & vp);

		/* Used to mix with previous pass -- other function in the future ?*/
		virtual void	preRender(RenderTargetRGB& prev) {} ;


		virtual void		whichRT(uint i)			{ _whichRT=i; }
		virtual uint		whichRT(void)			{ return _whichRT; }


		void				setResolution(const Vector2i& size);
		const Vector2i&		getResolution( void ) const;


		bool				active() { return _active; }
		void				active(bool act) { _active = act; }

		void				setFocus(bool focus);
		bool				isFocused(void) const;

	protected:
		uint			_whichRT;
		std::vector<RenderTargetLum::Ptr>	_masks;

		bool			_active = true;
		Vector2i		_resolution;
		bool			_focus = false;

	};

} // namespace sibr

#endif // __SIBR_VIEW_ViewBase_HPP__
