
#ifndef __SIBR_GRAPHICS_TEXTURE_HPP__
# define __SIBR_GRAPHICS_TEXTURE_HPP__

#include <type_traits>

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Image.hpp"
# include "core/graphics/RenderUtility.hpp"



# define SIBR_GPU_AUTOGEN_MIPMAP		(1<<0)
# define SIBR_GPU_MULSTISAMPLE			(1<<1)
# define SIBR_GPU_LINEAR_SAMPLING		(1<<2)
# define SIBR_MAX_SHADER_ATTACHMENTS	(1<<3)
# define SIBR_GPU_INTEGER				(1<<4)
# define SIBR_MSAA4X					(1<<5)
# define SIBR_MSAA8X					(1<<6)
# define SIBR_MSAA16X					(1<<7)
# define SIBR_MSAA32X					(1<<8)
# define SIBR_STENCIL_BUFFER			(1<<9)
# define SIBR_CLAMP_UVS					(1<<10)
# define SIBR_CLAMP_TO_BORDER			(1<<11)
# define SIBR_FLIP_TEXTURE				(1<<12)

# define SIBR_COMPILE_FORCE_SAMPLING_LINEAR	0

namespace sibr
{

	// -----------------------------------------------------------------------

	//typedef float depth32;
	//typedef uint depth24;
	//typedef ushort depth16;

	// -----------------------------------------------------------------------

	template <typename T> class GLType;

	template <> class GLType<unsigned char> {
	public:
		enum {type=GL_UNSIGNED_BYTE};
	};

	template <> class GLType<unsigned short> {
	public:
		enum {type=GL_UNSIGNED_SHORT};
	};

	template <> class GLType<short> {
	public:
		enum {type=GL_SHORT};
	};

	template <> class GLType<float> {
	public:
		enum {type=GL_FLOAT};
	};

	template <> class GLType<int> {
	public:
		enum {type=GL_INT};
	};

	//template <> class GLType<depth32> {
	//public:
	//	enum {type=GL_FLOAT};
	//};

	//template <> class GLType<depth24> {
	//public:
	//	enum {type=GL_UNSIGNED_INT};
	//};

	//template <> class GLType<depth16> {
	//public:
	//	enum {type=GL_UNSIGNED_SHORT};
	//};

	// -----------------------------------------------------------------------

	template <typename T_Type,int T_Num> class GLFormat;

	template <> class GLFormat<unsigned char,1> {
	public:
		enum {
			internal_format     = GL_R8,
			format              = GL_RED,
			int_internal_format = GL_R8UI,
			int_format          =  GL_RED_INTEGER,
			isdepth             = 0};
	};

	template <> class GLFormat<unsigned char,3> {
	public:
		enum {
			internal_format     = GL_RGB8,
			format              = GL_RGB,
			int_internal_format = GL_RGB8UI,
			int_format          = GL_RGB_INTEGER,
			isdepth             = 0};
	};

	template <> class GLFormat<unsigned char,4> {
	public:
		enum {
			internal_format     = GL_RGBA8,
			format              = GL_RGBA,
			int_internal_format = GL_RGBA8UI,
			int_format          = GL_RGBA_INTEGER,
			isdepth             = 0};
	};

	template <> class GLFormat<unsigned short,1> {
	public:
		enum {
			internal_format     = GL_R16,
			format              = GL_R,
			int_internal_format = GL_R16UI,
			int_format          = GL_RED_INTEGER,
			isdepth             = 0};
	};

	template <> class GLFormat<unsigned short,3> {
	public:
		enum {
			internal_format     = GL_RGB16,
			format              = GL_RGB,
			int_internal_format = GL_RGB16UI,
			int_format          = GL_RGB_INTEGER,
			isdepth             = 0};
	};

	template <> class GLFormat<unsigned short,4> {
	public:
		enum {
			internal_format     = GL_RGBA16,
			format              = GL_RGBA,
			int_internal_format = GL_RGBA16UI,
			int_format          = GL_RGBA_INTEGER,
			isdepth             = 0};
	};

	template <> class GLFormat<int,1> {
	public:
		enum {
			internal_format     = GL_R32I,
			format              = GL_RED_INTEGER,
			int_internal_format = GL_R32I,
			int_format          = GL_RED_INTEGER,
			isdepth             = 0};
	};

	template <> class GLFormat<float,1> {
	public:
		enum {
			internal_format     = GL_R32F,
			format              = GL_RED,
			int_internal_format = -1,
			int_format          = -1,
			isdepth             =  0};
	};

	template <> class GLFormat<float,3> {
	public:
		enum {
			internal_format     = GL_RGB32F,
			format              = GL_RGB,
			int_internal_format = -1,
			int_format          = -1,
			isdepth             =  0};
	};

	template <> class GLFormat<float,4> {
	public:
		enum {
			internal_format		= GL_RGBA32F,
			format              = GL_RGBA,
			int_internal_format = -1,
			int_format          = -1,
			isdepth             =  0};
	};



	//// depth texture format

	//template <> class GLFormat<depth32,1> {
	//public:
	//	enum {
	//		internal_format     = GL_DEPTH_COMPONENT32F,
	//		format              = GL_DEPTH_COMPONENT,
	//		int_internal_format = -1,
	//		int_format          = -1,
	//		isdepth             =  1};
	//};

	//template <> class GLFormat<depth24,1> {
	//public:
	//	enum {
	//		internal_format     = GL_DEPTH_COMPONENT24,
	//		format              = GL_DEPTH_COMPONENT,
	//		int_internal_format = -1,
	//		int_format          = -1,
	//		isdepth             =  1};
	//};

	//template <> class GLFormat<depth16,1> {
	//public:
	//	enum {
	//		internal_format     = GL_DEPTH_COMPONENT16,
	//		format              = GL_DEPTH_COMPONENT,
	//		int_internal_format = -1,
	//		int_format          = -1,
	//		isdepth             =  1};
	//};

	//-------------------------------------

	template <typename T_Type, int T_Num> class GLFormatCVmat;

	template <> class GLFormatCVmat<unsigned char, 1> {
	public:
		enum {
			internal_format = GLFormat<uchar, 1>::internal_format,
			format = GLFormat<uchar, 1>::format,
			int_internal_format = GLFormat<uchar, 1>::int_internal_format,
			int_format = GLFormat<uchar, 1>::int_format,
			isdepth = GLFormat<uchar, 1>::isdepth
		};
	};
	template <> class GLFormatCVmat<unsigned char, 3> {
	public:
		enum {
			internal_format = GLFormat<uchar,3>::internal_format,
			format = GL_BGR,
			int_internal_format = GLFormat<uchar, 3>::int_internal_format,
			int_format = GLFormat<uchar, 3>::int_format,
			isdepth = GLFormat<uchar, 3>::isdepth
		};
	};

	template <> class GLFormatCVmat<unsigned char, 4> {
	public:
		enum {
			internal_format = GLFormat<uchar, 4>::internal_format,
			format = GL_BGRA,
			int_internal_format = GLFormat<uchar, 4>::int_internal_format,
			int_format = GLFormat<uchar, 4>::int_format,
			isdepth = GLFormat<uchar, 4>::isdepth
		};
	};

	//helper class to specify from which image type we can use texture
	template<typename ImageType> struct ValidGLTexFormat {
		static const bool value = false;
	};
	template<typename ScalarType, uint N> struct ValidGLTexFormat<sibr::Image<ScalarType, N>> {
		static const bool value = true;
	};
	template<> struct ValidGLTexFormat<cv::Mat> {
		static const bool value = true;
	};

