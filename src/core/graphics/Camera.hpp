
#ifndef __SIBR_GRAPHICS_CAMERA_HPP__
# define __SIBR_GRAPHICS_CAMERA_HPP__

# include "core/graphics/Config.hpp"
# include "core/system/Transform3.hpp"


namespace sibr
{
	/**
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT Camera
	{
	public:
		typedef Transform3<float>		Transform3f;

	public:

		Camera( void ):
			_matViewProj(Matrix4f::Identity()), _invMatViewProj(Matrix4f::Identity()),
			_dirtyViewProj(true),_savePath(""), 
			_fov(70.f), _aspect(1.f), _znear(0.01f), _zfar(1000.f),_isOrtho(false), _p(0.5f, 0.5f) { }

		/////////////////////////////////////////////////////////////////
		///// ====================  Transform  ==================== /////
		/////////////////////////////////////////////////////////////////

		void					set( const Vector3f& translation, const Quaternionf& rotation );
		void					setLookAt( const Vector3f& Eye, const Vector3f& Center, const Vector3f& Up );

		void					translate( const Vector3f& v );

		// perform a translation with respect to frame ref
		void					translate( const Vector3f& v, const Transform3f& ref );

		void					position( const Vector3f& v );
		const Vector3f&			position( void ) const;

		// the action of performing the rotation, given the rotation
		void					rotate( const Quaternionf& rotation );
		void					rotate( const Vector3f& v );
		// perform a rotation with respect to frame ref
		void					rotate( const Vector3f& v, const Transform3f& ref );

		// set orientation values
		void					rotation( const Vector3f& v );
		void					rotation( const Quaternionf& q );
		const Quaternionf&		rotation( void ) const;

		/** Set transform */
		void 					transform( const Transform3f& t );
		/** Get transform */
		const Transform3f&		transform( void ) const;

		/////////////////////////////////////////////////////////////////
		///// ==================== Projection  ==================== /////
		/////////////////////////////////////////////////////////////////

		void				fovy( float value );
		float				fovy( void ) const;

		void				aspect( float value );
		float				aspect( void ) const;

		void				znear( float value );
		float				znear( void ) const;

		void				zfar( float value );
		float				zfar( void ) const;

		Vector3f			dir( void ) const;
		Vector3f			up( void ) const;
		Vector3f			right( void ) const;

		/** Project 3D point using perspective projection.
		* \param p 3D point
		* \returns pixel coordinates in [-1,1] range and depth in [-1,1]
		*/
		Vector3f			project( const Vector3f& point3d ) const;

		/** Back-project pixel coordinates and depth.
		* \param p pixel coordinates p[0],p[1] in [-1,1] and depth p[3] in [-1,1]
		* \returns 3D point
		*/
		Vector3f			unproject( const Vector3f& pixel2d ) const;

		void				perspective( float fovRad, float ratio, float znear, float zfar );

		bool				frustumTest(const Vector3f& position3d, const Vector2f& pixel2d) const; //pixel2d in [-1,1]
		bool				frustumTest(const Vector3f& position3d) const;

		Matrix4f			model( void ) const { return _transform.matrix(); }
		Matrix4f			view( void ) const { return _transform.invMatrix(); }
		Matrix4f			proj( void ) const;
		const Matrix4f&		viewproj( void ) const; /* cached */
		const Matrix4f&		invViewproj( void ) const; /* cached */

		/** \param p the principal point, expressed in [0,1] */
		void principalPoint(const sibr::Vector2f & p);

		static Camera		interpolate( const Camera& from, const Camera& to, float dist01 );

		void 				setStereoCam(bool isLeft, float, float);
		void				setOrthoCam(float right, float top);

		void				readViewProjMat(std::ifstream& infile) { infile >> _matViewProj; forceUpdateViewProj(); }
		void				saveViewProjMat(std::ofstream& outfile){ outfile << _matViewProj; };

		bool				needSave() const { return _savePath!=""; }
		std::string			savePath() const { return _savePath; }
		void				setSavePath(std::string savePath) { _savePath = savePath; }
		bool			_isOrtho;
		float			_right;
		float			_top;
	protected:
		void				forceUpdateViewProj( void ) const;

