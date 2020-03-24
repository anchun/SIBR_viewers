
#ifndef __SIBR_MESHBUFFERGL_HPP__
# define __SIBR_MESHBUFFERGL_HPP__

# include <array>
# include <vector>
# include "core/graphics/Config.hpp"


namespace sibr
{
	class Mesh;

	/**
	* This class is used to render mesh. It act like a vertex buffer object
	* (in reality there also a vertex array object and maybe other data).
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT MeshBufferGL
	{
	public:

		/** Predefined shader attribute location. */
		enum AttribLocation
		{
			VertexAttribLocation	= 0,
			ColorAttribLocation		= 1,
			TexCoordAttribLocation	= 2,
			NormalAttribLocation	= 3,

			AttribLocationCount
		};
		
		/** Predefined buffer location. */
		enum
		{
			BUFINDEX 	= 0,
			BUFVERTEX 	= 1,

			BUFCOUNT
		};

	public:

		/// Constructor.
		MeshBufferGL( void );

		/// Destructor.
		~MeshBufferGL( void );

		/// Move constructor.
		MeshBufferGL( MeshBufferGL&& other );

		/// Move operator.
		MeshBufferGL& operator =( MeshBufferGL&& other );
		
		/** Build from a mesh so you can then draw() it to render it.
		* \param mesh the mesh to upload
		* \note This function can't fail (errors stop the program with a message).
		*/
		void	build( const Mesh& mesh );

		/** Delete the GPU buffer, freeing memory. */
		void	free(void);

		/** This bind and draw elements stored in the buffer. */
		void	draw(void) const;

		/** This bind and draw elements in [begin, end[ stored in the buffer.
			\param begin ID of the first primitive to render
			\param end ID after the last primitive to render
		*/
		void	draw(unsigned int begin, unsigned int end) const;

		/** This bind and draw elements stored in the buffer with tessellation shader enabled. */
		void  drawTessellated(void) const;
		
		/** This bind and draw elements stored in the buffer, using pairs of indices to draw lines. */
		void draw_lines(void) const;

		/** This bind and draw vertex points stored in the buffer. */
		void	draw_points( void ) const;

		/** This bind and draw vertex points in [begin, end[ stored in the buffer.
			\param begin ID of the first primitive to render
			\param end ID after the last primitive to render
		*/
		void	draw_points( unsigned int begin, unsigned int end ) const;

		/** Bind the vertex and index buffers. */
		void	bind(void) const;

		/** Bind only indexes (useful for combining with other form of mesh. e.g. SlicedMesh) */
		void	bindIndices(void) const;

		/** Unbind arrays and buffers. */
		void	unbind(void) const;

		/** Copy constructor (disabled). */
		MeshBufferGL(const MeshBufferGL&) = delete;

		/** Copy operator (disabled). */
		MeshBufferGL& operator =(const MeshBufferGL&) = delete;

	private:
		
		GLuint 							_vaoId; ///< Vertex array object ID.
		std::array<GLuint, BUFCOUNT>	_bufferIds; ///< Buffers IDs.
		uint 							_indexCount; ///< Number of elements in the index buffer.
		uint							_vertexCount; ///< Number of elements in the vertex buffer.
	};

	///// DEFINITION /////


} // namespace sibr

#endif // __SIBR_MESHBUFFERGL_HPP__
