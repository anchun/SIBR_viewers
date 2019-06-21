#ifndef POISSONRECONSTRUCTION_H
#define POISSONRECONSTRUCTION_H

#include "Config.hpp"
#include <core/graphics/Image.hpp>


namespace sibr {

	/** \brief Performs gradient integration fro tasks such as Poisson-based inpainting, smooth filling, ...
	 * \ingroup sibr_imgproc
	 */
	class SIBR_IMGPROC_EXPORT PoissonReconstruction
	{
	public:

		/**
			Initialize reconstructor for a given problem.
		\param gradientsX the RGB32F horizontal color gradients to integrate along
		\param gradientsY the RGB32F vertical color gradients to integrate along
		\param mask the L32F mask denoting how each pixel should be treated. 
				0 are pixels to be inpainted, 
				>0 are pixels to be used as source/constraint, 
				-1 are pixels to be left unchanged and unused.
		\param img_target the RGB32 image to use as a source constraint (will be copied internally)
		**/
		PoissonReconstruction(
			const cv::Mat3f & gradientsX,
			const cv::Mat3f & gradientsY,
			const cv::Mat1f & mask,
			const cv::Mat3f & img_target
		);

		/** Solve the reconstruction problem. */
		void solve(void);

		/** \return the result of the reconstruction */
		cv::Mat result() { return _img_target; }

		/** helper to get the pixel coordinates of valid pixels for agiven pixel and image size.
		 *\param pos the central pixel position
		 *\param width number of columns/width
		 *\param height number of rows/height
		 *\return a vector containing neighboring pixels coordinates.
		 */
		static std::vector< sibr::Vector2i > getNeighbors(sibr::Vector2i pos, int width, int height);

	private:
		cv::Mat _img_target;
		cv::Mat _gradientsX;
		cv::Mat _gradientsY;
		cv::Mat _mask;

		std::vector<sibr::Vector2i> _pixels;
		std::vector<sibr::Vector2i> _boundaryPixels;
		std::vector<int > _pixelsId;
		std::vector<std::vector<int> > _neighborMap;

		/* Parse the mask and the additional label condition into a list of pixels to modified and boundaries conditions. */
		void parseMask(void);

		/* Make sure that every modified pixel is connected to some boundary condition, all non connected pixels are discarded. */
		void checkConnectivity(void);

		/* Heuristic to fill isolated black pixels. */
		void postProcessing(void);

		/* Are we in the mask (ie mask==0). */
		bool isInMask(sibr::Vector2i & pos);
		/* Are we ignored (ie mask==-1). */
		bool isIgnored(sibr::Vector2i & pos);

	};

}

#endif // POISSONRECONSTRUCTION_H