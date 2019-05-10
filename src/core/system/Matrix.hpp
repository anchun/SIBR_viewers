/******************************************************************************

  Design Decision -- Eigen Integration:

    At the very beginning, we used vector/matrices from libminisl. Then we
  began to switch to Eigen's tools (because already used in lots of our code).
  Thus during the migration phase, we used custom class inheriting from
  Eigen::Matrix.
    However I encountered issues when executing the code under linux. It
  appears it was because of SSE instructions (special pipeline on CPU allowing
  to perform some vector operations in parallel). This SSE instruction required
  128-bit aligned memory. There are articles about this on Eigen website:
	eigen.tuxfamily.org/dox/group__DenseMatrixManipulation__Alignement.html
  
  But to summerize: keeping the alignment is difficult because you have to
  overload new operator in each class containing an Eigen::Matrix. Too unsafe,
  thus I disable this (Eigen::DontAlign) but missing assignment operators did
  that this consideration was ignored in some cases.
  E.g.: sibr::Matrix A, B, C;
  // ... // set A and B
  C = A*B; // A*B return a temporary class of Eigen but C didn't have the
  // assignment operator for this class [...] it wrongly considered it has an
  // not-aligned matrix and data was corrupted.

  Now SIBR uses a plugin system to extend Eigen classes:
  	eigen.tuxfamily.org/dox/TopicCustomizingEigen.html

  It's both safer and faster. (but it was not possible during the migration
  phase because I needed the child type to perfom automatic convertion with
  remaining libminisl tools).

******************************************************************************/

#ifndef __SIBR_SYSTEM_MATRIX_HPP__
# define __SIBR_SYSTEM_MATRIX_HPP__

# include <fstream>
# include "core/system/Config.hpp"
# include "core/system/Vector.hpp"


namespace sibr
{
	/**
	\addtogroup sibr_graphics
	@{
	*/
	typedef	Eigen::Matrix<unsigned, 4, 4, Eigen::DontAlign, 4, 4>		Matrix4u;
	typedef	Eigen::Matrix<int, 4, 4, Eigen::DontAlign, 4, 4>		Matrix4i;
	typedef	Eigen::Matrix<float, 4, 4, Eigen::DontAlign, 4, 4>	Matrix4f;
	typedef	Eigen::Matrix<double, 4, 4, Eigen::DontAlign, 4, 4>		Matrix4d;
	typedef	Eigen::Matrix<unsigned, 3, 3, Eigen::DontAlign, 3, 3>		Matrix3u;
	typedef	Eigen::Matrix<int, 3, 3, Eigen::DontAlign, 3, 3>		Matrix3i;
	typedef	Eigen::Matrix<float, 3, 3, Eigen::DontAlign, 3, 3>	Matrix3f;
	typedef	Eigen::Matrix<double, 3, 3, Eigen::DontAlign, 3, 3>		Matrix3d;

	template <typename T>
		Eigen::Matrix<T, 4, 4, 0, 4, 4> matFromQuat( const Eigen::Quaternion<T, 0>& q ) {
			Eigen::Matrix<T, 3, 3, 0, 3, 3> s = q.toRotationMatrix();

			Eigen::Matrix<T, 4, 4, 0, 4, 4> mat;
			mat <<
				s(0,0), s(0,1), s(0,2), 0,
				s(1,0), s(1,1), s(1,2), 0,
				s(2,0), s(2,1), s(2,2), 0,
				0, 0, 0, 1;
			return mat;
		}

	template <typename T, int Options>
		Eigen::Matrix<T, 4, 4, 0, 4, 4> matFromTranslation( const Eigen::Matrix<T, 3, 1, Options>& vec ) {

			Eigen::Matrix<T, 4, 4, 0, 4, 4> mat;
			mat.setIdentity();

			mat(0,3) = vec.x();
			mat(1,3) = vec.y();
			mat(2,3) = vec.z();
			return mat;
		}

	Matrix4f SIBR_SYSTEM_EXPORT perspective( float fovRadian, float ratio, float zn, float zf );

	Matrix4f SIBR_SYSTEM_EXPORT perspectiveOffCenter(
			float left, float right, float bottom, float top, float mynear, float myfar );

	Matrix4f SIBR_SYSTEM_EXPORT perspectiveStereo( float fovRadian, float aspect, float zn, float zf, float focalDistance,
			float eyeDistance, bool isLeftEye ); 

	Matrix4f SIBR_SYSTEM_EXPORT orthographic(float right, float top, float mynear, float myfar);

	Matrix4f SIBR_SYSTEM_EXPORT lookAt( const Vector3f& eye, const Vector3f& center, const Vector3f& up );


	void 	SIBR_SYSTEM_EXPORT operator<< (std::ofstream& outfile, const Matrix4f& m);

	void 	SIBR_SYSTEM_EXPORT operator>>( std::ifstream& infile, Matrix4f& out);

	/** }@ */
} // namespace sibr

#endif // __SIBR_GRAPHICS_MATRIX_HPP__

