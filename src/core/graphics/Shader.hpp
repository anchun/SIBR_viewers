
#ifndef __SIBR_GRAPHICS_SHADER_HPP__
# define __SIBR_GRAPHICS_SHADER_HPP__

# include <vector>
# include <string>
# include "core/graphics/Config.hpp"
# include "core/system/Matrix.hpp"

#define SIBR_SHADER(version, shader)  std::string("#version " #version "\n" #shader)

namespace sibr
{
	//class Vector2f;
	//class Vector3f;
	//class Vector4f;
	//class Matrix4f;

	/**
	  OpenGL shader wrapper.
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT GLShader
	{
		SIBR_DISALLOW_COPY( GLShader );
		SIBR_CLASS_PTR(GLShader);
	public:
		struct Define
		{
			typedef std::vector<Define>	List;

			template <typename TValue>
			Define( const std::string& nameToSearch_, const TValue& valueToSet_ ) :
				nameToSearch(nameToSearch_) {
					std::ostringstream oss;
					oss << valueToSet_;
					valueToSet = oss.str();
			}

			std::string nameToSearch;
			std::string valueToSet;
		};

	public:
		GLShader( void );
		~GLShader( void );

		bool init(std::string name,
			std::string vp_code, std::string fp_code,
			std::string gp_code = std::string(),
			bool exitOnError = true,
			std::string tcs_code = std::string(),
			std::string tes_code = std::string());

		bool reload(
			std::string vp_code, std::string fp_code,
			std::string gp_code = std::string());

		void getBinary(std::vector<char> & binary);

		SIBR_OPT_INLINE		void	begin( void );
		SIBR_OPT_INLINE		void	end( void );

		void			init      ( GLuint s_handle );
		void			terminate( void );
		void			setStrict ( bool s );
		GLuint			shader    ( void )  const;
		std::string		name      ( void )  const;
		bool			isReady   ( void )  const;
		bool			isActive  ( void )  const;
		bool			isStrict  ( void )  const;

	private:
		GLuint	compileShader( const char* shader_code, GLuint type );
		SIBR_OPT_INLINE		void	authorize( void ) const;

		GLuint      m_Shader;
		std::string m_Name;
		bool        m_Strict;
		bool        m_Active;
	};

	// ------------------------------------------------------------------------


	/**
	  OpenGL shader uniform wrapper.
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT GLParameter
	{
	public:
		GLParameter( void );

		void	init( sibr::GLShader& shader, std::string name );
		bool	isInitialized( void ) const;

		GLint	handle( void ) const;

		SIBR_OPT_INLINE void	set( float f );
		SIBR_OPT_INLINE void	set( float a, float b );
		SIBR_OPT_INLINE void	set( float a, float b, float c );
		SIBR_OPT_INLINE void	set( float a, float b, float c, float d );
		SIBR_OPT_INLINE void	set( const float *matrix );
		SIBR_OPT_INLINE void	set( GLuint tex );
		SIBR_OPT_INLINE void	set( int v );
		SIBR_OPT_INLINE void	set( bool b);
		SIBR_OPT_INLINE void	setArray( const float *pv, int size );
		SIBR_OPT_INLINE void	set( const Vector2f& v );
		SIBR_OPT_INLINE void	set( const Vector3f& v );
		SIBR_OPT_INLINE void	set( const Vector4f& v );
		SIBR_OPT_INLINE void	set( const Matrix4f& m );
		SIBR_OPT_INLINE void setMatrixArray(const float * m, int num);
	private:
		sibr::GLShader*		m_Shader;
		GLint				m_Handle;
		std::string			m_Name;
		bool				m_Strict;

		SIBR_OPT_INLINE void	authorize( void ) const;
	};

	/** overload of loadFile() from sibr_system that allows you to set value for
	*  define in shaders. */
	SIBR_GRAPHICS_EXPORT std::string	loadFile( const std::string& filename, const GLShader::Define::List& defines );

	template<typename T> class GLuniform {

	public:
		/// \todo TT : check this doesnt break everything
		GLuniform(const T & t) : value(t) {}

		operator T & () { return value; }
		T & get() { return value; }
		T & operator=(const T & t) { value = t; return *this; }
		
		GLuniform & operator=(const GLuniform& other) { value = other.value; return *this; };
		explicit GLuniform(const GLuniform&other) : value(other.value) { };
		GLuniform() = default;

		void init(sibr::GLShader& shader, std::string name) {
			parameter.init(shader, name);
		}

		void send() {
			if (parameter.isInitialized()) {
				parameter.set(value);
			}
		}

		void set(const T & t) {
			value = t;
			send();
		}

	protected :
		T value;
		sibr::GLParameter parameter;
	};

} // namespace sibr

# ifdef SIBR_MAXIMIZE_INLINE
#  include "Shader.inl"
# endif

#endif // __SIBR_GRAPHICS_SHADER_HPP__
