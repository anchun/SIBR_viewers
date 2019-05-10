
# include "core/view/ViewBase.hpp"

namespace sibr
{
	ViewBase::ViewBase(const unsigned int w, const unsigned int h)
	{
		_resolution = Vector2i(w, h);
	}

	void 	ViewBase::onUpdate(Input& input, const Viewport & vp) {
		onUpdate(input);
	}

	void				ViewBase::setResolution(const Vector2i& size)
	{
		_resolution = size;
	}

	const Vector2i&			ViewBase::getResolution( void ) const
	{
		return _resolution;
	}

	void				ViewBase::setFocus(bool focus)
	{
		_focus = focus;
	}

	bool				ViewBase::isFocused(void) const
	{
		return _focus;
	}



	void											ViewBase::setMasks( const std::vector<RenderTargetLum::Ptr>& masks )
	{
		_masks = masks;
	}

	const std::vector<RenderTargetLum::Ptr>&	ViewBase::getMasks( void ) const
	{
		return _masks;
	}

	bool											ViewBase::useMasks( void ) const
	{
		return _masks.empty() == false;
	}

} // namespace sibr
