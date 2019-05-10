
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
	class IBRView;

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT ViewBase
	{
	public:
		typedef std::shared_ptr<ViewBase> Ptr;
	public:

		/**
		 * Constructor.
		 *
		 * DevNote:
		 * At the beginning, we had IBRScene that was used to load
		 * and store data for a given scene.
		 * It has a disadvantage; all viewers do not need the same
		 * data, and we don't want to load/store everything in
		 * IBRScene. Futhermore, a new kind of 'IBRScene' is now
		 * comming: 'AssetStreamer'. It is used to stream data (not
		 * loading everything at the beginning, but continuously).
		 *   Maybe we should stop using IBRScene/AssetStreamer so
		 * high in the hierarchy of your classes.
		 *   That's why you can provide a nullptr to the following
		 * constructor; preserve backward compatibility but try
		 * to avoid it.
		 */
		//ViewBase( const IBRScene::Ptr& ibrScene=nullptr );
		ViewBase( const unsigned int w=720, const unsigned int h=480);

		virtual void	onUpdate(Input& /*input*/) { }
		virtual void	onRender( Window& /*win*/ )		{ }
		virtual void	onRenderIBR(IRenderTarget& /*dst*/, const Camera& /*eye*/) {};
		virtual void	onGUI() { }
		/* I had to add these two for when we are not rendering directly into a window (fi MultiViewManager). (SR)*/
		virtual void	onRender(const Viewport & viewport) {  }
		virtual void	onUpdate(Input& input, const Viewport & vp);

		/* Used to mix with previous pass -- other function in the future ?*/
		virtual void	preRender(RenderTargetRGB& prev) {} ;

		// If your execution encounter the following SIBR_ASSERT, read ctor's comment.
		/*const IBRScene&			scene( void ) const		{ SIBR_ASSERT(_old_scene != nullptr); return *_old_scene; }
		IBRScene&				scene( void )			{ SIBR_ASSERT(_old_scene != nullptr); return *_old_scene; }
		const IBRScene::Ptr&	getScenePtr( void )		{ SIBR_ASSERT(_old_scene != nullptr); return _old_scene; }*/

		virtual void		whichRT(uint i)			{ _whichRT=i; }
		virtual uint		whichRT(void)			{ return _whichRT; }


		void				setResolution(const Vector2i& size);
		const Vector2i&		getResolution( void ) const;


		bool				active() { return _active; }
		void				active(bool act) { _active = act; }

		void				setFocus(bool focus);
		bool				isFocused(void) const;

		/* set masks RTs for rendering using masks, note that default fragment shaders do not use masks */
		virtual void									setMasks( const std::vector<RenderTargetLum::Ptr>& masks );
		const std::vector<RenderTargetLum::Ptr>&		getMasks( void ) const;
		bool											useMasks( void ) const;

	protected:
		uint			_whichRT;
		std::vector<RenderTargetLum::Ptr>	_masks;

	protected:
		//IBRScene::Ptr	_old_scene;
		bool			_active;
		Vector2i		_resolution;
		bool			_focus;

	};

} // namespace sibr

#endif // __SIBR_VIEW_ViewBase_HPP__
