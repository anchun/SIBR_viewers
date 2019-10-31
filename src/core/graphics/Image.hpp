#ifndef __SIBR_GRAPHICS_IMAGE_HPP__
# define __SIBR_GRAPHICS_IMAGE_HPP__

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/system/ByteStream.hpp"

# pragma warning(push, 0)
#  include <opencv2/core/core.hpp>
#  include <opencv2/imgproc/imgproc.hpp>
#  include <opencv2/highgui/highgui.hpp>
#  include <boost/filesystem.hpp>
# pragma warning(pop)


namespace cv
{
	template <typename T_Type, int cn>
	class DataType<Eigen::Matrix<T_Type, cn, 1, Eigen::DontAlign> >
	{
	public:
		typedef Eigen::Matrix<T_Type, cn, 1, Eigen::DontAlign> value_type;
		typedef Eigen::Matrix<typename DataType<T_Type>::work_type, cn, 1, Eigen::DontAlign> work_type;
		typedef T_Type channel_type;
		typedef value_type vec_type;
		enum { generic_type = 0, depth = DataDepth<channel_type>::value, channels = cn, fmt = ((channels - 1) << 8) + DataDepth<channel_type>::fmt, type = CV_MAKETYPE(depth, channels) };
	};
}

namespace sibr
{
	namespace opencv
	{
		template <typename T_Type>
		SIBR_GRAPHICS_EXPORT int		imageType(void);// { return -1; } // default, unknown

		template <typename T_Type>
		inline float			imageTypeRange(void) {
			return (float)std::numeric_limits<T_Type>::max();//-std::numeric_limits<T_Type>::min();
		}

		template <> SIBR_GRAPHICS_EXPORT inline int		imageType< uint8 >(void) { return CV_8U; }
		template <> SIBR_GRAPHICS_EXPORT inline int		imageType< int8  >(void) { return CV_8S; }
		template <> SIBR_GRAPHICS_EXPORT inline int		imageType< uint16>(void) { return CV_16U; }
		template <> SIBR_GRAPHICS_EXPORT inline int		imageType< int16 >(void) { return CV_16S; }
		template <> SIBR_GRAPHICS_EXPORT inline int		imageType< int32 >(void) { return CV_32S; }
		template <> SIBR_GRAPHICS_EXPORT inline int		imageType< float >(void) { return CV_32F; }
		template <> SIBR_GRAPHICS_EXPORT inline int		imageType< double>(void) { return CV_64F; }

		template <> SIBR_GRAPHICS_EXPORT inline float			imageTypeRange< float >(void) { return 1.f; }
		template <> SIBR_GRAPHICS_EXPORT inline float			imageTypeRange< double>(void) { return 1.f; }

		SIBR_GRAPHICS_EXPORT float			imageTypeCVRange(int cvDepth);

		SIBR_GRAPHICS_EXPORT void			convertBGR2RGB(cv::Mat& dst);
		SIBR_GRAPHICS_EXPORT void			convertRGB2BGR(cv::Mat& dst);
	}

	typedef	Vector4f			ColorRGBA;

	/**
	* Interface virtual class for all the templated image classes
	* Contains all functions not making reference to the internal type or numComp in their signature/return type
	* see Image class to know what these functions actually do
	*/
	class IImage {
	public:
		SIBR_CLASS_PTR(IImage);

		virtual bool			load(const std::string& filename, bool verbose = true, bool warning_if_not_found = true) = 0;
		virtual bool			loadByteStream(const std::string& filename, bool verbose = true) = 0;

		virtual void			save(const std::string& filename, bool verbose = true) const = 0;
		virtual void			saveByteStream(const std::string& filename, bool verbose = true) const = 0;

		virtual uint			w(void) const = 0;
		virtual uint			h(void) const = 0;
		virtual sibr::Vector2u	size(void) const = 0;

		virtual bool			isInRange(const sibr::Vector2i & xy)  const = 0;

		virtual std::string		pixelStr(const sibr::Vector2i & xy)  const = 0;

		virtual uint			numComp(void) const = 0;
		virtual uint			sizeOfComp(void) const = 0;

		virtual void			flipH(void) = 0;
		virtual void			flipV(void) = 0;

		virtual int				opencvType(void) const = 0;
		virtual const cv::Mat&	toOpenCV(void) const = 0;
		virtual cv::Mat&		toOpenCVnonConst(void) = 0;
		virtual void			fromOpenCV(const cv::Mat& img) = 0;
		virtual cv::Mat			toOpenCVBGR(void) const = 0;
		virtual void			fromOpenCVBGR(const cv::Mat& img) = 0;


	};



	template<typename T_Type, unsigned int T_NumComp>
	class ImagePtr;

	/**
	* This class is used to store images. Internally, a cv::Mat
	* is used. The template parameter define a fixed size/format that
	* will be used to convert automatically the image format when
	* you load or copy from another image.
	* Note that OpenCV uses generally BGR channels (e.g. after loading
	* an image file). However the internal cv::Mat of this class stores
	* RGB channels. You can get RGB cv::Mat with toOpenCV() or use
	* toOpenCVBGR(). (Most of OpenCV's features works with RGB too but
	* not imshow, imwrite, imread.)
	\ingroup sibr_graphics
	*/
	template<typename T_Type, unsigned int T_NumComp>
	class /*SIBR_GRAPHICS_EXPORT*/ Image : public IImage {
	public:
		typedef T_Type						Type;
		//SIBR_CLASS_PTR(Image);
		typedef ImagePtr<T_Type, T_NumComp> Ptr;