	/// Helper class to provide, from an image type, all the information needed for opengl textures
	/// right now it can work with all sibr::Image and with cv::Mat (3U8 only)
	/// you can add more using explicit template instanciation to specify both 
	/// ValidGLTexFormat and the following GLTexFormat properties
	template<typename ImageType, typename ScalarType = typename ImageType::Type, uint N = ImageType::e_NumComp> struct GLTexFormat {
		static_assert(ValidGLTexFormat<ImageType>::value, "ImageWrapper currently only specialized for sibr::Image and cv::Mat ");
		static ImageType flip(const ImageType & img);
		static ImageType resize(const ImageType & img, uint w, uint h);
		static uint width(const ImageType & img);
		static uint height(const ImageType & img);
		static const void* data(const ImageType &img);

		static const uint internal_format;
		static const uint format;
		static const uint int_internal_format;
		static const uint int_format;
		static const uint isdepth;
		static const uint type;
	};

	template<typename ScalarType, uint N > struct GLTexFormat<sibr::Image<ScalarType, N>, ScalarType, N > {
		using ImageType = sibr::Image<ScalarType, N>;

		static ImageType flip(const ImageType & img) {
			ImageType temp = img.clone();
			temp.flipH();
			return temp;
		}
		static ImageType resize(const ImageType & img, uint w, uint h) {
			return img.resized(w, h);
		}
		static uint width(const ImageType & img) {
			return img.w();
		}
		static uint height(const ImageType & img) {
			return img.h();
		}
		static const void * data(const ImageType & img) {
			return img.data();
		}

		static const uint internal_format = GLFormat<ScalarType, N>::internal_format;
		static const uint format = GLFormat<ScalarType, N>::format;
		static const uint int_internal_format = GLFormat<ScalarType, N>::int_internal_format;
		static const uint int_format = GLFormat<ScalarType, N>::int_format;
		static const uint isdepth = GLFormat<ScalarType, N>::isdepth;
		static const uint type = GLType<ScalarType>::type;
	};

	template<typename ScalarType, uint N > struct GLTexFormat<cv::Mat, ScalarType, N> {
		static_assert(std::is_same_v<ScalarType, uchar> && (N == 3 || N == 4 || N == 1) , "GLTexFormat with cv::Mat currently only defined for 3U8 or 4U8");

		static cv::Mat flip(const cv::Mat & img) {
			cv::Mat temp;
			cv::flip(img, temp, 0); //0 for flipH
			return temp;
		}
		static cv::Mat resize(const cv::Mat & img, uint w, uint h) {
			cv::Mat temp;
			cv::resize(img, temp, cv::Size(w, h));
			return temp;
		}
		static uint width(const cv::Mat & img) {
			return img.cols;
		}
		static uint height(const cv::Mat & img) {
			return img.rows;
		}
		static const void* data(const cv::Mat & img) {
			return img.ptr();
		}
		static void* data(cv::Mat & img) {
			return img.ptr();
		}
		static uint cv_type() {
			return CV_MAKE_TYPE(cv::DataType<ScalarType>::depth, N);
		}

		static const uint internal_format = GLFormatCVmat<ScalarType, N>::internal_format;
		static const uint format = GLFormatCVmat<ScalarType, N>::format;
		static const uint int_internal_format = GLFormatCVmat<ScalarType, N>::int_internal_format;
		static const uint int_format = GLFormatCVmat<ScalarType, N>::int_format;
		static const uint isdepth = GLFormatCVmat<ScalarType, N>::isdepth;
		static const uint type = GLType<ScalarType>::type;
	};

	//-------------------------------------


	// -----------------------------------------------------------------------
	// Note About These Interfaces:
	// I like template but they increase the compilation time. So I try to
	// avoid propagation of templates (e.g. in code using template) by
	// proposing a generic interface.
	// Thus if you want to get <any> render target in your function, try
	// using IRenderTarget instead of templates.
	// (It's technically a bit slower (one indirection due to virtual
	// functions) but its nothing compared to the added compilation time of
	// new templated codes).

	/**
	* \ingroup sibr_graphics
	*/
	class ITexture2D
	{
	public:
		typedef std::shared_ptr<ITexture2D>	Ptr;
		typedef std::unique_ptr<ITexture2D>	UPtr;
	public:
		virtual ~ITexture2D( void ) { }

		virtual GLuint handle (void) const = 0;
		virtual uint   w      (void) const = 0;
		virtual uint   h      (void) const = 0;
	};

	/**
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT IRenderTarget
	{
	public:
		typedef std::shared_ptr<IRenderTarget>	Ptr;
		typedef std::unique_ptr<IRenderTarget>	UPtr;
	public:
		virtual ~IRenderTarget( void ) { }

		virtual GLuint texture( uint t = 0 ) const = 0;
		virtual void bind( void ) = 0;
		virtual void unbind( void ) = 0;
		virtual void clear( void ) = 0;
		//virtual void readBack(void) = 0;
		virtual uint   w      (void) const = 0;
		virtual uint   h      (void) const = 0;
		virtual GLuint fbo		(void) const = 0;
	};

	// -----------------------------------------------------------------------


	/** 
	Texture 2D generic
	* \ingroup sibr_graphics
	*/
	template<typename T_Type, unsigned int T_NumComp>
	class Texture2D : public ITexture2D {
		SIBR_DISALLOW_COPY( Texture2D );
	public:
		typedef		Image<T_Type, T_NumComp>			PixelImage;
		typedef		typename PixelImage::Pixel			PixelFormat;
		typedef		std::shared_ptr<Texture2D<T_Type, T_NumComp>>	Ptr;
		typedef		std::unique_ptr<Texture2D<T_Type, T_NumComp>>	UPtr;

	private:
		GLuint  m_Handle		= 0;
		uint    m_W				= 0;
		uint    m_H				= 0;
		uint    m_Flags			= 0;
		bool	m_autoMIPMAP	= false;

		/// Create 2D texture
		//static GLuint create2D(const PixelImage& array, uint flags);

		/// Create 2D texture from cv mat
		//static GLuint create2D(const cv::Mat & array, uint flags);

		/// Create 2D texture from cv mat
		template<typename ImageType> static GLuint create2D(const ImageType & array, uint flags);

		/// Create 2D texture with mipmap
		static GLuint create2D(const std::vector<PixelImage>& miparray, uint flags);

		/// Send 2D texture to GPU memory 
		template<typename ImageType> static void send2D(GLuint id, const ImageType & array, uint flags);

		/// Send 2D texture to GPU memory, each mipmap is specified
		static void send2Dmipmap(GLuint id, const std::vector<PixelImage>& miparray, uint flags);

	public:
		Texture2D(void);
		template<typename ImageType> Texture2D(const ImageType & img, uint flags = 0);

		Texture2D(const std::vector<PixelImage>& miparray, uint flags=0);

		~Texture2D(void);

		GLuint handle (void) const;
		uint   w      (void) const;
		uint   h      (void) const;

		sibr::Image<T_Type, T_NumComp>		readBack( void ) const;

		template<typename ImageType> void update(const ImageType & img);

		void mipmap(int maxLOD = -1);	// manually generate mipmap
	};


	/** 
	Render target 2D generic
	* \ingroup sibr_graphics
	*/
	template<typename T_Type, unsigned int T_NumComp>
	class RenderTarget : public IRenderTarget {
		SIBR_DISALLOW_COPY( RenderTarget );
	public:
		typedef		Image<T_Type, T_NumComp>		PixelImage;
		typedef		typename PixelImage::Pixel		PixelFormat;
		typedef		std::shared_ptr<RenderTarget<T_Type, T_NumComp>>	Ptr;
		typedef		std::unique_ptr<RenderTarget<T_Type, T_NumComp>>	UPtr;

	private:

		GLuint m_fbo			= 0;
		GLuint m_depth_rb		= 0;
		GLuint m_stencil_rb		= 0;
		GLuint m_textures[SIBR_MAX_SHADER_ATTACHMENTS];
		uint   m_numtargets		= 0;
		bool   m_autoMIPMAP		= false;
		bool   m_msaa			= false;
		bool   m_stencil		= false;
		uint   m_W				= 0;
		uint   m_H				= 0;

	public:

		RenderTarget(void);

		RenderTarget(uint w, uint h, uint flags=0, uint num=1);

		~RenderTarget(void);

		GLuint texture(uint t = 0) const; // deprecated (use handle)
		GLuint handle(uint t = 0) const;
		GLuint depthRB() const;

