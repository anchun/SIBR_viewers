
#ifndef __SIBR_FPSCOUNTER_HPP__
# define __SIBR_FPSCOUNTER_HPP__

# include <vector>
# include "core/view/Config.hpp"
# include <core/system/Vector.hpp>

# include <chrono>

namespace sibr
{

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT FPSCounter
	{
	public:
		typedef std::chrono::time_point<std::chrono::steady_clock> time_point;

		FPSCounter(const bool overlayed = true);

		void init(const sibr::Vector2f & position);

		void render();

		void update(float deltaTime);
		
		void update(bool doRender = true);

		void toggleVisibility() {
			_hidden = !_hidden;
		}

		bool active() const {
			return !_hidden;
		}

	private:
		time_point							_lastFrameTime;
		sibr::Vector2f						_position;
		std::vector<float>					_frameTimes;
		size_t								_frameIndex;
		float								_frameTimeSum;
		int									_flags;
		bool								_hidden;
	};

} // namespace sibr

#endif // __SIBR_FPSCOUNTER_HPP__