		//typedef std::unique_ptr<classname>	UPtr;
		typedef Eigen::Matrix<T_Type, T_NumComp, 1, Eigen::DontAlign>	Pixel; // RGB
		enum { e_NumComp = T_NumComp };

	public:
		Image(void);
		Image(uint width, uint height);
		Image(uint width, uint height, const T_Type& init);
		Image(uint width, uint height, const Pixel& init);
		// Image( const std::string& filename ); // <== I don't recommand this (how do we check result?)

		// Copy is not authorized, this is a move ctor
		Image(Image&& other);
		Image& operator=(Image&& other);

		bool		load(const std::string& filename, bool verbose = true, bool warning_if_not_found = true);
		bool		loadByteStream(const std::string& filename, bool verbose = true);
		/// ATTENTION: if you try to save an image with a channel depth
		/// that is not managed by a file format (e.g. saving 32F to .jpeg),
		/// you will porbably get weird pixel.
		/// TODO: try to print an error for this.
		void		save(const std::string& filename, bool verbose = true) const;
		void		saveHDR(const std::string& filename, bool verbose = true) const;
		void		saveByteStream(const std::string& filename, bool verbose = true) const;

		/*const Pixel&	pixel(uint x, uint y) const;
		Pixel&			pixel(uint x, uint y);
		const Pixel&	pixel(const sibr::Vector2i & xy) const;
		Pixel&			pixel(const sibr::Vector2i & xy);*/

		const Pixel&	operator()(uint x, uint y) const;
		Pixel&			operator()(uint x, uint y);
		const Pixel&	operator()(const sibr::Vector2i & xy) const;
		Pixel&			operator()(const sibr::Vector2i & xy);
		const Pixel&	operator()(const sibr::Vector2f & xy) const;
		Pixel&			operator()(const sibr::Vector2f & xy);

		virtual std::string		pixelStr(const sibr::Vector2i & xy)  const;

		void			pixel(uint x, uint y, const Pixel& p); /// \todo TODO: should be removed
		const void*		data(void) const;
		void*			data(void);

		ColorRGBA	 color(uint x, uint y) const;
		void		 color(uint x, uint y, const ColorRGBA& c);
		static Pixel color(const ColorRGBA& rgba);

		/// Return a resized version of the current image
		Image		resized(int width, int height, int cv_interpolation_method = cv::INTER_LINEAR) const;
		/// Return a resized version of the current image so that
		/// the maximum length (either width or height) is now equal
		/// to \param maxlen. Keep original ratio.
		/// e.g.: src is 2048x1024
		///          resizedMax(1024) -> dst is 1024x512
		Image		resizedMax(int maxlen) const;

		Image		clone(void) const;

		ImagePtr<T_Type, T_NumComp>	  clonePtr(void) const;

		/// Image sizes
		/// Note that 'width' and 'height' would be more coherent than
		/// just 'w' and 'h'...
		uint			w(void) const;
		uint			h(void) const;
		sibr::Vector2u size(void) const;
		template <typename T>
		bool			isInRange(T x, T y) const { return (x >= 0 && y >= 0 && x < (T)w() && y < (T)h()); }
		template <typename T>
		bool			inRange(T x, T y) const { return (x >= 0 && y >= 0 && x < (T)w() && y < (T)h()); }
		bool			isInRange(const sibr::Vector2i & xy)  const { return (xy.x() >= 0 && xy.y() >= 0 && xy.x() < (int)w() && xy.y() < (int)h()); }
		bool			isInRange(const sibr::Vector2f & xy)  const { return (xy.x() >= 0 && xy.y() >= 0 && xy.x() < (float)w() && xy.y() < (float)h()); }

		uint		numComp(void) const;
		uint		sizeOfComp(void) const;

		/// Flip horizontaly
		void		flipH(void);
		/// Flip vertically
		void		flipV(void);

		int				opencvType(void) const { return CV_MAKETYPE(opencv::imageType<T_Type>(), T_NumComp); }
		const cv::Mat&	toOpenCV(void) const { return _pixels; }
		cv::Mat&		toOpenCVnonConst(void) { return _pixels; }
		void			fromOpenCV(const cv::Mat& img);
		cv::Mat			toOpenCVBGR(void) const;
		void			fromOpenCVBGR(const cv::Mat& img);

		// compatibility functions from libsl
		void findMinMax(Pixel&, Pixel&);
		void remap(const Pixel&, const Pixel&);

		/// Cast into another image type
		template<class T_Image> T_Image cast() const {
			T_Image b;
			b.fromOpenCV(toOpenCV());
			return b;
		}

		/// bilinear interpolation color from a floating 2d position
		/// \param pixel query position in [0,w[x[0,h[
		Pixel bilinear(const sibr::Vector2f & pixel) const;

		///static helper for bicubic function
		static Eigen::Matrix<float, T_NumComp, 1, Eigen::DontAlign> monoCubic(float t, const Eigen::Matrix<float, T_NumComp, 4, Eigen::DontAlign> & colors);

		/// bicubic interpolation color from a floating 2d position
		/// \param pixelPosition query position in [0,w[x[0,h[
		Pixel bicubic(const sibr::Vector2f & pixelPosition) const;

	private:
		//Image&		operator =( const cv::Mat& img ); // deprecated, use fromOpenCV() instead

		// I though that opencv was doing something clever when copying images (only duplicate
		// pixels if we modify them) but no, they just point to the same place. We should use
		// shared_ptr to do this (or unique_ptr if you can).
		// Note you can also use no pointer at all and even use it in STL container such as
		// std::vector. If you do that and use a such vector as a class member, then disallow
		// the copy of your class using SIBR_DISALLOW_COPY (see example in sibr/view/IBRScene).
		// Visual Studio is too stupid to automatically disallow copy of your class and will try
		// to generate copy ctor/ copy operator even if you don't use them (-_-'). In this case
		// use SIBR_DISALLOW_COPY to specify you clearly don't want to generate them.
		//		Image( const Image& );					// Disallowed
		Image& 		operator =(const Image&);	// Disallowed

