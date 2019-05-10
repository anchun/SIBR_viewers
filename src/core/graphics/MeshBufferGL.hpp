
#ifndef __SIBR_MESHBUFFERGL_HPP__
# define __SIBR_MESHBUFFERGL_HPP__

# include <array>
# include <vector>
# include "core/graphics/Config.hpp"


namespace sibr
{
	class Mesh;

	///
	/// This class is used to render mesh. It act like a vertex buffer object
	/// (in reality there also a vertex array object and maybe other data).
	/// \ingroup sibr_graphics
	class SIBR_GRAPHICS_EXPORT MeshBufferGL
	{
	public:

		enum AttribLocation
		{
			VertexAttribLocation	= 0,
			ColorAttribLocation		= 1,
			TexCoordAttribLocation	= 2,
			NormalAttribLocation	= 3,

			AttribLocationCount
		};
		
		enum
		{
			BUFINDEX 	= 0,
			BUFVERTEX 	= 1,

			BUFCOUNT
		};

	public:

		MeshBufferGL( void );
		~MeshBufferGL( void );
		MeshBufferGL( MeshBufferGL&& other );
		MeshBufferGL& operator =( MeshBufferGL&& other );
		
		/// Build from a mesh so you can then draw() it to render it.
		/// This function can't fail (errors stop the program with
		/// a message).
		void	build( const Mesh& mesh );

		/// Delete buffer (useful for freeing GPU memory)
		void	free(void);

		/// This bind and draw elements stored in the buffer.
		void	draw(void) const;

		/// This bind and draw elements in [begin, end[ stored in the buffer.
		void	draw(unsigned int begin, unsigned int end) const;

		/// This bind and draw elements stored in the buffer with tessellation shader.
		void  MeshBufferGL::drawTessellated(void) const;
		
		/// This bind and draw elements stored in the buffer, using pairs of indices to draw lines.
		void draw_lines(void) const;

		/// This bind and draw vertex points stored in the buffer.
		void	draw_points( void ) const;

		/// This bind and draw vertex points in [begin, end[ stored in the buffer.
		void	draw_points( unsigned int begin, unsigned int end ) const;

		void	bind(void) const;

		/// Bind only indexes (useful for combining with other form of mesh. e.g. SlicedMesh)
		void	bindIndices(void) const;

		void	unbind(void) const;

	private:
		MeshBufferGL( const MeshBufferGL& );				// disallowed
		MeshBufferGL& operator =( const MeshBufferGL& );	// disallowed
		
		GLuint 							_vaoId;
		std::array<GLuint, BUFCOUNT>	_bufferIds;
		uint 							_indexCount;
		uint							_vertexCount;
	};

	///// DEFINITION /////


} // namespace sibr

#endif // __SIBR_MESHBUFFERGL_HPP__