		std::string				_savePath;
		mutable Matrix4f		_matViewProj;
		mutable Matrix4f		_invMatViewProj;
		mutable bool			_dirtyViewProj;

		Transform3f		_transform;
		float			_fov;
		float			_aspect;
		float			_znear;
		float			_zfar;
		sibr::Vector2f   _p = {0.5f, 0.5};
		//bool			_isOrtho;
		//float			_right;
		//float			_top;
	};

	SIBR_GRAPHICS_EXPORT ByteStream&		operator << (ByteStream& stream, const Camera& c );
	SIBR_GRAPHICS_EXPORT ByteStream&		operator >> (ByteStream& stream, Camera& c );

	///// DEFINITIONS /////

	/////////////////////////////////////////////////////////////////
	inline const Transform3f&		Camera::transform( void ) const {
		return _transform;
	}

	inline void				Camera::set( const Vector3f& translation, const Quaternionf& rotation ) {
		_dirtyViewProj = true; _transform.set(translation, rotation);
	}

	inline void				Camera::setLookAt( const Vector3f& Eye, const Vector3f& At, const Vector3f& Up ) {
		const Vector3f zAxis( (Eye - At).normalized() );
		const Vector3f xAxis( (Up.normalized().cross(zAxis)).normalized() );
		const Vector3f yAxis( zAxis.cross(xAxis).normalized() );

		Eigen::Matrix3f rotation;
		rotation << xAxis, yAxis, zAxis;
		Quaternionf q(rotation);

		_transform.set(Eye,q);
		forceUpdateViewProj();
	}

	inline void				Camera::translate( const Vector3f& v ) {
		_dirtyViewProj = true; _transform.translate(v);
	}
	inline void				Camera::translate( const Vector3f& v, const Transform3f& ref ) {
		_dirtyViewProj = true; _transform.translate(v, ref);
	}
	inline void				Camera::position( const Vector3f& v ) {
		_dirtyViewProj = true; _transform.position(v);
	}
	inline const Vector3f&		Camera::position( void ) const {
		return _transform.position();
	}

	inline void					Camera::rotate( const Quaternionf& rotation ) {
		_dirtyViewProj = true; _transform.rotate(rotation);
	}
	inline void					Camera::rotate( const Vector3f& v ) {
		_dirtyViewProj = true; _transform.rotate(v);
	}
	inline void					Camera::rotate( const Vector3f& v, const Transform3f& ref ) {
		_dirtyViewProj = true; _transform.rotate(v, ref);
	}

	inline void					Camera::rotation( const Vector3f& v ) {
		_dirtyViewProj = true; _transform.rotation(v);
	}
	inline void					Camera::rotation( const Quaternionf& q ) {
		_dirtyViewProj = true; _transform.rotation(q);
	}

	inline const Quaternionf&		Camera::rotation( void ) const {
		return _transform.rotation();
	}

	/////////////////////////////////////////////////////////////////

	inline void	Camera::fovy( float value ) {
		_fov = value; _dirtyViewProj = true;
	}
	inline float	Camera::fovy( void ) const {
		return _fov;
	}

	inline void	Camera::aspect( float value ) {
		_aspect = value; _dirtyViewProj = true;
	}
	inline float	Camera::aspect( void ) const {
		return _aspect;
	}

	inline void	Camera::znear( float value ) {
		_znear = value; _dirtyViewProj = true;
	}
	inline float	Camera::znear( void ) const {
		return _znear;
	}

	inline void	Camera::zfar( float value ) {
		_zfar = value; _dirtyViewProj = true;
	}
	inline float	Camera::zfar( void ) const {
		return _zfar;
	}

	inline void Camera::principalPoint(const sibr::Vector2f & p) {
		_p = p; _dirtyViewProj = true;
	}

	inline const Matrix4f&			Camera::viewproj( void ) const {
		if (_dirtyViewProj)
			forceUpdateViewProj();

		return _matViewProj;
	}

	inline const Matrix4f&			Camera::invViewproj( void ) const {
		if (_dirtyViewProj)
			forceUpdateViewProj();

		return _invMatViewProj;
	}

} // namespace sibr

#endif // __SIBR_GRAPHICS_CAMERA_HPP__