	protected:
		cv::Mat			_pixels;	///< pixels stored in RGB format
	};

	template<typename T_Type, unsigned int T_NumComp>
	class ImagePtr {
	public:
		
		using ImageType = Image<T_Type, T_NumComp>;

		std::shared_ptr<Image<T_Type, T_NumComp>> imPtr;
		
		ImagePtr() { imPtr = std::shared_ptr<Image<T_Type, T_NumComp>>(); };
		ImagePtr(Image<T_Type, T_NumComp>* imgPtr) { imPtr = std::shared_ptr<Image<T_Type, T_NumComp>>(imgPtr); };
		ImagePtr(const std::shared_ptr<Image<T_Type, T_NumComp>>& imgPtr)  {imPtr = std::shared_ptr<Image<T_Type, T_NumComp>>(imgPtr); };

		static ImagePtr fromImg(const ImageType & img) { return ImagePtr(std::make_shared<Image<T_Type, T_NumComp>>(img.clone())); }

		void reset(ImageType * ptr) { imPtr.reset(ptr); }

		typename Image<T_Type, T_NumComp>*	get() { return imPtr.get(); };
		const typename Image<T_Type, T_NumComp>::Pixel&			operator()(uint x, uint y) const;
		typename Image<T_Type, T_NumComp>::Pixel&				operator()(uint x, uint y);
		const typename Image<T_Type, T_NumComp>::Pixel&			operator()(const sibr::Vector2i & xy) const;
		typename Image<T_Type, T_NumComp>::Pixel&				operator()(const sibr::Vector2i & xy);

		typename Image<T_Type, T_NumComp>&						operator * () { return imPtr.operator*(); };
		const typename Image<T_Type, T_NumComp>&				operator * () const { return imPtr.operator*(); };
		typename Image<T_Type, T_NumComp>*						operator -> () { return imPtr.operator->(); };
		const typename Image<T_Type, T_NumComp>*				operator -> () const { return imPtr.operator->(); };
		//void													operator = (const std::shared_ptr<Image<T_Type, T_NumComp>> imgShPtr) const { (*this) = imgShPtr; };
		//typename std::shared_ptr<Image<T_Type, T_NumComp>> & 		operator = (const std::shared_ptr<Image<T_Type, T_NumComp>> & imgShPtr) const { imPtr = imgShPtr; return &imPtr; };
		typename std::shared_ptr<Image<T_Type, T_NumComp>> & 			operator = (std::shared_ptr<Image<T_Type, T_NumComp>> & imgShPtr) { imPtr = imgShPtr; return &imPtr; };
		operator bool() { return imPtr.get() != nullptr; };
		operator bool() const { return imPtr.get() != nullptr; };

	};