		void bind(void);

		void unbind(void);

		/**
		 * \warn Beware, this function will unbind the render target after clearing.
		 */
		void clear( void );

		/**
		 * \warn Beware, this function will unbind the render target after clearing.
		 */
		void clear( const typename RenderTarget<T_Type, T_NumComp>::PixelFormat& v );
		void clearStencil( void );

		template <typename TType, uint NNumComp>
		void swap( RenderTarget<TType, NNumComp>& other );

		//void readBack( PixelImage& array, uint target=0 ) const;
		//void readBack(sibr::Image<T_IType, N_INumComp>& img, uint target = 0);

		template <typename TType, uint NNumComp>
			void readBack( sibr::Image<TType, NNumComp>& image, uint target=0 ) const;

		template <typename TType, uint NNumComp>
			void readBackToCVmat(cv::Mat & image, uint target = 0) const;

		template <typename TType, uint NNumComp>
			void readBackDepth( sibr::Image<TType, NNumComp>& image, uint target=0 ) const;

		uint   numTargets (void)  const;
		uint   w          (void)  const;
		uint   h          (void)  const;
		GLuint fbo		  (void)  const;
	};

	// -----------------------------------------------------------------------

	typedef Texture2D<unsigned char,3>     Texture2DRGB;
	typedef Texture2D<unsigned char,4>     Texture2DRGBA;
	typedef Texture2D<unsigned char,1>     Texture2DLum;

	typedef Texture2D<unsigned short,4>    Texture2DRGBA16;
	typedef Texture2D<unsigned short,1>    Texture2DLum16;
	typedef Texture2D<unsigned short,2>    Texture2DUV16;
	typedef Texture2D<short,2>             Texture2DUV16s;

	typedef Texture2D<float,3>             Texture2DRGB32F;
	typedef Texture2D<float,4>             Texture2DRGBA32F;
	typedef Texture2D<float,1>             Texture2DLum32F;

	typedef RenderTarget<unsigned char,3>  RenderTargetRGB;
	typedef RenderTarget<unsigned char,4>  RenderTargetRGBA;
	typedef RenderTarget<unsigned char,1>  RenderTargetLum;

	typedef RenderTarget<float,3>          RenderTargetRGB32F;
	typedef RenderTarget<float,4>          RenderTargetRGBA32F;
	typedef RenderTarget<float,1>          RenderTargetLum32F;

	typedef RenderTarget<int,1>			   RenderTargetInt1;
	//////////////////////////////////////////////////////////////////////////////
	// IMPORTANT NOTE CONCERNING DEPTH BUFFER:
	//
	// *NEW* They have been commented; they have not been tested and doesn't seem
	//       to work correctly.
	//
	// /!\ They have not been heavily tested /!\
	// Other kinds of RenderTarget (e.g. RenderTargetRGB) creates
	// also a new depth buffer that is bound in RenderTarget::bind()
	// (with the color buffer).
	// Thus don't try to create two render targets; one for the
	// color buffer and one for the depth buffer (both are generally
	// included).
	//////////////////////////////////////////////////////////////////////////////
	//typedef RenderTarget<depth24,1>        RenderTargetDepth24;
	//typedef RenderTarget<depth32,1>        RenderTargetDepth32;

	/**
	* \ingroup sibr_graphics
	*/
	template <typename T_Type, unsigned T_NumComp>
	static void		show( const RenderTarget<T_Type, T_NumComp> & rt, const std::string& winTitle="sibr::show()" ) {
		Image<T_Type, T_NumComp> img;
		rt.readBack(img);
		show(img, winTitle);
	}

	template <typename T_Type, unsigned T_NumComp>
	static void		show(const Texture2D<T_Type, T_NumComp> & texture, const std::string& winTitle = "sibr::show()") {
		Image<T_Type, T_NumComp> img(texture.w(),texture.h());
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.handle());

