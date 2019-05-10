
#ifndef __SIBR_SYSTEM_QUATERNION_HPP__
# define __SIBR_SYSTEM_QUATERNION_HPP__

# include <Eigen/Core>
# include <Eigen/Geometry>
# include "core/system/Config.hpp"
# include "core/system/Matrix.hpp"
# include "core/system/Vector.hpp"

namespace sibr
{

	// Clarify if this function is equivalent to sibr::Quaternion(rotationmatrix);
	// In my experience it was not the case (SR).
	template <typename T, int Options>
	Eigen::Quaternion<T, 0>	quatFromMatrix(const Eigen::Matrix<T, 3, 3, Options, 3, 3>& m) {
		Eigen::Quaternion<T, 0> q;
		float trace = m(0, 0) + m(1, 1) + m(2, 2) + 1.f;
		if (trace > 0)
		{
			float s = 0.5f / sqrtf(trace);
			q.x() = (m(1, 2) - m(2, 1)) * s;
			q.y() = (m(2, 0) - m(0, 2)) * s;
			q.z() = (m(0, 1) - m(1, 0)) * s;
			q.w() = 0.25f / s;
		}
		else
		{
			if ((m(0, 0) > m(1, 1)) && (m(0, 0) > m(2, 2)))
			{
				float s = sqrtf(1.f + m(0, 0) - m(1, 1) - m(2, 2)) * 2.f;
				q.x() = 0.5f / s;
				q.y() = (m(1, 0) + m(0, 1)) / s;
				q.z() = (m(2, 0) + m(0, 2)) / s;
				q.w() = (m(2, 1) + m(1, 2)) / s;
			}
			else if (m(1, 1) > m(2, 2))
			{
				float s = sqrtf(1.f - m(0, 0) + m(1, 1) - m(2, 2)) * 2.f;
				q.x() = (m(1, 0) + m(0, 1)) / s;
				q.y() = 0.5f / s;
				q.z() = (m(2, 1) + m(1, 2)) / s;
				q.w() = (m(2, 0) + m(0, 2)) / s;
			}
			else
			{
				float s = sqrtf(1.f - m(0, 0) - m(1, 1) + m(2, 2)) * 2.f;
				q.x() = (m(2, 0) + m(0, 2)) / s;
				q.y() = (m(2, 1) + m(1, 2)) / s;
				q.z() = 0.5f / s;
				q.w() = (m(1, 0) + m(0, 1)) / s;
			}
		}
		return q;
	}

	template <typename T, int Options>
	Eigen::Quaternion<T, 0>	quatFromMatrix( const Eigen::Matrix<T, 4,4, Options, 4,4>& m ) {
		Eigen::Quaternion<T, 0> q;
		float trace = m(0, 0) + m(1, 1) + m(2, 2) + 1.f;
		if (trace > 0)
		{
			float s = 0.5f / sqrtf(trace);
			q.x() = (m(1, 2) - m(2, 1)) * s;
			q.y() = (m(2, 0) - m(0, 2)) * s;
			q.z() = (m(0, 1) - m(1, 0)) * s;
			q.w() = 0.25f / s;
		}
		else
		{
			if ((m(0, 0) > m(1, 1)) && (m(0, 0) > m(2, 2)))
			{
				float s = sqrtf(1.f + m(0, 0) - m(1, 1) - m(2, 2)) * 2.f;
				q.x() = 0.5f / s;
				q.y() = (m(1, 0) + m(0, 1)) / s;
				q.z() = (m(2, 0) + m(0, 2)) / s;
				q.w() = (m(2, 1) + m(1, 2)) / s;
			}
			else if (m(1, 1) > m(2, 2))
			{
				float s = sqrtf(1.f - m(0, 0) + m(1, 1) - m(2, 2)) * 2.f;
				q.x() = (m(1, 0) + m(0, 1)) / s;
				q.y() = 0.5f / s;
				q.z() = (m(2, 1) + m(1, 2)) / s;
				q.w() = (m(2, 0) + m(0, 2)) / s;
			}
			else
			{
				float s = sqrtf(1.f - m(0, 0) - m(1, 1) + m(2, 2)) * 2.f;
				q.x() = (m(2, 0) + m(0, 2)) / s;
				q.y() = (m(2, 1) + m(1, 2)) / s;
				q.z() = 0.5f / s;
				q.w() = (m(1, 0) + m(0, 1)) / s;
			}
		}
		return q;
	}