	template <typename T_Type, unsigned T_NumComp>
	static void		show(const Image<T_Type, T_NumComp> & img, const std::string& windowTitle = "sibr::show()", bool closeWindow = true) {
		cv::namedWindow(windowTitle, CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
		// Note: CV_GUI_EXPANDED does only work with Qt :s
		//SIBR_ASSERT(img.w() > 1 || img.h() > 1);

		cv::imshow(windowTitle, img.toOpenCVBGR());
		cv::waitKey(0);
		if (closeWindow) {
			cv::destroyWindow(windowTitle);
		}
	}

	SIBR_GRAPHICS_EXPORT Image<unsigned char, 3> coloredClass(const Image<unsigned char, 1>::Ptr imClass);
	SIBR_GRAPHICS_EXPORT Image<unsigned char, 3> coloredClass(const Image<int, 1>::Ptr imClass);
	SIBR_GRAPHICS_EXPORT void showFloat(const Image<float, 1> & im, bool logScale = false, double min = -DBL_MAX, double max = DBL_MAX);

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>::Image(void) :
		_pixels(0, 0, opencvType()) { }

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>::Image(uint width, uint height) :
		_pixels(height, width, opencvType()) { }

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>::Image(uint width, uint height, const T_Type& init) :
		_pixels(height, width, opencvType(), init) { }

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>::Image(uint width, uint height, const Pixel& init)
	{
		cv::Scalar scal(0);
		for (int i = 0; i < T_NumComp; i++)
			scal(i) = init(i);

		_pixels = cv::Mat(height, width, opencvType(), scal);

	}

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>::Image(Image<T_Type, T_NumComp>&& other) {
		operator =(std::move(other));
	}

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>& Image<T_Type, T_NumComp>::operator=(Image<T_Type, T_NumComp>&& other) {
		_pixels = std::move(other._pixels);
		return *this;
	}

	template<typename T_Type, unsigned int T_NumComp>
	const void*			Image<T_Type, T_NumComp>::data(void) const {
		SIBR_ASSERT(_pixels.isContinuous() == true); // if not true, you don't want to use this function
		return _pixels.ptr();
	}

	template<typename T_Type, unsigned int T_NumComp>
	void*			Image<T_Type, T_NumComp>::data(void) {
		SIBR_ASSERT(_pixels.isContinuous() == true); // if not true, you don't want to use this function
		return _pixels.ptr();
	}

	template<typename T_Type, unsigned int T_NumComp>
	cv::Mat			Image<T_Type, T_NumComp>::toOpenCVBGR(void) const {
		cv::Mat out = toOpenCV().clone();
		opencv::convertRGB2BGR(out);
		return out;
	}

	template<typename T_Type, unsigned int T_NumComp>
	void			Image<T_Type, T_NumComp>::fromOpenCVBGR(const cv::Mat& imgSrc) {
		cv::Mat img = imgSrc.clone();
		opencv::convertBGR2RGB(img);
		fromOpenCV(img);
	}

	template<typename T_Type, unsigned int T_NumComp>
	void			Image<T_Type, T_NumComp>::fromOpenCV(const cv::Mat& imgSrc) {
		cv::Mat img = imgSrc.clone();

		if (img.depth() != opencv::imageType<T_Type>())
		{
			img.convertTo(img, opencv::imageType<T_Type>(),
				opencv::imageTypeRange<T_Type>() / opencv::imageTypeCVRange(img.depth()));
		}

		cv::Vec<T_Type, T_NumComp> p;
		if (img.channels() != T_NumComp)
		{
			_pixels = cv::Mat(img.rows, img.cols, opencvType());
			for (int y = 0; y < img.rows; ++y)
			{
				for (int x = 0; x < img.cols; ++x)
				{
					const T_Type* ptr = img.ptr<T_Type>(y, x);
					assert(ptr != nullptr);
					uint i;
					for (i = 0; i < (uint)img.channels() && i < T_NumComp; ++i)
						p[i] = ptr[i];
					for (; i < T_NumComp && i < 3; ++i)
						p[i] = p[0];
					for (; i < T_NumComp && i < 4; ++i)
						p[i] = static_cast<T_Type>(opencv::imageTypeRange<T_Type>());

					_pixels.at<cv::Vec<T_Type, T_NumComp>>(y, x) = p;
				}
			}
		}
		else
			_pixels = img;
	}

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>		Image<T_Type, T_NumComp>::clone(void) const {
		Image<T_Type, T_NumComp> img;
		img._pixels = _pixels.clone();
		return img;
	}

	template<typename T_Type, unsigned int T_NumComp>
	ImagePtr<T_Type, T_NumComp>		Image<T_Type, T_NumComp>::clonePtr(void) const {
		ImagePtr<T_Type, T_NumComp> img(new Image<T_Type, T_NumComp>());
		img->_pixels = _pixels.clone();
		return img;
	}

	template<typename T_Type, unsigned int T_NumComp>
	bool		Image<T_Type, T_NumComp>::load(const std::string& filename, bool verbose, bool warning_if_not_found) {
		if (verbose)
			SIBR_LOG << "Loading image file '" << filename << "'." << std::endl;
		else
			std::cerr << ".";
		cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED | CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
		if (img.data == nullptr)
		{
			operator =(Image<T_Type, T_NumComp>()); // reset mat

			if (warning_if_not_found) {
				SIBR_WRG << "Image file not found '" << filename << "'." << std::endl;
			}

			return false;
		}
		opencv::convertBGR2RGB(img);
		fromOpenCV(img);
		return true;
	}

	template<typename T_Type, unsigned int T_NumComp>
	bool		Image<T_Type, T_NumComp>::loadByteStream(const std::string& filename, bool verbose) {
		if (verbose)
			SIBR_LOG << "Loading image file '" << filename << "'." << std::endl;
		else
			std::cerr << ".";


		cv::Vec<T_Type, T_NumComp> p;

		sibr::ByteStream bs;
		if (!bs.load(filename))
			SIBR_WRG << "Image file not found '" << filename << "'." << std::endl;

		int wIm;
		int hIm;
		bs >> wIm >> hIm;

		_pixels = cv::Mat(hIm, wIm, opencvType());
		for (int y = 0; y < hIm; ++y)
		{
			for (int x = 0; x < wIm; ++x)
			{
				uint i;
				for (i = 0; i < T_NumComp; ++i)
					bs >> p[i];

				_pixels.at<cv::Vec<T_Type, T_NumComp>>(y, x) = p;
			}
		}

		return true;
	}

	template<typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::save(const std::string& filename, bool verbose) const {
		{ // Create the output dir if doesn't exists
			boost::filesystem::path outdir = boost::filesystem::path(filename).parent_path();
			if (outdir.empty() == false)
				boost::filesystem::create_directories(outdir);
		}

		// Important Note:
		// If you have a problem when saving an image (e.g. black image) then
		// check the targeted image file format manages correctly the T_Type and
		// T_NumpComp you provide.
		// OpenCV doesn't seem to check always for such incompatibility (and just
		// save empty pixels)

		if (verbose)
			SIBR_LOG << "Saving image file '" << filename << "'." << std::endl;

		cv::Mat img;
		if (T_NumComp == 1) {
			cv::cvtColor(toOpenCVBGR(), img, CV_GRAY2BGR);
		} /// \todo TODO: support for 2 channels images.
		else {
			// For 3 and 4 channels, leave the image untouched.
			img = toOpenCVBGR();
		}

		cv::Mat finalImage;
		if (T_NumComp == 4) {
			cv::Mat4b imageF_8UC4;
			double scale = 255.0 / (double)opencv::imageTypeRange<T_Type>();
			img.convertTo(imageF_8UC4, CV_8UC4, scale);
			finalImage = imageF_8UC4;
		}
		else {
			cv::Mat3b imageF_8UC3;
			double scale = 255.0 / (double)opencv::imageTypeRange<T_Type>();
			img.convertTo(imageF_8UC3, CV_8UC3, scale);
			finalImage = imageF_8UC3;
		}

		if (img.cols > 0 && img.rows > 0)
		{
			if (cv::imwrite(filename, finalImage) == false)
				SIBR_ERR << "unknown error while saving image '" << filename << "'"
				<< " (do the targeted file format manages correctly the bpc ?)" << std::endl;
		}
		else
			SIBR_WRG << "failed to save (image is empty)" << std::endl;
	}

	template<typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::saveHDR(const std::string& filename, bool verbose) const {
		{ // Create the output dir if doesn't exists
			boost::filesystem::path outdir = boost::filesystem::path(filename).parent_path();
			if (outdir.empty() == false)
				boost::filesystem::create_directories(outdir);
		}

		// Important Note:
		// If you have a problem when saving an image (e.g. black image) then
		// check the targeted image file format manages correctly the T_Type and
		// T_NumpComp you provide.
		// OpenCV doesn't seem to check always for such incompatibility (and just
		// save empty pixels)

		if (verbose)
			SIBR_LOG << "Saving image file '" << filename << "'." << std::endl;

		cv::Mat img = toOpenCVBGR();
		

		cv::Mat finalImage;
		if (T_NumComp == 4) {
			cv::Mat4f imageF_32FC4;
			double scale = 1.0 / (double)opencv::imageTypeRange<T_Type>();
			img.convertTo(imageF_32FC4, CV_32FC4, scale);
			finalImage = imageF_32FC4;
		}
		else {
			cv::Mat3f imageF_32FC3;
			double scale = 1.0 / (double)opencv::imageTypeRange<T_Type>();
			img.convertTo(imageF_32FC3, CV_32FC3, scale);
			finalImage = imageF_32FC3;
		}

		if (img.cols > 0 && img.rows > 0)
		{
			if (cv::imwrite(filename, finalImage) == false)
				SIBR_ERR << "unknown error while saving image '" << filename << "'"
				<< " (do the targeted file format manages correctly the bpc ?)" << std::endl;
		}
		else
			SIBR_WRG << "failed to save (image is empty)" << std::endl;
	}

	/* Save and Load function for images of arbitrary types*/
	template <typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::saveByteStream(const std::string& filename, bool verbose) const {
		{ // Create the output dir if doesn't exists
			boost::filesystem::path outdir = boost::filesystem::path(filename).parent_path();
			if (outdir.empty() == false)
				boost::filesystem::create_directories(outdir);
		}
		if (verbose)
			SIBR_LOG << "Saving image file '" << filename << "'." << std::endl;

		sibr::ByteStream bs;

		int wIm = w();
		int hIm = h();

		if (wIm > 0 && hIm > 0) {
			bs << wIm << hIm;
			for (int j = 0; j < hIm; j++) {
				for (int i = 0; i < wIm; i++) {
					for (int k = 0; k < T_NumComp; k++) {
						bs << _pixels.at<cv::Vec<T_Type, T_NumComp>>(j, i)[k];
					}
				}
			}
			bs.saveToFile(filename);
		}
		else
			SIBR_WRG << "failed to save (image is empty)" << std::endl;
	}

	template<typename T_Type, unsigned int T_NumComp>
	inline const typename Image<T_Type, T_NumComp>::Pixel&		Image<T_Type, T_NumComp>::operator()(uint x, uint y) const {
#ifndef NDEBUG
		if (!(x < w() && y < h())) {
			std::cout << " access (" << x << " , " << y << ") while size is " << w() << " x " << h() << std::endl;
}
#endif
		SIBR_ASSERT(x < w() && y < h());
		return _pixels.at<typename Image<T_Type, T_NumComp>::Pixel>(y, x);
	}

	template<typename T_Type, unsigned int T_NumComp>
	inline const typename Image<T_Type, T_NumComp>::Pixel & ImagePtr<T_Type, T_NumComp>::operator()(uint x, uint y) const
	{
		return (*imPtr)(x, y);
	}

	template<typename T_Type, unsigned int T_NumComp>
	inline typename Image<T_Type, T_NumComp>::Pixel&		Image<T_Type, T_NumComp>::operator()(uint x, uint y) {
#ifndef NDEBUG
		if (!(x < w() && y < h())) {
			std::cout << " access (" << x << " , " << y << ") while size is " << w() << " x " << h() << std::endl;
		}
#endif
		SIBR_ASSERT(x < w() && y < h());
		return _pixels.at<typename Image<T_Type, T_NumComp>::Pixel>(y, x);
		//return reinterpret_cast<Image<T_Type, T_NumComp>::Pixel>(_pixels.at<cv::Vec<T_Type, T_NumComp>>(y, x));
		// return _pixels.at<typename T_Type>(y, x);
	}

	template<typename T_Type, unsigned int T_NumComp>
	inline typename Image<T_Type, T_NumComp>::Pixel & ImagePtr<T_Type, T_NumComp>::operator()(uint x, uint y)
	{
		return (*imPtr)(x, y);
	}



	template<typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::pixel(uint x, uint y, const Pixel& p) {
		SIBR_ASSERT(x < w() && y < h());
		cv::Vec<T_Type, T_NumComp> v;//(p.data(), T_NumComp);
		for (uint i = 0; i < T_NumComp; ++i) v[i] = p[i];
		_pixels.at<cv::Vec<T_Type, T_NumComp>>(y, x) = v;
	}

	template<typename T_Type, unsigned int T_NumComp>
	inline const typename Image<T_Type, T_NumComp>::Pixel& Image<T_Type, T_NumComp>::operator()(const sibr::Vector2i & xy) const {
		return operator()(xy[0], xy[1]);
	}
	template<typename T_Type, unsigned int T_NumComp>
	inline const typename Image<T_Type, T_NumComp>::Pixel & ImagePtr<T_Type, T_NumComp>::operator()(const sibr::Vector2i & xy) const
	{
		return (*imPtr)(xy[0], xy[1]);
	}

	template<typename T_Type, unsigned int T_NumComp>
	inline typename Image<T_Type, T_NumComp>::Pixel& Image<T_Type, T_NumComp>::operator()(const sibr::Vector2i & xy) {
		return operator()(xy[0], xy[1]);
	}
	template<typename T_Type, unsigned int T_NumComp>
	inline typename Image<T_Type, T_NumComp>::Pixel & ImagePtr<T_Type, T_NumComp>::operator()(const sibr::Vector2i & xy)
	{
		return (*imPtr)(xy[0], xy[1]);
	}

	template<typename T_Type, unsigned int T_NumComp>
	inline typename Image<T_Type, T_NumComp>::Pixel& Image<T_Type, T_NumComp>::operator()(const sibr::Vector2f & xy) {
		return operator()((int)xy[0], (int)xy[1]);
	}
	template<typename T_Type, unsigned int T_NumComp>
	inline const typename Image<T_Type, T_NumComp>::Pixel& Image<T_Type, T_NumComp>::operator() (const sibr::Vector2f & xy) const {
		return operator()((int)xy[0], (int)xy[1]);
	}

	template<typename T_Type, unsigned int T_NumComp>
	ColorRGBA	Image<T_Type, T_NumComp>::color(uint x, uint y) const {
		SIBR_ASSERT(x < w() && y < h());
		float scale = 1.f / opencv::imageTypeRange<T_Type>();
		cv::Vec<T_Type, T_NumComp> v = _pixels.at<cv::Vec<T_Type, T_NumComp>>(y, x);

		return ColorRGBA(v.val[0] * scale, v.val[1] * scale, v.val[2] * scale,
			(T_NumComp > 3) ? v.val[3] * scale : 1.f);
	}
	template<typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::color(uint x, uint y, const ColorRGBA& rgba) {
		SIBR_ASSERT(x < w() && y < h());
		float scale = opencv::imageTypeRange<T_Type>();
		cv::Vec<T_Type, T_NumComp> v;//(p.data(), T_NumComp);
		for (uint i = 0; i < T_NumComp; ++i) v[i] = T_Type(rgba[i] * scale);
		_pixels.at<cv::Vec<T_Type, T_NumComp>>(y, x) = v;
	}

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>	Image<T_Type, T_NumComp>::resized(int width, int height, int cv_interpolation_method) const
	{
		if (width == w() && height == h())
			return clone();
		Image dst;
		cv::resize(toOpenCV(), dst._pixels, cv::Size(width, height), 0, 0, cv_interpolation_method);
		return dst;
	}

	template<typename T_Type, unsigned int T_NumComp>
	Image<T_Type, T_NumComp>		Image<T_Type, T_NumComp>::resizedMax(int maxlen) const
	{
		float newWidth = (w() >= h()) ? maxlen : maxlen * ((float)w() / (float)h());
		float newHeight = (h() >= w()) ? maxlen : maxlen * ((float)h() / (float)w());

		return resized((int)newWidth, (int)newHeight);
	}

	template<typename T_Type, unsigned int T_NumComp>
	typename Image<T_Type, T_NumComp>::Pixel Image<T_Type, T_NumComp>::color(const ColorRGBA& rgba) {
		float scale = opencv::imageTypeRange<T_Type>();
		Pixel v;//(p.data(), T_NumComp);
		for (uint i = 0; i < T_NumComp; ++i) v[i] = T_Type(rgba[i] * scale);
		return v;
	}

	template<typename T_Type, unsigned int T_NumComp>
	std::string Image<T_Type, T_NumComp>::pixelStr(const sibr::Vector2i & xy)  const {
		if (isInRange(xy)) {
			std::stringstream ss;
			ss << "( " << operator()(xy).cast<std::conditional<std::is_same_v<T_Type, uchar>, int, T_Type>::type>().transpose() << " )";
			return  ss.str();
		}
		return "";
	}

	template<typename T_Type, unsigned int T_NumComp>
	uint		Image<T_Type, T_NumComp>::w(void) const {
		return _pixels.cols;
	}

	template<typename T_Type, unsigned int T_NumComp>
	uint		Image<T_Type, T_NumComp>::h(void) const {
		return _pixels.rows;
	}

	template<typename T_Type, unsigned int T_NumComp>
	sibr::Vector2u	Image<T_Type, T_NumComp>::size(void) const {
		return sibr::Vector2u(w(), h());
	}

	template<typename T_Type, unsigned int T_NumComp>
	uint		Image<T_Type, T_NumComp>::numComp(void) const {
		return T_NumComp;
	}

	template<typename T_Type, unsigned int T_NumComp>
	uint		Image<T_Type, T_NumComp>::sizeOfComp(void) const {
		return sizeof(T_Type)*T_NumComp;
	}

	template<typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::flipH(void) {
		cv::flip(_pixels, _pixels, 0 /*!=0 means horizontal*/);
	}
	template<typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::flipV(void) {
		cv::flip(_pixels, _pixels, 1 /*!=1 means vertical*/);
	}
	//template<typename T_Type, unsigned int T_NumComp>
	//typename Image<T_Type, T_NumComp>::SLArray		Image<T_Type, T_NumComp>::toSLArray( void ) const {
	//	SLArray out(w(), h());

	//	Vertex<T_Type, T_NumComp> v;
	//	for (uint y = 0; y < h(); ++y)
	//	{
	//		for (uint x = 0; x < w(); ++x)
	//		{
	//			Pixel p = pixel(x, y);
	//			for (uint i = 0; i < T_NumComp; ++i)
	//				v[i] = p[i];
	//			out.at(x, y) = v;
	//		}
	//	}
	//	return out;
	//}

	/// Compute min/max over image
	template<typename T_Type, unsigned int T_NumComp>
	void Image<T_Type, T_NumComp>::findMinMax(Pixel& minImage, Pixel& maxImage) {
		for (uint c = 0; c < T_NumComp; ++c) {
			minImage[c] = T_Type(opencv::imageTypeRange<Type>());
			maxImage[c] = T_Type(-opencv::imageTypeRange<Type>());
		}

		Pixel p;
		for (uint y = 0; y < h(); ++y) {
			for (uint x = 0; x < w(); ++x) {
				Pixel v = operator()(x, y);
				for (uint c = 0; c < T_NumComp; ++c) {
					minImage[c] = std::min(v[c], minImage[c]);
					maxImage[c] = std::max(v[c], maxImage[c]);
				}
			}
		}
	}

	// remap into a given range -- useful eg going from 0..1 to 0...255
	template<typename T_Type, unsigned int T_NumComp>
	void		Image<T_Type, T_NumComp>::remap(const Pixel& minVal, const Pixel& maxVal) {
		Pixel minImage;
		Pixel maxImage;
		findMinMax(minImage, maxImage);

		Pixel p;
		for (uint y = 0; y < h(); ++y) {
			for (uint x = 0; x < w(); ++x) {
				Pixel v = operator()(x, y);
				for (uint i = 0; i < T_NumComp; ++i)
					p[i] = minVal[i] + ((maxVal[i] - minVal[i])*(v[i] - minImage[i])) / (maxImage[i] - minImage[i]);
				pixel(x, y, p);
			}
		}
	}

	template<typename T_Type, unsigned int T_NumComp>
	Eigen::Matrix<T_Type, T_NumComp, 1, Eigen::DontAlign> Image<T_Type, T_NumComp>::bilinear(const sibr::Vector2f & queryPosition) const
	{
		if (w() < 2 || h() < 2) {
			return Eigen::Matrix<T_Type, T_NumComp, 1, Eigen::DontAlign>();
		}

		const sibr::Vector2i cornerPixel = (queryPosition - 0.5f*sibr::Vector2f(1, 1)).unaryExpr([](float f) { return std::floor(f); }).cast<int>();
		const sibr::Vector2f ts = queryPosition - (cornerPixel.cast<float>() + 0.5f*sibr::Vector2f(1, 1));

		const sibr::Vector2i topLeft(0, 0), bottomRight(w() - 1, h() - 1);

		const sibr::Vector2i mm = sibr::clamp<int, 2>(cornerPixel + sibr::Vector2i(0, 0), topLeft, bottomRight);
		const sibr::Vector2i pm = sibr::clamp<int, 2>(cornerPixel + sibr::Vector2i(1, 0), topLeft, bottomRight);
		const sibr::Vector2i mp = sibr::clamp<int, 2>(cornerPixel + sibr::Vector2i(0, 1), topLeft, bottomRight);
		const sibr::Vector2i pp = sibr::clamp<int, 2>(cornerPixel + sibr::Vector2i(1, 1), topLeft, bottomRight);
		return (
			operator()(mm).cast<float>() * (1.0f - ts[0]) * (1.0f - ts[1]) +
			operator()(pm).cast<float>() * ts[0] * (1.0f - ts[1]) +
			operator()(mp).cast<float>() * (1.0f - ts[0]) * ts[1] +
			operator()(pp).cast<float>() * ts[0] * ts[1]
			).cast<T_Type>();
	}

	template<typename T_Type, unsigned int T_NumComp>
	Eigen::Matrix<float, T_NumComp, 1, Eigen::DontAlign> Image<T_Type, T_NumComp>::monoCubic(float t, const Eigen::Matrix<float, T_NumComp, 4, Eigen::DontAlign> & colors)
	{
		static const Eigen::Matrix<float, 4, 4> M = 0.5f* (Eigen::Matrix<float, 4, 4>() <<
			0, 2, 0, 0,
			-1, 0, 1, 0,
			2, -5, 4, -1,
			-1, 3, -3, 1
			).finished().transpose();

		return colors * (M*Eigen::Matrix<float, 4, 1>(1, t, t*t, t*t*t));
	}

	template<typename T_Type, unsigned int T_NumComp>
	Eigen::Matrix<T_Type, T_NumComp, 1, Eigen::DontAlign> Image<T_Type, T_NumComp>::bicubic(const sibr::Vector2f & queryPosition) const
	{
		static const std::vector<std::vector<sibr::Vector2i> > offsets = {
			{ { -1,-1 },{ 0,-1 } ,{ 1,-1 },{ 2,-1 } },
			{ { -1,0 },{ 0,0 } ,{ 1,0 },{ 2,0 } },
			{ { -1,1 },{ 0,1 } ,{ 1,1 },{ 2,1 } },
			{ { -1,2 },{ 0,2 } ,{ 1,2 },{ 2,2 } }
		};

		typedef Eigen::Matrix<float, T_NumComp, 4, Eigen::DontAlign> ColorStack;

		if (w() < 4 || h() < 4) {
			return Vector<T_Type, T_NumComp>();
		}

		const sibr::Vector2i cornerPixel = (queryPosition - 0.5f*sibr::Vector2f(1, 1)).unaryExpr([](float f) { return std::floor(f); }).cast<int>();
		const sibr::Vector2f ts = queryPosition - (cornerPixel.cast<float>() + 0.5f*sibr::Vector2f(1, 1));

		ColorStack colorsGrid[4];
		const sibr::Vector2i topLeft(0, 0), bottomRight(w() - 1, h() - 1);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				const sibr::Vector2i pixelPosition = cornerPixel + offsets[i][j];
				colorsGrid[i].col(j) = operator()(sibr::clamp(pixelPosition, topLeft, bottomRight)).cast<float>();
			}
		}