		glGetTexImage(GL_TEXTURE_2D, 0, sibr::GLFormat<T_Type, T_NumComp>::format, sibr::GLType<T_Type>::type, img.data());
		show(img, winTitle);
	}

	// -----------------------------------------------------------------------

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	GLuint Texture2D<T_Type, T_NumComp>::create2D(const ImageType& img, uint flags) {
		GLuint id = 0;
		CHECK_GL_ERROR;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		if (flags & SIBR_CLAMP_UVS) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else if (flags & SIBR_CLAMP_TO_BORDER) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		if (flags & SIBR_GPU_AUTOGEN_MIPMAP) {
			if (flags & SIBR_GPU_INTEGER) {
				throw std::runtime_error("Mipmapping on integer texture not supported, probably not even by OpenGL");
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
#if SIBR_COMPILE_FORCE_SAMPLING_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
			if (flags & SIBR_GPU_LINEAR_SAMPLING) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
#endif
		}
		send2D(id, img, flags);
		CHECK_GL_ERROR;
		return id;
	}

	template<typename T_Type, unsigned int T_NumComp>
	/*static*/ GLuint Texture2D<T_Type, T_NumComp>::create2D(const std::vector<PixelImage>& miparray, uint flags) {
		GLuint id = 0;
		CHECK_GL_ERROR;
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_2D,id);
		if (flags & SIBR_CLAMP_UVS) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else if (flags & SIBR_CLAMP_TO_BORDER) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		send2Dmipmap(id,miparray,flags);
		CHECK_GL_ERROR;
		return id;
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	void Texture2D<T_Type, T_NumComp>::send2D(GLuint id, const ImageType& img, uint flags) {
		using FormatInfos = GLTexFormat<ImageType, T_Type, T_NumComp>;

		if (flags & SIBR_GPU_INTEGER) {
			if (FormatInfos::int_internal_format < 0) {
				throw std::runtime_error("Texture format does not support integer mapping");
			}
		}
	
		bool flip = flags & SIBR_FLIP_TEXTURE;
		ImageType flippedImg;
		if (flip) {
			flippedImg = FormatInfos::flip(img);
		}
		const ImageType & sendedImg = flip ? flippedImg : img;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			(flags & SIBR_GPU_INTEGER) ? FormatInfos::int_internal_format : FormatInfos::internal_format,
			FormatInfos::width(sendedImg), FormatInfos::height(sendedImg),
			0,
			(flags & SIBR_GPU_INTEGER) ? FormatInfos::int_format : FormatInfos::format,
			FormatInfos::type,
			FormatInfos::data(sendedImg)
		);

		bool autoMIPMAP = ((flags & SIBR_GPU_AUTOGEN_MIPMAP) != 0);
		if (autoMIPMAP)
			glGenerateMipmap(GL_TEXTURE_2D);
		CHECK_GL_ERROR;
	}

	/// Send 2D texture to GPU memory, each mipmap is specified
	template<typename T_Type, unsigned int T_NumComp>
	/*static*/ void Texture2D<T_Type, T_NumComp>::send2Dmipmap(GLuint id, const std::vector<PixelImage>& miparray, uint flags) {
		CHECK_GL_ERROR;
		if (flags & SIBR_GPU_INTEGER) {
			throw std::runtime_error("Mipmapping on integer texture not supported, probably not even by OpenGL");
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT,   1);
		glBindTexture(GL_TEXTURE_2D,id);

		std::vector<PixelImage> flippedMipArray;
		bool flip = m_Flags & SIBR_FLIP_TEXTURE;
		if (flip) {
			flippedMipArray.resize(miparray.size());
#pragma omp parallel for
			for (uint l = 0; l<miparray.size(); l++) {
				flippedMipArray[l] = miparray[l].clone();
				flippedMipArray[l].flipH();
			}
		}
		const std::vector<PixelImage> & sendedMipArray = flip ? flippedMipArray : miparray;

		for (uint l=0; l<miparray.size(); l++) {
			glTexImage2D(GL_TEXTURE_2D,
				l,
				GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::internal_format,
				miparray[l].w(), miparray[l].h(),
				0,
				GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::format,
				GLType<typename PixelFormat::Type>::type,
				sendedMipArray[l].data()
			);
		}
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp>
	Texture2D<T_Type, T_NumComp>::Texture2D(void) {
		m_Flags = 0;
		m_W     = 0;
		m_H     = 0;
		m_Handle= 0;
		m_autoMIPMAP = false;
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	Texture2D<T_Type, T_NumComp>::Texture2D(const ImageType& img, uint flags) {
		using TexFormat = GLTexFormat<ImageType, T_Type, T_NumComp>;
		m_Flags = flags;
		m_W = TexFormat::width(img);
		m_H = TexFormat::height(img);
		m_Handle = create2D(img, m_Flags);
		m_autoMIPMAP = ((flags & SIBR_GPU_AUTOGEN_MIPMAP) != 0);
	}

	template<typename T_Type, unsigned int T_NumComp>
	Texture2D<T_Type, T_NumComp>::Texture2D(const std::vector<PixelImage>& miparray, uint flags) {
		m_Flags = flags;
		m_W     = miparray[0].w();
		m_H     = miparray[0].h();
		m_Handle= create2D(miparray,m_Flags);
		m_autoMIPMAP = false;
	}

	template<typename T_Type, unsigned int T_NumComp>
	Texture2D<T_Type, T_NumComp>::~Texture2D(void) {
		CHECK_GL_ERROR;
		glDeleteTextures(1,&m_Handle);
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp>
	GLuint Texture2D<T_Type, T_NumComp>::handle (void) const { return m_Handle;}
	template<typename T_Type, unsigned int T_NumComp>
	uint   Texture2D<T_Type, T_NumComp>::w      (void) const { return m_W;     }
	template<typename T_Type, unsigned int T_NumComp>
	uint   Texture2D<T_Type, T_NumComp>::h      (void) const { return m_H;     }


	template<typename T_Type, unsigned int T_NumComp>
	sibr::Image<T_Type, T_NumComp>		Texture2D<T_Type, T_NumComp>::readBack( void ) const {

		// makes sure Vertex have the correct size (read back relies on pointers)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT,   1);
		glBindTexture(GL_TEXTURE_2D, handle());

		int w, h;
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH, &w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);

		sibr::Image<T_Type, T_NumComp> img(w, h);

		glGetTexImage(GL_TEXTURE_2D,
		0,
		GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::format,
		GLType<typename PixelFormat::Type>::type,
		img.data()
		);

		// flip data vertically to get origin on lower left corner
		img.flipH();

		CHECK_GL_ERROR;

		return img;
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	void Texture2D<T_Type, T_NumComp>::update(const ImageType& img) {
		using FormatInfos = GLTexFormat<ImageType, T_Type, T_NumComp>;
		if (FormatInfos::width(img) == w() && FormatInfos::height(img) == h())
		{
			bool flip = m_Flags & SIBR_FLIP_TEXTURE;
			ImageType flippedImg;
			if (flip) {
				flippedImg = FormatInfos::flip(img);
			}
			const ImageType & sendedImg = flip ? flippedImg : img;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glBindTexture(GL_TEXTURE_2D, handle());
			glTexSubImage2D(GL_TEXTURE_2D, 0,
				0, 0, FormatInfos::width(sendedImg), FormatInfos::height(sendedImg),
				FormatInfos::format,
				FormatInfos::type,
				FormatInfos::data(sendedImg)
			);
			if (m_autoMIPMAP)
				glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			m_W = FormatInfos::width(img);
			m_H = FormatInfos::height(img);
			send2D(m_Handle, img, m_Flags);
		}
	}

	template<typename T_Type, unsigned int T_NumComp>
	void Texture2D<T_Type, T_NumComp>::mipmap(int maxLOD) {
		glBindTexture(GL_TEXTURE_2D, handle());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLOD >= 0 ? maxLOD : 1000);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		m_autoMIPMAP = true;
		glGenerateMipmap(GL_TEXTURE_2D);	
	}


	// -----------------------------------------------------------------------

	template<typename T_Type, unsigned int T_NumComp>
	RenderTarget<T_Type, T_NumComp>::RenderTarget(void) {
		m_fbo = 0;
		m_depth_rb = 0;
		m_numtargets = 0;
		m_W = 0;
		m_H = 0;
	}

	template<typename T_Type, unsigned int T_NumComp>
	RenderTarget<T_Type, T_NumComp>::RenderTarget(uint w, uint h, uint flags, uint num) {
		RenderUtility::useDefaultVAO();

		m_W = w;
		m_H = h;

		bool is_depth = (GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::isdepth != 0);

		int maxRenterTargets = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxRenterTargets);

		SIBR_ASSERT(num < uint(maxRenterTargets) && num > 0);
		SIBR_ASSERT(!is_depth || num == 1);

		if (flags & SIBR_GPU_INTEGER) {
			if (GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::int_internal_format < 0) {
				throw std::runtime_error("Integer render  - format does not support integer mapping");
			}
		}

		glGenFramebuffers(1, &m_fbo);

		if (!is_depth) {
			glGenRenderbuffers(1, &m_depth_rb); // depth buffer for color rt
			//glGenRenderbuffers(1, &m_stencil_rb); // stencil buffer for color rt
		}
		else
			m_depth_rb = 0;

		m_numtargets = num;
		m_autoMIPMAP = ((flags & SIBR_GPU_AUTOGEN_MIPMAP) != 0);

		m_msaa = ((flags & SIBR_GPU_MULSTISAMPLE) != 0);
		m_stencil = ((flags & SIBR_STENCIL_BUFFER) != 0);

		if( m_msaa && (m_numtargets != 1))
			throw std::runtime_error("Only one MSAA render target can be attached.");
		for (uint n=0; n<m_numtargets; n++) {
			if (m_msaa)
				break;

			glGenTextures(1, &m_textures[n]);


			glBindTexture(GL_TEXTURE_2D,m_textures[n]);

			if (flags & SIBR_CLAMP_UVS) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			/// TODO: following causes enum compare warning -Wenum-compare
			glTexImage2D(GL_TEXTURE_2D,
				0,
				(flags & SIBR_GPU_INTEGER)
				? GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::int_internal_format
				: GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::internal_format,
				w,h,
				0,
				(flags & SIBR_GPU_INTEGER)
				? GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::int_format
				: GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::format,
				GLType<typename PixelFormat::Type>::type,
				NULL);


			if (!m_autoMIPMAP) {
#if SIBR_COMPILE_FORCE_SAMPLING_LINEAR
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#else
				if (flags & SIBR_GPU_LINEAR_SAMPLING) {
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				}
				else {
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
				}
#endif
			} else { /// \todo TODO: this crashes with 16F RT
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			}
		}


		if (!m_msaa) {
			if (!is_depth) {
				glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rb);
				if(!m_stencil)
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h);
				else
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

				//CHECK_GL_ERROR;
				//glBindRenderbuffer(GL_RENDERBUFFER, m_stencil_rb);
				//glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);
				CHECK_GL_ERROR;
				glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
				for (uint n = 0; n < m_numtargets; n++) {
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + n, GL_TEXTURE_2D, m_textures[n], 0);
				}
				CHECK_GL_ERROR;
				if( !m_stencil )
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_rb);
				else
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_rb);
				//CHECK_GL_ERROR;
				//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_stencil_rb);
			}
			else {
				glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures[0], 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}
		}

		if (m_msaa) {
			uint msaa_samples = ((flags >> 7) & 0xF) << 2;

			if( msaa_samples == 0 )
				throw std::runtime_error("Number of MSAA Samples not set. Please use SIBR_MSAA4X, SIBR_MSAA8X, SIBR_MSAA16X or SIBR_MSAA32X as an additional flag.");

			glGenTextures(1, &m_textures[0]);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textures[0]);
			CHECK_GL_ERROR;
			/// TODO: following causes enum compare warning -Wenum-compare
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
				msaa_samples,
				(flags & SIBR_GPU_INTEGER)
				? GLFormat<typename PixelFormat::Type, PixelFormat::NumComp>::int_internal_format
				: GLFormat<typename PixelFormat::Type, PixelFormat::NumComp>::internal_format,
				w, h,
				GL_TRUE
				);
			glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rb);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa_samples, GL_DEPTH_COMPONENT32, w, h);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_textures[0], 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_rb);
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			switch (status) {
			case GL_FRAMEBUFFER_UNSUPPORTED:
				throw std::runtime_error("Cannot create FBO - GL_FRAMEBUFFER_UNSUPPORTED error");
				break;
			default:
				SIBR_DEBUG(status);
				throw std::runtime_error("Cannot create FBO (unknow reason)");
				break;
			}
		}

		if (m_autoMIPMAP) {
			for (uint i=0; i<m_numtargets; i++) {
				glBindTexture(GL_TEXTURE_2D, m_textures[i]);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp>
	RenderTarget<T_Type, T_NumComp>::~RenderTarget(void) {
		for (uint i=0; i<m_numtargets; i++)
			glDeleteTextures(1, &m_textures[i]);
		glDeleteFramebuffers(1,&m_fbo);
		glDeleteRenderbuffers(1,&m_depth_rb);
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp>
	GLuint RenderTarget<T_Type, T_NumComp>::depthRB() const {
		return m_depth_rb;
	}

	template<typename T_Type, unsigned int T_NumComp>
	GLuint RenderTarget<T_Type, T_NumComp>::texture(uint t) const {
		SIBR_ASSERT(t<m_numtargets);
		return m_textures[t];
	}
	template<typename T_Type, unsigned int T_NumComp>
	GLuint RenderTarget<T_Type, T_NumComp>::handle(uint t) const {
		SIBR_ASSERT(t<m_numtargets);
		return m_textures[t];
	}

	template<typename T_Type, unsigned int T_NumComp>
	void RenderTarget<T_Type, T_NumComp>::bind(void) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		bool is_depth = (GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::isdepth != 0);
		if (!is_depth) {
			if (m_numtargets > 0) {
				GLenum drawbuffers[SIBR_MAX_SHADER_ATTACHMENTS];
				for (uint i=0; i<SIBR_MAX_SHADER_ATTACHMENTS; i++)
					drawbuffers[i] = GL_COLOR_ATTACHMENT0 + i;
				glDrawBuffers(m_numtargets, drawbuffers);
			}
		} else {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
	}

	template<typename T_Type, unsigned int T_NumComp>
	void RenderTarget<T_Type, T_NumComp>::unbind(void) {
		if (m_autoMIPMAP) {
			for (uint i=0; i<m_numtargets; i++) {
				glBindTexture(GL_TEXTURE_2D, m_textures[i]);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}

	template<typename T_Type, unsigned int T_NumComp>
	void RenderTarget<T_Type, T_NumComp>::clear( void ) {
		clear(PixelFormat());
	}

	/// \todo TODO : This function does not rescale values for uchar (so background is either 0 or 1)
	template<typename T_Type, unsigned int T_NumComp>
	void RenderTarget<T_Type, T_NumComp>::clear(const typename RenderTarget<T_Type, T_NumComp>::PixelFormat& v) {
		bind();
		if (PixelFormat::NumComp == 1) {
			glClearColor(v[0],0,0,0);
		} else if (PixelFormat::NumComp == 2) {
			glClearColor(v[0],v[1],0,0);
		} else if (PixelFormat::NumComp == 3) {
			glClearColor(v[0],v[1],v[2],0);
		} else if (PixelFormat::NumComp == 4) {
			glClearColor(v[0],v[1],v[2],v[3]);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		unbind();
	}

	template<typename T_Type, unsigned int T_NumComp>
	void RenderTarget<T_Type, T_NumComp>::clearStencil() {
		bind();
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		unbind();
	}

	template<typename T_Type, unsigned int T_NumComp>
	template <typename T_IType, uint N_INumComp>
	void RenderTarget<T_Type, T_NumComp>::readBack( sibr::Image<T_IType, N_INumComp>& img, uint target ) const {
		//void RenderTarget<T_Type, T_NumComp>::readBack(PixelImage& img, uint target) const {
		glFinish();
		if (target >= m_numtargets)
			SIBR_ERR << "Reading back texture out of bounds" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		bool is_depth = (GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::isdepth != 0);
		if (!is_depth) {
			if (m_numtargets > 0) {
				sibr::Image<T_Type, T_NumComp> buffer(m_W, m_H);

				GLenum drawbuffers = GL_COLOR_ATTACHMENT0 + target;
				glDrawBuffers(1, &drawbuffers);
				glReadBuffer(drawbuffers);

				glReadPixels(0, 0, m_W, m_H,
					GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::format,
					GLType<typename PixelFormat::Type>::type,
					buffer.data()
					);

				sibr::Image<T_IType, N_INumComp>	out;
				img.fromOpenCV(buffer.toOpenCV());
			}
		}
		else
			SIBR_ERR << "RenderTarget::readBack: This function should be specialized "
			"for handling depth buffer." << std::endl;
		img.flipH();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

/*/
		// IMPORTANT DEV NOTE:
		// If the following does not work (with your driver) try implementing
		// this function using glReadPixels() instead

		// makes sure Vertex have the correct size (read back relies on pointers)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT,   1);
		glBindTexture(GL_TEXTURE_2D, m_textures[target]);

		CHECK_GL_ERROR;

		int w, h;
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH, &w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);
		SIBR_ASSERT( uint(w*h) <= img.w()*img.h());
		glGetTexImage(GL_TEXTURE_2D,
			0,
			GLFormat<typename PixelFormat::Type,PixelFormat::NumComp>::format,
			GLType<typename PixelFormat::Type>::type,
			img.data()
			);

		// flip data vertically to get origin on lower left corner
		img.flipH();
		//for (uint j=0; j<img.height()/2; j++) {
		//	for (uint i=0; i<array.width(); i++) {
		//		typename PixelArray::t_Element tmp = array(i,j);
		//		array(i,j)=array(i,array.height()-1-j);
		//		array(i,array.height()-1-j) = tmp;
		//	}
		//}
//*/
	}


	template<typename T_Type, unsigned int T_NumComp>
	template <typename T_IType, uint N_INumComp>
	void RenderTarget<T_Type, T_NumComp>::readBackToCVmat(cv::Mat & img, uint target) const {

		using Infos = GLTexFormat<cv::Mat, T_IType, N_INumComp>;

		if (target >= m_numtargets)
			SIBR_ERR << "Reading back texture out of bounds" << std::endl;

		cv::Mat tmp(m_H, m_W, Infos::cv_type());

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		bool is_depth = (Infos::isdepth != 0);
		if (!is_depth) {
			if (m_numtargets > 0) {
				GLenum drawbuffers = GL_COLOR_ATTACHMENT0 + target;
				glDrawBuffers(1, &drawbuffers);
				glReadBuffer(drawbuffers);

				glReadPixels(0, 0, m_W, m_H,
					Infos::format,
					Infos::type,
					Infos::data(tmp)
				);
			}
		} else {
			SIBR_ERR << "RenderTarget::readBack: This function should be specialized "
				"for handling depth buffer." << std::endl; \
		}
		img = Infos::flip(tmp);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



	//template<>
	//template <typename T_IType, uint N_INumComp>
	//void RenderTarget<depth24, 1>::readBack( sibr::Image<T_IType, N_INumComp>& img, uint target ) const {
	//	readBackDepth(img, target);
	//}

	//template<>
	//template <typename T_IType, uint N_INumComp>
	//void RenderTarget<depth32, 1>::readBack( sibr::Image<T_IType, N_INumComp>& img, uint target ) const {
	//	readBackDepth(img, target);
	//}

	// ATTN: unclear whether consistent with readBack -- may require flip; not tested !
	template <typename TType, uint NNumComp>
	template <typename T_IType, uint N_INumComp>
	void RenderTarget<TType, NNumComp>::readBackDepth( sibr::Image<T_IType, N_INumComp>& image, uint target ) const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glReadBuffer(GL_COLOR_ATTACHMENT0);

		sibr::Image<float, 1> buffer(m_W, m_H);
		glReadPixels(0, 0, m_W, m_H,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			buffer.data()
			);

		sibr::Image<T_IType, N_INumComp>	out(buffer.w(), buffer.h());
		for (uint y = 0; y < buffer.h(); ++y)
			for (uint x = 0; x < buffer.w(); ++x)
				out.color(x, y, sibr::ColorRGBA(1, 1, 1, 1.f) * buffer(x, y)[0]);
		image = std::move(out);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	template<typename T_Type, unsigned int T_NumComp>
	uint   RenderTarget<T_Type, T_NumComp>::numTargets (void)  const { return m_numtargets;  }
	template<typename T_Type, unsigned int T_NumComp>
	uint   RenderTarget<T_Type, T_NumComp>::w          (void)  const { return m_W;           }
	template<typename T_Type, unsigned int T_NumComp>
	uint   RenderTarget<T_Type, T_NumComp>::h          (void)  const { return m_H;           }
	template<typename T_Type, unsigned int T_NumComp>
	uint   RenderTarget<T_Type, T_NumComp>::fbo		   (void)  const { return m_fbo; }

	/**
	* \ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void			blit(const ITexture2D& src, const ITexture2D& dst, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);

	/**
	* \ingroup sibr_graphics
	*/
	class ITexture2DArray
	{
	public:
		typedef std::shared_ptr<ITexture2DArray>	Ptr;
		typedef std::unique_ptr<ITexture2DArray>	UPtr;
	public:
		virtual ~ITexture2DArray(void) { }

		virtual GLuint	handle(void) const = 0;
		virtual uint	w(void) const = 0;
		virtual uint	h(void) const = 0;
		virtual uint	depth(void) const = 0;
	};

	/**
	* \ingroup sibr_graphics
	*/
	template<typename T_Type, unsigned int T_NumComp>
	class Texture2DArray : public ITexture2DArray {
		SIBR_DISALLOW_COPY(Texture2DArray);
	public:
		typedef		Image<T_Type, T_NumComp>			PixelImage;
		typedef		typename PixelImage::Pixel			PixelFormat;
		typedef		RenderTarget<T_Type, T_NumComp>			PixelRT;
		typedef		std::shared_ptr<Texture2DArray<T_Type, T_NumComp>>	Ptr;
		typedef		std::unique_ptr<Texture2DArray<T_Type, T_NumComp>>	UPtr;

	private:
		GLuint  m_Handle = 0;
		uint    m_W = 0;
		uint    m_H = 0;
		uint    m_Flags = 0;
		uint	m_Depth = 0;

	public:

		Texture2DArray(const uint d = 0, uint flags = 0);

		Texture2DArray(const uint w, const uint h, const uint d, uint flags = 0);

		Texture2DArray(const std::vector<typename PixelRT::Ptr> & images, uint flags = 0);

		template<typename ImageType>
		Texture2DArray(const std::vector<ImageType> & images, uint flags = 0);

		template<typename ImageType>
		Texture2DArray(const std::vector<ImageType> & images, uint w, uint h, uint flags = 0);

		/// Create 2D texture and send it to GPU
		template<typename ImageType>
		void createFromImages(const std::vector<ImageType> & images, uint flags = 0);

		/// Create 2D texture and send it to GPU
		template<typename ImageType>
		void createFromImages(const std::vector<ImageType> & images, uint w, uint h, uint flags = 0);

		template<typename ImageType>
		void updateFromImages(const std::vector<ImageType> & images);

		void createFromRTs(const std::vector<typename PixelRT::Ptr> & RTs, uint flags = 0);

		template<typename ImageType>
		void updateSlices(const std::vector<ImageType> & images, const std::vector<int> & slices);

		~Texture2DArray(void);

		GLuint	handle(void) const;
		uint	w(void) const;
		uint	h(void) const;
		uint	depth(void) const;

	private:
		void createArray();

		template<typename ImageType>
		void sendArray(const std::vector<ImageType> & images);

		void sendArray(const std::vector<typename PixelRT::Ptr> & RTs);

		template<typename ImageType>
		std::vector<const ImageType*> applyFlipAndResize(
			const std::vector<ImageType> & images,
			std::vector<ImageType> & tmp,
			const std::vector<int> & slices
		);

		template<typename ImageType>
		std::vector<const ImageType*> applyFlipAndResize(
			const std::vector<ImageType> & images,
			std::vector<ImageType> & tmp
		);
	};

	// -----------------------------------------------------------------------

	typedef Texture2DArray<unsigned char, 1>     Texture2DArrayLum;
	typedef Texture2DArray<unsigned char, 3>     Texture2DArrayRGB;
	typedef Texture2DArray<unsigned char, 4>     Texture2DArrayRGBA;

	typedef Texture2DArray<unsigned short, 1>    Texture2DArrayLum16;
	typedef Texture2DArray<unsigned short, 2>    Texture2DArrayUV16;
	typedef Texture2DArray<unsigned short, 4>    Texture2DArrayRGBA16;

	typedef Texture2DArray<short, 2>             Texture2DArrayUV16s;

	typedef Texture2DArray<float, 1>             Texture2DArrayLum32F;
	typedef Texture2DArray<float, 3>             Texture2DArrayRGB32F;
	typedef Texture2DArray<float, 4>             Texture2DArrayRGBA32F;

	template<typename T_Type, unsigned int T_NumComp>
	Texture2DArray<T_Type, T_NumComp>::Texture2DArray(const uint d, uint flags) {
		m_Depth = d;
		m_Flags = flags;
	}

	template<typename T_Type, unsigned int T_NumComp>
	Texture2DArray<T_Type, T_NumComp>::Texture2DArray(const uint w, const uint h, const uint d, uint flags) {
		m_W = w;
		m_H = h;
		m_Depth = d;
		m_Flags = flags;
		createArray();
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	Texture2DArray<T_Type, T_NumComp>::Texture2DArray(const std::vector<ImageType> & images, uint flags) {
		m_Flags = flags;
		createFromImages(images,flags);
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	Texture2DArray<T_Type, T_NumComp>::Texture2DArray(const std::vector<ImageType> & images, uint w, uint h, uint flags) {
		m_Flags = flags;
		createFromImages(images, w, h, flags);
	}

	template<typename T_Type, unsigned int T_NumComp>
	Texture2DArray<T_Type, T_NumComp>::Texture2DArray(const std::vector<typename PixelRT::Ptr> & RTs, uint flags) {
		m_Flags = flags;
		createFromRTs(RTs, flags);
	}

	template<typename T_Type, unsigned int T_NumComp>
	void Texture2DArray<T_Type, T_NumComp>::createArray() {
		CHECK_GL_ERROR;
		glGenTextures(1, &m_Handle);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);

		if (m_Flags & SIBR_GPU_LINEAR_SAMPLING) {
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		const bool autoMIPMAP = ((m_Flags & SIBR_GPU_AUTOGEN_MIPMAP) != 0);
		const int numMipMap = autoMIPMAP ? (int)std::floor(std::log2(std::max(m_W, m_H))) : 1;

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, numMipMap,
			GLFormat<T_Type, T_NumComp>::internal_format,
			m_W,
			m_H,
			m_Depth
		);
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	void Texture2DArray<T_Type, T_NumComp>::sendArray(const std::vector<ImageType> & images) {
		using ImgTypeInfo = GLTexFormat<ImageType, T_Type, T_NumComp>;
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		// Make sure all images have the same size.
		std::vector<ImageType> tmp;
		std::vector<const ImageType*> imagesPtrToSend = applyFlipAndResize(images,tmp);

		for (int im = 0; im < (int)m_Depth; ++im) {
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
				0,
				0, 0, im,
				m_W,
				m_H,
				1, // one slice at a time
				ImgTypeInfo::format,
				ImgTypeInfo::type,
				ImgTypeInfo::data(*imagesPtrToSend[im])
			);
			//CHECK_GL_ERROR;
		}
		bool autoMIPMAP = ((m_Flags & SIBR_GPU_AUTOGEN_MIPMAP) != 0);
		if (autoMIPMAP) {
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		}
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	std::vector<const ImageType*> Texture2DArray<T_Type, T_NumComp>::applyFlipAndResize(
		const std::vector<ImageType>& images,
		std::vector<ImageType>& tmp,
		const std::vector<int>& slices ) 
	{
		using ImgTypeInfo = GLTexFormat<ImageType, T_Type, T_NumComp>;

		std::vector<const ImageType*> imagesPtrToSend(images.size());
		tmp.resize(images.size());

		bool flip = m_Flags & SIBR_FLIP_TEXTURE;
		//#pragma omp parallel for // Disabled due to performance reasons when live-updating slices.
		for (int slice_id = 0; slice_id < (int)slices.size(); ++slice_id) {
			int im = slices[slice_id];
			
			bool resize = !(m_W == ImgTypeInfo::width(images[im]) && m_H == ImgTypeInfo::height(images[im]));
			if (!flip && !resize) {
				imagesPtrToSend[im] = &images[im];
			} else {
				if (resize) {
					tmp[im] = ImgTypeInfo::resize(images[im], m_W, m_H);
				}
				if (flip) {
					tmp[im] = ImgTypeInfo::flip(resize ? tmp[im] : images[im]);
				}
				imagesPtrToSend[im] = &tmp[im];
			}
		}

		return imagesPtrToSend;
	}

	template<typename T_Type, unsigned int T_NumComp>
	template<typename ImageType>
	std::vector<const ImageType*> Texture2DArray<T_Type, T_NumComp>::applyFlipAndResize(
		const std::vector<ImageType>& images,
		std::vector<ImageType>& tmp
	) {
		std::vector<int> slices(m_Depth);
		for (int i = 0; i < (int)m_Depth; ++i) {
			slices[i] = i;
		}
		return applyFlipAndResize(images, tmp, slices);
	}

	template<typename T_Type, unsigned int T_NumComp>
	void Texture2DArray<T_Type, T_NumComp>::sendArray(const std::vector<typename PixelRT::Ptr> & RTs) {
		CHECK_GL_ERROR;
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		for (int im = 0; im < (int)m_Depth; ++im) {
			// Set correct RT as read-framebuffer.

			RTs[im]->bind();
			glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY,
				0,
				0, 0, im,
				0, 0,
				m_W,
				m_H
			);
			RTs[im]->unbind();
		}
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	void Texture2DArray<T_Type, T_NumComp>::createFromImages(const std::vector<ImageType> & images, uint flags) {
		using ImgTypeInfo = GLTexFormat<ImageType,T_Type,T_NumComp>;

		sibr::Vector2u maxSize(0, 0);
		for (const auto & img : images) {
			maxSize = maxSize.cwiseMax(sibr::Vector2u(ImgTypeInfo::width(img),ImgTypeInfo::height(img)));
		}
		createFromImages(images, maxSize[0], maxSize[1], flags);
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	void Texture2DArray<T_Type, T_NumComp>::createFromImages(const std::vector<ImageType> & images, uint w, uint h, uint flags) {
		using ImgTypeInfo = GLTexFormat<ImageType, T_Type, T_NumComp>;
		m_W = w;
		m_H = h;
		m_Depth = (uint)images.size();
		m_Flags = flags;
		createArray();
		sendArray(images);
	}

	template<typename T_Type, unsigned int T_NumComp> template<typename ImageType>
	void Texture2DArray<T_Type, T_NumComp>::updateFromImages(const std::vector<ImageType> & images) {
		using ImgTypeInfo = GLTexFormat<ImageType, T_Type, T_NumComp>;
		sibr::Vector2u maxSize(0, 0);
		for (const auto & img : images) {
			maxSize = maxSize.cwiseMax(sibr::Vector2u(ImgTypeInfo::width(img), ImgTypeInfo::height(img)));
		}
		if (images.size() == m_Depth && m_W == maxSize[0] && m_H == maxSize[1]) {
			sendArray(images);
		} else {
			createFromImages(images, m_Flags);
		}
	}

	template<typename T_Type, unsigned int T_NumComp>  template<typename ImageType>
	void Texture2DArray<T_Type, T_NumComp>::updateSlices(const std::vector<ImageType> & images, const std::vector<int>& slices) {
		using ImgTypeInfo = GLTexFormat<ImageType, T_Type, T_NumComp>;

		int numSlices = (int)slices.size();
		if (numSlices == 0) {
			return;
		}

		sibr::Vector2u maxSize(0, 0);
		for (int i = 0; i < numSlices; ++i) {
			maxSize = maxSize.cwiseMax(sibr::Vector2u(ImgTypeInfo::width(images[slices[i]]), ImgTypeInfo::height(images[slices[i]])));
		}
		if ( m_W != maxSize[0] || m_H != maxSize[1]) {
			m_W = maxSize[0];
			m_H = maxSize[1];
			createArray();
		}

		glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		std::vector<ImageType> tmp;
		std::vector<const ImageType*> imagesPtrToSend = applyFlipAndResize(images, tmp, slices);

		for (int i = 0; i < numSlices; ++i) {
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
				0,
				0, 0, slices[i],
				m_W,
				m_H,
				1, // one slice at a time
				ImgTypeInfo::format,
				ImgTypeInfo::type,
				ImgTypeInfo::data(*imagesPtrToSend[slices[i]])
			);
		}
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp>
	void Texture2DArray<T_Type, T_NumComp>::createFromRTs(const std::vector<typename PixelRT::Ptr> & RTs, uint flags) {
		m_W = 0;
		m_H = 0;
		for (const auto & RT : RTs) {
			m_W = (std::max)(m_W, RT->w());
			m_H = (std::max)(m_H, RT->h());
		}
		m_Depth = (uint)RTs.size();
		m_Flags = flags;
		createArray();
		sendArray(RTs);
	}

	template<typename T_Type, unsigned int T_NumComp>
	Texture2DArray<T_Type, T_NumComp>::~Texture2DArray(void) {
		CHECK_GL_ERROR;
		glDeleteTextures(1, &m_Handle);
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp>
	GLuint Texture2DArray<T_Type, T_NumComp>::handle(void) const { return m_Handle; }
		
	template<typename T_Type, unsigned int T_NumComp>
	uint Texture2DArray<T_Type, T_NumComp>::w(void) const { return m_W; }

	template<typename T_Type, unsigned int T_NumComp>
	uint Texture2DArray<T_Type, T_NumComp>::h(void) const { return m_H; }

	template<typename T_Type, unsigned int T_NumComp>
	uint Texture2DArray<T_Type, T_NumComp>::depth(void) const { return m_Depth; }
	



	/**
	* \ingroup sibr_graphics
	*/
	class ITextureCubeMap
	{
	public:
		typedef std::shared_ptr<ITextureCubeMap>	Ptr;
		typedef std::unique_ptr<ITextureCubeMap>	UPtr;
	public:
		virtual ~ITextureCubeMap(void) { }

		virtual GLuint	handle(void) const = 0;
		virtual uint	w(void) const = 0;
		virtual uint	h(void) const = 0;
	};


	/**
	* \ingroup sibr_graphics
	*/
	template<typename T_Type, unsigned int T_NumComp>
	class TextureCubeMap : public ITextureCubeMap {
		SIBR_DISALLOW_COPY(TextureCubeMap);

	public:
		typedef		Image<T_Type, T_NumComp>			PixelImage;
		typedef		typename PixelImage::Pixel			PixelFormat;
		typedef		RenderTarget<T_Type, T_NumComp>			PixelRT;
		typedef		std::shared_ptr<TextureCubeMap<T_Type, T_NumComp>>	Ptr;
		typedef		std::unique_ptr<TextureCubeMap<T_Type, T_NumComp>>	UPtr;

	private:
		GLuint  m_Handle = 0;
		uint    m_W = 0;
		uint    m_H = 0;
		uint    m_Flags = 0;

	public:

		TextureCubeMap(void);
		TextureCubeMap(const uint w, const uint h, uint flags = 0);
		TextureCubeMap(	const PixelImage& xpos, const PixelImage& xneg,
			const PixelImage& ypos, const PixelImage& yneg,
			const PixelImage& zpos, const PixelImage& zneg, uint flags = 0);
		

		/// Create 2D texture and send it to GPU
		void createFromImages(const PixelImage& xpos, const PixelImage& xneg,
			const PixelImage& ypos, const PixelImage& yneg,
			const PixelImage& zpos, const PixelImage& zneg, uint flags = 0);


		~TextureCubeMap(void);

		GLuint	handle(void) const;
		uint	w(void) const;
		uint	h(void) const;

	private:
		void createCubeMap();
		void sendCubeMap(const PixelImage& xpos, const PixelImage& xneg,
			const PixelImage& ypos, const PixelImage& yneg,
			const PixelImage& zpos, const PixelImage& zneg);
	};

	// -----------------------------------------------------------------------

	typedef TextureCubeMap<unsigned char, 1>    TextureCubeMapLum;
	typedef TextureCubeMap<unsigned char, 3>    TextureCubeMapRGB;
	typedef TextureCubeMap<unsigned char, 4>    TextureCubeMapRGBA;

	typedef TextureCubeMap<unsigned short, 1>   TextureCubeMapLum16;
	typedef TextureCubeMap<unsigned short, 2>   TextureCubeMapUV16;
	typedef TextureCubeMap<unsigned short, 4>   TextureCubeMapRGBA16;

	typedef TextureCubeMap<short, 2>            TextureCubeMapUV16s;

	typedef TextureCubeMap<float, 1>            TextureCubeMapLum32F;
	typedef TextureCubeMap<float, 3>            TextureCubeMapRGB32F;
	typedef TextureCubeMap<float, 4>            TextureCubeMapRGBA32F;

	template<typename T_Type, unsigned int T_NumComp>
	TextureCubeMap<T_Type, T_NumComp>::TextureCubeMap(void) {}

	template<typename T_Type, unsigned int T_NumComp>
	TextureCubeMap<T_Type, T_NumComp>::TextureCubeMap(const uint w, const uint h, uint flags) {
		m_W = w;
		m_H = h;
		m_Flags = flags;
		createCubeMap();
	}

	template<typename T_Type, unsigned int T_NumComp>
	TextureCubeMap<T_Type, T_NumComp>::TextureCubeMap(const PixelImage& xpos, const PixelImage& xneg,
		const PixelImage& ypos, const PixelImage& yneg,
		const PixelImage& zpos, const PixelImage& zneg, uint flags) {
		m_Flags = flags;
		createFromImages(xpos, xneg, ypos, yneg, zpos, zneg, flags);
	}

	
	template<typename T_Type, unsigned int T_NumComp>
	void TextureCubeMap<T_Type, T_NumComp>::createCubeMap() {

		// We enable seamless junctions between cubemap faces.
		static bool enableStates = false;
		if (enableStates == false)
		{
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			enableStates = true;
		}
		CHECK_GL_ERROR;

		glGenTextures(1, &m_Handle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);

		if (m_Flags & SIBR_GPU_LINEAR_SAMPLING) {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		CHECK_GL_ERROR;
	}

	

	template<typename T_Type, unsigned int T_NumComp>
	void TextureCubeMap<T_Type, T_NumComp>::sendCubeMap(const PixelImage& xpos, const PixelImage& xneg,
		const PixelImage& ypos, const PixelImage& yneg,
		const PixelImage& zpos, const PixelImage& zneg) {
		CHECK_GL_ERROR;

		if (m_Flags & SIBR_GPU_INTEGER) {
			if (GLFormat<typename PixelFormat::Type, PixelFormat::NumComp>::int_internal_format < 0) {
				throw std::runtime_error("Texture format does not support integer mapping");
			}
		}

		// Handle flipping.
		const PixelImage * sendedXpos = &xpos;
		const PixelImage * sendedYpos = &ypos;
		const PixelImage * sendedZpos = &zpos;
		const PixelImage * sendedXneg = &xneg;
		const PixelImage * sendedYneg = &yneg;
		const PixelImage * sendedZneg = &zneg;

		PixelImage flippedXpos, flippedYpos, flippedZpos;
		PixelImage flippedXneg, flippedYneg, flippedZneg;
		
		// ...
		if (m_Flags & SIBR_FLIP_TEXTURE) {
			flippedXpos = xpos.clone();
			flippedXpos.flipH();
			sendedXpos = &flippedXpos;
			
			flippedYpos = ypos.clone();
			flippedYpos.flipH();
			sendedYpos = &flippedYpos;
			
			flippedZpos = zpos.clone();
			flippedZpos.flipH();
			sendedZpos = &flippedZpos;

			flippedXneg = xneg.clone();
			flippedXneg.flipH();
			sendedXneg = &flippedXneg;

			flippedYneg = yneg.clone();
			flippedYneg.flipH();
			sendedYneg = &flippedYneg;

			flippedZneg = zneg.clone();
			flippedZneg.flipH();
			sendedZneg = &flippedZneg;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);

		const auto tinternal_format = (m_Flags & SIBR_GPU_INTEGER)
			? GLFormat<typename PixelFormat::Type, PixelFormat::NumComp>::int_internal_format
			: GLFormat<typename PixelFormat::Type, PixelFormat::NumComp>::internal_format;
		const auto tformat = (m_Flags & SIBR_GPU_INTEGER)
			? GLFormat<typename PixelFormat::Type, PixelFormat::NumComp>::int_format
			: GLFormat<typename PixelFormat::Type, PixelFormat::NumComp>::format;
		const auto ttype = GLType<typename PixelFormat::Type>::type;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, tinternal_format, xpos.w(), xpos.h(), 0, tformat, ttype, xpos.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, tinternal_format, xneg.w(), xneg.h(), 0, tformat, ttype, xneg.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, tinternal_format, ypos.w(), ypos.h(), 0, tformat, ttype, ypos.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, tinternal_format, yneg.w(), yneg.h(), 0, tformat, ttype, yneg.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, tinternal_format, zpos.w(), zpos.h(), 0, tformat, ttype, zpos.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, tinternal_format, zneg.w(), zneg.h(), 0, tformat, ttype, zneg.data());


		bool autoMIPMAP = ((m_Flags & SIBR_GPU_AUTOGEN_MIPMAP) != 0);
		if (autoMIPMAP) {
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
		
	
	}


	template<typename T_Type, unsigned int T_NumComp>
	void TextureCubeMap<T_Type, T_NumComp>::createFromImages(const PixelImage& xpos, const PixelImage& xneg,
		const PixelImage& ypos, const PixelImage& yneg,
		const PixelImage& zpos, const PixelImage& zneg, uint flags) {
		const int numMipMap = 1;
		sibr::Vector2u maxSize(0, 0);
		/// \todo TODO: check if the six images have the same size.
		m_W = xpos.w();
		m_H = xpos.h();
		m_Flags = flags;
		createCubeMap();
		sendCubeMap(xpos, xneg, ypos, yneg, zpos, zneg);
	}


	template<typename T_Type, unsigned int T_NumComp>
	TextureCubeMap<T_Type, T_NumComp>::~TextureCubeMap(void) {
		CHECK_GL_ERROR;
		glDeleteTextures(1, &m_Handle);
		CHECK_GL_ERROR;
	}

	template<typename T_Type, unsigned int T_NumComp>
	GLuint TextureCubeMap<T_Type, T_NumComp>::handle(void) const { return m_Handle; }

	template<typename T_Type, unsigned int T_NumComp>
	uint TextureCubeMap<T_Type, T_NumComp>::w(void) const { return m_H; }

	template<typename T_Type, unsigned int T_NumComp>
	uint TextureCubeMap<T_Type, T_NumComp>::h(void) const { return m_W; }

} // namespace sibr



#endif // __SIBR_GRAPHICS_TEXTURE_HPP__