	template <typename T, int Options>
	Eigen::Quaternion<T, 0>	quatFromEulerAngles( const Eigen::Matrix<T, 3, 1,Options>& deg ) {
		Vector3f v(SIBR_DEGTORAD(deg.x()), SIBR_DEGTORAD(deg.y()), SIBR_DEGTORAD(deg.z()));
		Vector3f halfAngles( v.x() * 0.5f, v.y() * 0.5f, v.z() * 0.5f );

		const float cx = cosf (halfAngles.x());
		const float sx = sinf (halfAngles.x());
		const float cy = cosf (halfAngles.y());
		const float sy = sinf (halfAngles.y());
		const float cz = cosf (halfAngles.z());
		const float sz = sinf (halfAngles.z());

		const float cxcz = cx*cz;
		const float cxsz = cx*sz;
		const float sxcz = sx*cz;
		const float sxsz = sx*sz;

		Eigen::Quaternion<T, 0> dst;
		dst.vec().x() = (cy * sxcz) - (sy * cxsz);
		dst.vec().y() = (cy * sxsz) + (sy * cxcz);
		dst.vec().z() = (cy * cxsz) - (sy * sxcz);
		dst.w() = (cy * cxcz) + (sy * sxsz);
		return dst;
	}

	template <typename T, int Options>
	Eigen::Matrix<T, 3, 1, Options>	quatRotateVec(
		const Eigen::Quaternion<T, 0>& rotation, const Eigen::Matrix<T, 3, 1, Options>& vec ) {
		return rotation._transformVector(vec); // why '_' in this function name ? I don't have any trust
		// in the longivity of this function so I have done this function wrapper
	}

	template <typename T>
	inline static Eigen::Quaternion<T> dot( const Eigen::Quaternion<T>& q1, const Eigen::Quaternion<T>& q2 ) {
		return q1.vec().dot(q2.vec()) + q1.w()*q2.w();
	}

	template <typename T>
	inline static float		angleRadian( const Eigen::Quaternion<T>& q1, const Eigen::Quaternion<T>& q2 ) {
		const float mid = 3.14159f;
		float angle = q1.angularDistance(q2);
		return angle > mid? mid-angle : angle; // be sure to return the shortest angle
	}

    /// linear quaternion interpolation
	template <typename T>
    inline static Eigen::Quaternion<T> lerp( const Eigen::Quaternion<T>& q1, const Eigen::Quaternion<T>& q2, float t ) {
		return (q1*(1-t) + q2*t).normalized();
	}

	/// spherical linear interpolation
	template <typename T>
	static Eigen::Quaternion<T> slerp( const Eigen::Quaternion<T>& q1, const Eigen::Quaternion<T>& q2, float t ) {
		Eigen::Quaternion<T> q3;
		float dot = q1.dot(q2);// Eigen::Quaternion<T>::dot(q1, q2);
		// dot = cos(theta)
		// 	 if (dot < 0), q1 and q2 are more than 90 degrees apart,
		// 	 so we can invert one to reduce spinning
		if (dot < 0)
		{
			dot = -dot;
			q3 = -q2;
		} else q3 = q2;
		if (dot < 0.95f)
		{
			float angle = acosf(dot);
			return (q1*sinf(angle*(1-t)) + q3*sinf(angle*t))/sinf(angle);
		} else // if the angle is small, use linear interpolation
			return lerp(q1,q3,t);
	}

	typedef	Eigen::Quaternion<unsigned>		Quaternionu;
	typedef	Eigen::Quaternion<int>			Quaternioni;
	typedef	Eigen::Quaternion<float>		Quaternionf;
	typedef	Eigen::Quaternion<double>		Quaterniond;

} // namespace sibr

#endif // __SIBR_GRAPHICS_QUATERNION_HPP__
