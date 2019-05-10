
#ifndef __SIBR_VECTOR_UTILS_HPP__
# define __SIBR_VECTOR_UTILS_HPP__

# include <vector>
# include "core/system/Config.hpp"
#include <functional>

namespace sibr
{
	/**
	* \addtogroup sibr_system
	* @{
	*/

	template<typename T_in, typename T_out = T_in> T_out sum(
		const std::vector<T_in> & vec ,
		const std::function<bool(T_in)> & f = [](T_in val) { return true; }
	) {
		double sum = 0;
		for (T_in val : vec) {
			if( f(val) ){
				sum += (double)val;
			}
		}
		return (T_out)sum;
	}

	template<typename T_in, typename T_out = T_in> std::vector<T_out> weighted_normalization(
		const std::vector<T_in> & vec,
		const std::vector<T_in> & weights,
		const std::function<bool(T_in)> & f = [](T_in val) { return true; }
	) {
		double sum = 0;
		int size = (int)std::min(vec.size(), weights.size());
		for (int i = 0; i < size; ++i) {
			T_in val = vec[i];
			if (f(val)) {
				sum += (double)val*(double)weights[i];
			}
		}

		std::vector<T_out> out(size);
		for (int i = 0; i < size; ++i) {
			if ((sum == 0) || !f(vec[i])) {
				out[i] = (T_out)vec[i];
			} else {
				out[i] = (T_out)( ( (double)vec[i] * (double)weights[i] )/sum );
			}
		}
			
		return out;
	}

	template<typename T_in, typename T_out = T_in> std::vector<T_out> applyLambda(
		const std::vector<T_in> & vec,
		const std::function<T_out(T_in)> & f
	) {
		std::vector<T_out> out(vec.size());
		for (int i = 0; i < vec.size(); ++i) {
			out[i] = f(vec[i]);
		}
		return out;
	}
	
	template<typename T_in, typename T_out> std::vector<T_out> applyLambda(
		const std::vector<T_in> & vec1,
		const std::vector<T_in> & vec2,
		const std::function<T_out(T_in,T_in)> & f
	) {
		int size = (int)std::min(vec1.size(), vec2.size());
		std::vector<T_out> out(size);
		for (int i = 0; i < size; ++i) {
			out[i] = f(vec1[i],vec2[i]);
		}
		return out;
	}

	template<typename T_in, typename T_out = T_in> T_out var(
		const std::vector<T_in> & vec,
		const std::function<bool(T_in)> & f = [](T_in val) { return true; } 
	) {
		double sum = 0;
		double sum2 = 0;
		int n = 0;

		for (T_in val : vec) {
			if ( f(val) ) {
				sum += (double)val;
				sum2 += (double)val*(double)val;
				++n;
			}
		}

		if (n < 2) {
			return (T_out)(-1);
		}
		else {
			return (T_out)((sum2 - sum*sum / (double)n) / double(n - 1));
		}
		
	}
	
	template<typename T_in, typename T_out = T_in> std::vector<T_out> normalizedMinMax(
		const std::vector<T_in> & vec,
		const std::function<bool(T_in)> & f = [](T_in val) { return true; }
	) {
		T_in min = 0, max = 0;
		bool first = true;
		for (T_in val : vec) {
			if (f(val)) {
				if (first || val > max) {
					max = val;
				}
				if (first || val < min) {
					min = val;
				}
				first = false;
			}
		}

		//std::cout << " min max " << min << " " << max << std::endl;

		if (min == max) {
			return std::vector<T_out>();
		}
		else {
			std::vector<T_out> out(vec.size());
			double normFactor = 1.0 / (double)(max - min);
			for (int i = 0; i < (int)vec.size(); ++i) {
				out[i] = f(vec[i]) ? (T_out)((double)(vec[i] - min)*normFactor) : (T_out)vec[i];
			}
			return out;
		}
	}

