
#ifndef __SIBR_SYSTEM_VECTOR_HPP__
# define __SIBR_SYSTEM_VECTOR_HPP__

# include "core/system/Config.hpp"



namespace Eigen
{
	/**
	* \addtogroup sibr_graphics
	* @{
	*/

	// The following operators work with Eigen structs, so
	// they must be declared in the namespace Eigen (or
	// you would have to do sibr::operator < (left, right)
	// instead of simple left < right)

	template <typename T, int N, int Options>
	bool operator<(const Eigen::Matrix<T, N, 1, Options>& left, const Eigen::Matrix<T, N, 1, Options>& right) {

		for (int c = 0; c < N; c++) {
			if (left[c] < right[c]) return true;
			else if (left[c] > right[c]) return false;
		}
		return false; //case where they are equal

	}

	// stream

	template <typename T, int N, int Options>
	std::ostream& operator<<( std::ostream& s, const Eigen::Matrix<T, N, 1, Options>& t ) {
		s << '(';
		for (uint i=0; i<N; i++) { s << t[i]; if (i < N-1) s << ','; }
		s << ')';
		return (s);
	}

	template <typename T, int N, int Options>
	std::istream& operator>>( std::istream& s, Eigen::Matrix<T, N, 1, Options>& t ) {
		char tmp = 0;
		s >> tmp; // (
		for (int i = 0; i < N; ++i)
		{
			s >> t [i];
			s >> tmp; //, or )
		}

		return s;
	}

	/** @} */
}

namespace sibr
{

	/**
	* \addtogroup sibr_system
	* @{
	*/

	template <typename T, int N>
	using Vector = Eigen::Matrix<T, N, 1, Eigen::DontAlign>;

	// Return the fractional part
	template <typename T, int N, int Options>
	Eigen::Matrix<T, N, 1, Options>			frac( const Eigen::Matrix<T, N, 1, Options>& A ) {
		Eigen::Matrix<T, N, 1, Options> out = A;
		for (int i = 0; i < N; ++i)
			out[i] = out[i] - floor(out[i]);
		return out;
	}

	template <typename T, int N, int Options>
	inline T			distance( const Eigen::Matrix<T, N, 1, Options>& A, const Eigen::Matrix<T, N, 1, Options>& B ) {
		return (A-B).norm();
	}

	template <typename T, int N, int Options>
	inline T			length( const Eigen::Matrix<T, N, 1, Options>& A ) {
		return A.norm();
	}

	template <typename T, int N, int Options>
	inline T			sqLength( const Eigen::Matrix<T, N, 1, Options>& A ) {
		return A.squaredNorm();
	}

	template <typename T, int N, int Options>
	inline T			dot( const Eigen::Matrix<T, N, 1, Options>& A, const Eigen::Matrix<T, N, 1, Options>& B ) {
		return A.dot(B);
	}

	template <typename T, int N, int Options>
	inline Eigen::Matrix<T, N, 1, Options>	cross( const Eigen::Matrix<T, N, 1, Options>& A, const Eigen::Matrix<T, N, 1, Options>& B ) {
		return A.cross(B);
	}

	template <typename T, int N>
	inline Vector<T,N> clamp(const Vector<T, N>& A, const Vector<T, N> & min, const Vector<T, N> & max) {
		return A.cwiseMax(min).cwiseMin(max);
	}

	SIBR_SYSTEM_EXPORT Eigen::Matrix<float, 3,	1, Eigen::DontAlign>  toColorFloat( Vector<unsigned char, 3> & colorUB );

	SIBR_SYSTEM_EXPORT Eigen::Matrix<unsigned char, 3,1,Eigen::DontAlign> toColorUB( Vector<float,3> & colorFloat );
	
	typedef	Eigen::Matrix<float, 1,			1,Eigen::DontAlign>			Vector1f;
	typedef	Eigen::Matrix<int, 1,			1,Eigen::DontAlign>			Vector1i;

	typedef	Eigen::Matrix<unsigned, 2,		1,Eigen::DontAlign>			Vector2u;
	typedef	Eigen::Matrix<unsigned char, 2,	1,Eigen::DontAlign>			Vector2ub;
	typedef	Eigen::Matrix<int, 2,			1,Eigen::DontAlign>			Vector2i;
	typedef	Eigen::Matrix<float, 2,			1,Eigen::DontAlign>			Vector2f;
	typedef	Eigen::Matrix<double, 2,		1,Eigen::DontAlign>			Vector2d;

	typedef	Eigen::Matrix<unsigned, 3,		1,Eigen::DontAlign>			Vector3u;
	typedef	Eigen::Matrix<unsigned char, 3,	1,Eigen::DontAlign>			Vector3ub;
	typedef	Eigen::Matrix<unsigned short int, 3, 1,Eigen::DontAlign>	Vector3s;
	typedef	Eigen::Matrix<int, 3,			1,Eigen::DontAlign>			Vector3i;
	typedef	Eigen::Matrix<float, 3,			1,Eigen::DontAlign>			Vector3f;
	typedef	Eigen::Matrix<double, 3,		1,Eigen::DontAlign>			Vector3d;

	typedef	Eigen::Matrix<unsigned, 4,		1,Eigen::DontAlign>			Vector4u;
	typedef	Eigen::Matrix<unsigned char, 4,	1,Eigen::DontAlign>			Vector4ub;
	typedef	Eigen::Matrix<int, 4,			1,Eigen::DontAlign>			Vector4i;
	typedef	Eigen::Matrix<float, 4,			1,Eigen::DontAlign>			Vector4f;
	typedef	Eigen::Matrix<double, 4,		1,Eigen::DontAlign>			Vector4d;

	/**
		Return a 4x4 3D rotation matrix that align the first vector onto the second one.
		\param from source vector, current direction
		\param to destination vector, target direction
		\return the rotation matrix
	*/
	SIBR_SYSTEM_EXPORT Eigen::Matrix<float, 4, 4, Eigen::DontAlign> alignRotationMatrix(const sibr::Vector3f & from, const sibr::Vector3f & to);

	/** @} */
} // namespace sibr


#endif // __SIBR_GRAPHICS_VECTOR_HPP__