		ColorStack bs;
		for (int i = 0; i < 4; ++i) {
			bs.col(i) = monoCubic(ts[0], colorsGrid[i]);
		}

		Vector<float, T_NumComp> resultFloat = monoCubic(ts[1], bs);
		return (resultFloat.unaryExpr([](float f) { return sibr::clamp(f, 0.0f, sibr::opencv::imageTypeRange<T_Type>()); })).cast<T_Type>();
	}

	template <typename sibr_T, typename openCV_T, int N>
	inline Vector<sibr_T, N> fromOpenCV(const cv::Vec<openCV_T, N> & vec) {
		Vector<sibr_T, N> out;
		for (int i = 0; i < N; ++i) {
			out[i] = static_cast<sibr_T>(vec[i]);
		}
		return out;
	}

	template <typename openCV_T, typename sibr_T, int N>
	inline cv::Vec<openCV_T, N> toOpenCV(const Vector<sibr_T, N> & vec) {
		cv::Vec<openCV_T, N> out;
		for (int i = 0; i < N; ++i) {
			out[i] = static_cast<openCV_T>(vec[i]);
		}
		return out;
	}

	//template<typename T_Type, unsigned int T_NumComp>
	//void		Image<T_Type, T_NumComp>::fromSLArray( const SLArray& pixels ) {
	//	_pixels.create(pixels.ysize(), pixels.xsize(), opencvType());

	//	Pixel p;
	//	for (uint y = 0; y < h(); ++y)
	//	{
	//		for (uint x = 0; x < w(); ++x)
	//		{
	//			Pixel v = pixels.at(x, y);
	//			for (uint i = 0; i < T_NumComp; ++i)
	//				p[i] = v[i];
	//			pixel(x, y, p);
	//		}
	//	}
	//}

	//template<typename T_Type, unsigned int T_NumComp>
	//void		Image<T_Type, T_NumComp>::fromSLRenderTarget( const SLRenderTarget& img, uint target=0 ) {
	//	SLRenderTarget::PixelArray pa(img.w(), img.h());
	//	img.readBack(pa, target);
	//	fromSLArray(pa);
	//}

	/// Standard image types
	typedef Image<unsigned char, 3> ImageRGB;
	typedef Image<unsigned char, 4> ImageRGBA;
	typedef Image<unsigned char, 1> ImageL8;
	typedef Image<unsigned char, 2> ImageUV8;
	typedef Image<unsigned short int, 3> ImageRGB16;
	typedef Image<unsigned short int, 1> ImageL16;
	typedef Image<float, 3>         ImageRGB32F;
	typedef Image<float, 3>         ImageFloat3;
	typedef Image<float, 4>         ImageRGBA32F;
	typedef Image<float, 4>         ImageFloat4;
	typedef Image<float, 1>         ImageL32F;
	typedef Image<float, 1>         ImageFloat1;
	typedef Image<float, 2>         ImageFloat2;
	//typedef Image<half,3>          ImageRGB16F;
	//typedef Image<half,4>          ImageRGBA16F;
	//typedef Image<half,1>          ImageL16F;
	typedef Image<bool, 1>          ImageBool1;
	typedef Image<double, 1>        ImageDouble1;
	typedef Image<double, 2>        ImageDouble2;
	typedef Image<double, 3>        ImageDouble3;
	typedef Image<double, 4>        ImageDouble4;

	typedef Image<int, 1>        ImageInt1;

	/**
	*convert a 1 channel 32 bits into a 4 channels 8 bits, useful to save float maps as png, and such get png compression for free
	*/
	SIBR_GRAPHICS_EXPORT sibr::ImageRGBA convertL32FtoRGBA(const sibr::ImageL32F & imgF);

	/**
	*convert a 4 channels 8 bits into a 1 channel 32 bits, see convertL32FtoRGBA()
	*/
	SIBR_GRAPHICS_EXPORT sibr::ImageL32F convertRGBAtoL32F(const sibr::ImageRGBA  & imgRGBA);

	SIBR_GRAPHICS_EXPORT sibr::ImageRGBA convertRGB32FtoRGBA(const sibr::ImageRGB32F & imgF);
	//SIBR_GRAPHICS_EXPORT sibr::ImageRGBA convertRGB32FtoRGBA_2(const sibr::ImageRGB32F & imgF);

	SIBR_GRAPHICS_EXPORT sibr::ImageRGB32F convertRGBAtoRGB32F(const sibr::ImageRGBA & imgF);

	/**
	*convert a normal map 3 channels bits into 2 channels 16 bits as 4 channels 8 bits, keeping theta and phi as half float
	*/
	SIBR_GRAPHICS_EXPORT sibr::ImageRGBA convertNormalMapToSphericalHalf(const sibr::ImageRGB32F & imgF);
	SIBR_GRAPHICS_EXPORT sibr::ImageRGB32F convertSphericalHalfToNormalMap(const sibr::ImageRGBA & imgF);

	/**
	* create a 3 channel mat from a single channel mat
	*/
	SIBR_GRAPHICS_EXPORT cv::Mat duplicate3(cv::Mat c);

} // namespace sibr

#endif // __SIBR_GRAPHICS_IMAGE_HPP__