	template<typename T_in, typename T_out = T_in, unsigned int Power = 2> std::vector<T_out> normalizedZeroOne(
		const std::vector<T_in> & vec,
		const std::function<bool(T_in)> & f = [](T_in val) { return true; }
	) {
		double sumP = 0;

		for (T_in val : vec) {
			if (f(val)) {
				sumP += std::pow((double)val, Power);
			}
			
		}

		if (sumP == 0) {
			return std::vector<T_out>();
		}
		else {
			std::vector<T_out> out(vec.size());
			for (int i = 0; i <(int)vec.size(); ++i) {
				out[i] = f(vec[i]) ? (T_out)(vec[i] / sumP) : (T_out)vec[i];
			}
			return out;
		}
			
	}

	/*** @} */

	/**
	* \ingroup sibr_system
	*/
	template< typename T, unsigned int N >
	class MultiVector : public std::vector< MultiVector<T, N - 1> >
	{
		static_assert(N >= 1, " MultiVector<N> : the number of dimensions N must be >= 1 ");

		friend class MultiVector<T, N + 1>;

		typedef MultiVector<T, N - 1> SubVector;

	public:
		MultiVector() {}

		MultiVector(int n, const T & t = T() )
			: std::vector< SubVector >(n, SubVector(n, t)) { }

		MultiVector(const std::vector<int> & dims, const T & t = T() )
			: std::vector< SubVector >(dims.at(dims.size()-N), SubVector(dims, t)) { }

		//template<unsigned int M>
		//MultiVector<T, N - M> & slice(const std::vector<int> & ids)
		//{
		//	static_assert(N-M >= 1 && M>=0, " MultiVector<N> : slice<M>() : 0 <= M <= N-1 mandatory ");
		//	return at(ids.at(ids.size() - M)).slice<M-1>(ids);
		//}

		T & multiAt(const std::vector<int> & ids) {
			return at(ids.at(ids.size() - N)).multiAt(ids);
		}
		
		const T & multiAt(const std::vector<int> & ids) const {
			return at(ids.at(ids.size() - N)).multiAt(ids);
		}

		std::vector<int> dims() const
		{
			std::vector<int> v;
			dimsRecur(v);
			return v;
		}
		
		void dimsDisplay() const {
			std::vector<int> d(dims());
			std::cout << " [ ";
			for (int i = 0; i < N; ++i) {
				std::cout << d[i] << (i != N - 1 ? " x " : "");
			}
			std::cout << " ] " << std::endl;
		}

	protected:
		void dimsRecur(std::vector<int> & v) const
		{
			v.push_back((int)size());
			at(0).dimsRecur(v);
		}
	};

	//template<typename T, unsigned int N>
	//template<>
	//MultiVector<T, N> & MultiVector<T, N>::slice<0>(const std::vector<int> & ids)
	//{
	//	return (*this);
	//}

	template<typename T>
	class MultiVector<T, 1> : public std::vector<T>
	{
		friend class MultiVector<T, 2>;

	public:
		MultiVector() {}

		MultiVector(int n, const T & t = T() )
			: std::vector<T>(n, t) { }

		MultiVector(const std::vector<int> & dims, const T & t = T())
			: std::vector<T>(dims.at(dims.size()-1), t) { }
		
		//template<unsigned int M>
		//MultiVector<T, 1 - M> & slice(const std::vector<int> & ids)
		//{
		//	return (*this);
		//}

		T & multiAt(const std::vector<int> & ids) {
			return at(ids.at(ids.size() - 1));
		}

		const T & multiAt(const std::vector<int> & ids) const {
			return at(ids.at(ids.size() - 1));
		}

		void dimsDisplay() const {
			std::cout << " [ " << size() << " ] " << std::endl;
		}

	protected:
		void dimsRecur(std::vector<int> & v) const
		{
			v.push_back((int)size());
		}

	};

	
} // namespace sibr

#endif // __SIBR_VECTOR_UTILS_HPP__
