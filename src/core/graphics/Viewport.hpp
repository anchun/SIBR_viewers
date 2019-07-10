
#ifndef __SIBR_GRAPHICS_VIEWPORT_HPP__
# define __SIBR_GRAPHICS_VIEWPORT_HPP__

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"

namespace sibr
{

	/**
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT Viewport
	{
	public:
		Viewport( void ):
			_parent(nullptr),
			_left(0.f), _top(0.f), _right(1.f), _bottom(1.f) { }

		Viewport( float left, float top, float right, float bottom ) :
			_parent(nullptr),
			_left(left), _top(top), _right(right), _bottom(bottom) { }

		Viewport( const Viewport* parent_, float left, float top, float right, float bottom ) :
			_left(left), _top(top), _right(right), _bottom(bottom) { parent(parent_); }

		Viewport(const Viewport & parent_, float left, float top, float right, float bottom) :
			Viewport(&parent_, left, top, right, bottom) {
			*this = Viewport(finalLeft(), finalTop(), finalRight(), finalBottom());
		}

		inline float	left( void ) const { return _left; }
		inline float	top( void ) const { return _top; }
		inline float	right( void ) const { return _right; }
		inline float	bottom( void ) const { return _bottom; }

		inline float	width( void ) const { return _right-_left; }
		inline float	height( void ) const { return _bottom-_top; }

		float	finalLeft( void ) const;
		float	finalTop( void ) const;
		float	finalRight( void ) const;
		float	finalBottom( void ) const;

		float	finalWidth( void ) const;
		float	finalHeight( void ) const;
		
		sibr::Vector2f finalSize() const;
		Vector2f finalTopLeft() const;

		Vector2f pixAt(const Vector2f & uv) const;

		bool	contains( float x, float y ) const;
		bool	contains( int x, int y ) const;
		bool	contains(const Vector2f & xy) const;

		void			bind( uint screenWidth, uint screenHeight ) const;
		void			bind( void ) const; // tmp

		
		void			clear( const Vector3f& bgColor=Vector3f(0.f, 0.f, 0.f) ) const;

		// allow to track a window
		void				parent( const Viewport* view );
		const Viewport*		parent( void ) const;

		bool isEmpty() const;

	private:
		const Viewport*	_parent; ///< (optional)

		float	_left;
		float	_top;
		float	_right;
		float	_bottom;

	};

	///// DEFINITIONS /////

	inline void				Viewport::parent( const Viewport* view ) { 
		_parent = view; 

		//if (_parent == this) // means 'is the root'
		//	_parent = nullptr;
	}
	inline const Viewport*		Viewport::parent( void ) const { 
		return _parent; 
	}

	inline float	Viewport::finalLeft( void ) const {
		return (_parent)? (_parent->finalLeft() + _parent->finalWidth()*left()) : left();
	}

	inline float	Viewport::finalTop( void ) const {
		return (_parent)? ( _parent->finalTop() + _parent->finalHeight()*top() ) : top();
	}

	inline float	Viewport::finalRight( void ) const {
		return (_parent)? (_parent->finalLeft() + _parent->finalWidth()*right()) : right();
	}

	inline float	Viewport::finalBottom( void ) const {
		return (_parent)? (_parent->finalTop() + _parent->finalHeight()*bottom()) : bottom();
	}

	inline float	Viewport::finalWidth( void ) const {
		return (_parent)? _parent->finalWidth()*width() : width();
	}

	inline float	Viewport::finalHeight( void ) const {
		return (_parent)? _parent->finalHeight()*height() : height();
	}

	inline sibr::Vector2f	Viewport::finalSize(void) const {
		return sibr::Vector2f(finalWidth(),finalHeight());
	}

	inline Vector2f Viewport::finalTopLeft() const {
		return { finalLeft(), finalTop() };
	}


} // namespace sibr

#endif // __SIBR_GRAPHICS_VIEWPORT_HPP__
