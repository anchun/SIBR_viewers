
#ifndef __SIBR_SYSTEM_TRANSFORM3_HPP__
# define __SIBR_SYSTEM_TRANSFORM3_HPP__

# include "core/system/ByteStream.hpp"
# include "core/system/Config.hpp"
# include "core/system/Matrix.hpp"
# include "core/system/Vector.hpp"
# include "core/system/Quaternion.hpp"


namespace sibr
{
	/// \todo TODO: add scale if required

	/**
	* \ingroup sibr_graphics
	*/
	template <typename T>
	class Transform3
	{
	public:
		typedef Eigen::Matrix<T,3, 1, Eigen::DontAlign>		Vector3;
		typedef Eigen::Quaternion<T>						Quaternion;

	public:
		Transform3( void ) : _position(0, 0, 0) {
			_rotation.setIdentity();
		}

		void	       set( const Vector3& translation, const Quaternion& rotation ) {
				_position = translation;
				_rotation = rotation;
		}

		void				translate( float x, float y, float z );
		void				translate( float x, float y, float z,
											const Transform3& ref);
		void				translate( const Vector3& v );
		void				translate( const Vector3& v,
											const Transform3& ref );
		void				position( float x, float y, float z );
		void				position( const Vector3& v );

		const Vector3&	position( void ) const;


		/// Apply global rotation
		void					rotate( const Quaternion& rotation );
		void					rotate( float x, float y, float z );
		void					rotate( float x, float y, float z,
											const Transform3& ref);
		void					rotate( const Vector3& v );
		void					rotate( const Vector3& v, const Transform3& ref );

		void					rotation( float x, float y, float z );
		void					rotation( const Vector3& v );
		void					rotation( const Quaternion& q );

		const Quaternion&	rotation( void ) const;

		Matrix4f		matrix( void ) const;
		Matrix4f		invMatrix( void ) const;

		static Transform3<T>	interpolate( const Transform3<T>& from, const Transform3<T>& to, float dist01 ) {
			//_pos = (1.0f-k)*cs.pos() + k*ce.pos();
			dist01 = std::max(0.f, std::min(1.f, dist01)); // clamp
			
			Transform3<T> out;
			out.position((1.0f-dist01)*from.position() + dist01*to.position());
			//out.position((1.0f-dist01)*to.position() + dist01*from.position());
			out.rotation(from.rotation().slerp(dist01, to.rotation()));
			return out;
		}

		/** \todo Simple extrapolation based on delta transformation. TODO: add possibility to extrapolate with dist01 > 1*/
		static Transform3<T>	extrapolate(const Transform3<T>& previous, const Transform3<T>& current, float dist01) {

			Vector3f deltaPosition = current.position() - previous.position();
			Quaternion deltaRotation = previous.rotation().inverse() * current.rotation();

			Transform3<T> t = current;
			t.rotate(deltaRotation);
			t.translate(deltaPosition);
			return interpolate(current, t, dist01);
		}

		
		static Transform3<T>	computeFinal( const Transform3<T>& parentTr, const Transform3<T>& childTr ) {
			Transform3<T>		finalTr;
			finalTr.position(parentTr.position() + parentTr.rotation() * childTr.position());
			finalTr.rotation(parentTr.rotation() * childTr.rotation());
			return finalTr;
		}

	private:
		Vector3		    _position;
		Quaternion		_rotation;

	};

	typedef Transform3<float> Transform3f;
	
	template <typename T>
	ByteStream&		operator << (ByteStream& stream, const Transform3<T>& t ) {
		typename Transform3<T>::Vector3 v = t.position();
		typename Transform3<T>::Quaternion q = t.rotation();
		return stream
			<< v.x() << v.y() << v.z()
			<< q.x() << q.y() << q.z() << q.w();
	}

