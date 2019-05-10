
#ifndef __SIBR_SIMPLE_TIMER_HPP__
# define __SIBR_SIMPLE_TIMER_HPP__

# include "core/system/Config.hpp"
#include <vector>
#include <chrono>

namespace sibr
{
	/**
	* \ingroup sibr_system
	*/
	class Timer
	{
	public:
		typedef std::chrono::time_point<std::chrono::steady_clock> time_point;
		typedef std::chrono::nanoseconds nano;
		typedef std::chrono::microseconds micro;
		typedef std::chrono::milliseconds milli;
		typedef std::chrono::seconds s;

		const double timeResolution = (double)std::chrono::high_resolution_clock::period::num
			/ std::chrono::high_resolution_clock::period::den;

		Timer(bool start_now = false ) : hasStarted(false)
		{
			if (start_now) {
				tic();
			}
		}

		Timer(const Timer & timer) {
			hasStarted = timer.hasStarted;
			current_tic = timer.current_tic;
		}

		void tic()
		{
			tocs.resize(0);
			hasStarted = true;
			current_tic = std::chrono::high_resolution_clock::now();
		}

		void toc()
		{
			auto toc = std::chrono::high_resolution_clock::now();
			tocs.push_back(toc);
		}

		template<typename T = Timer::milli>
		double deltaTimeFromLastTic() const
		{
			if (!hasStarted) { return std::numeric_limits<double>::max(); }
			auto toc = std::chrono::high_resolution_clock::now();
			double deltaTime = 1;
			if (!getDeltaTime<T>(current_tic, toc, deltaTime)) {
				std::cout << "[SIBR - Timer] : below time reslution " << std::endl;
			}

			return deltaTime;
		}

		template<typename T = Timer::milli>
		void display(bool tac_now = false)
		{
			if (tac_now) {
				toc();
			}
			int n = (int)tocs.size();
			if (!hasStarted || n == 0) {
				std::cout << "[SIBR - Timer] : no tic or no toc" << std::endl;
			}
			else {
				double deltaTime;
				for (auto & toc : tocs) {
					if (getDeltaTime<T>(current_tic,toc,deltaTime) ) {
						std::cout << "[SIBR - Timer] : " << deltaTime << std::endl;
					}
					else {
						std::cout << "[SIBR - Timer] : below time reslution " << std::endl;
					}
				}
			}
		}

		template<typename T = Timer::milli>
		bool getDeltaTime(const time_point & tic, const time_point & toc, double & deltaTime) const {
			double timediff_nanoSeconds = (double)std::chrono::duration_cast<Timer::nano>(toc - tic).count();
			if (timediff_nanoSeconds < Timer::timeResolution) {
				return false;
			}
			else {
				deltaTime = (double)std::chrono::duration_cast<T>(toc - tic).count();
				return true;
			}
		}

	private:
		time_point current_tic;
		std::vector<time_point> tocs;
		bool hasStarted;
	};
	

} // namespace sibr

#endif // __SIBR_SIMPLE_TIMER_HPP__