	template <typename T>
	ByteStream&		operator >> (ByteStream& stream, Transform3<T>& t ) {
		typename Transform3<T>::Vector3 v;
		typename Transform3<T>::Quaternion q;
		stream
			>> v.x() >> v.y() >> v.z()
			>> q.x() >> q.y() >> q.z() >> q.w();
		t.position(v);
		t.rotation(q);
		return stream;
	}

	//==================================================================//
	// Inlines
	//==================================================================//

	template <typename T>
	void		Transform3<T>::translate( float x, float y, float z ) {
		_position.x() += x; _position.y() += y; _position.z() += z;
	}

	template <typename T>
	void		Transform3<T>::translate( float x, float y, float z,
		const Transform3<T>& ref) {
			translate( Vector3( x, y, z ), ref );
	}

	template <typename T>
	void		Transform3<T>::translate( const Vector3& v ) {
		_position.x() += v.x(); _position.y() += v.y(); _position.z() += v.z();
	}

	template <typename T>
	void		Transform3<T>::translate( const Vector3& v, const Transform3& ref ) {
		translate( ref.rotation().operator*(v) );
	}

	template <typename T>
	void		Transform3<T>::position( float x, float y, float z ) {
		_position.x() = x; _position.y() = y; _position.z() = z;
	}

	template <typename T>
	void		Transform3<T>::position( const Vector3& v ) {
		_position.x() = v.x(); _position.y() = v.y(); _position.z() = v.z();
	}

	template <typename T>
	const typename Transform3<T>::Vector3&	Transform3<T>::position( void ) const {
		return _position;
	}


	template <typename T>
	void		Transform3<T>::rotate( const Quaternion& rotation ) {
		_rotation = rotation * _rotation;
		_rotation.normalize();
	}

	template <typename T>
	void		Transform3<T>::rotate( float x, float y, float z ) {
		Quaternion q = quatFromEulerAngles(Vector3(x, y, z));
		q.normalize();
		rotate(q);
	}

	template <typename T>
	void		Transform3<T>::rotate( const Vector3& v ) {
		rotate( v.x(), v.y(), v.z() );
	}

	template <typename T>
	void		Transform3<T>::rotate( const Vector3& v, const Transform3& ref ) {
		rotate( v.x(), v.y(), v.z(), ref );
	}

	template <typename T>
	void		Transform3<T>::rotation( float x, float y, float z ) {
		_rotation = quatFromEulerAngles(Vector3(x, y, z));
	}

	template <typename T>
	void		Transform3<T>::rotation( const Vector3& v ) {
		rotation( v.x(), v.y(), v.z() );
	}

	template <typename T>
	void		Transform3<T>::rotation( const Quaternion& q ) {
		_rotation = q;
	}

	template <typename T>
	const typename Transform3<T>::Quaternion&	Transform3<T>::rotation( void ) const {
		return _rotation;
	}

	template <typename T>
	Matrix4f Transform3<T>::matrix( void ) const {
		Matrix4f trans = matFromQuat(_rotation);
		trans = matFromTranslation(_position) * trans; // Opti (direct)
		return trans;
	}

	template <typename T>
	Matrix4f Transform3<T>::invMatrix( void ) const {
		// This is wrapped so we can (in the future) add a policy class
		// to enable caching this inv matrix
		return matrix().inverse();
	}

	template <typename T>
	void		Transform3<T>::rotate( float x, float y, float z,
		const Transform3<T>& ref)
	{
		Quaternion q = quatFromEulerAngles(Vector3(x, y, z));
		q.normalize();

		if ( &ref == this ) // Local Rotation
		{
			_rotation = _rotation * q;
			_rotation.normalize();
		}
		else
		{
			Quaternion refConj = ref.rotation();
			refConj.conjugate();

			// 1) Apply global rotation of ref on 'q' (ref * q)
			// 2) Apply local rotation of ref.conj (~inv) on 'q' (q*ref.conj)
			// 3) The rotation is converted and can be applied using rotate
			rotate((ref.rotation() * q) * refConj);
		}
	}


} // namespace sibr

#endif // __SIBR_GRAPHICS_TRANSFORM3_HPP__
