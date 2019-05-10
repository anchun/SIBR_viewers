
#include "core/graphics/Mesh.hpp"
#include "core/graphics/MeshBufferGL.hpp"

namespace sibr
{
	template <typename TTo, typename TFrom>
	static std::vector<TTo> 	prepareVertexData( 
		const std::vector<TFrom>& fromData, uint numVertices )
	{
		static_assert(sizeof(TFrom) >= sizeof(TTo),
			"Conversion not automatically managed with these two types");
		/*constexpr*/ const uint typeSize = sizeof(TFrom)/sizeof(TTo);
		static_assert((typeSize*sizeof(TTo)) == sizeof(TFrom),
			"Provided types have not a correct size");	

		if (fromData.empty())
			return std::vector<TTo>(); // empty
		//// If no data available, generate null ones
		//if (fromData.empty())
		//	return std::vector<TTo>(numIndices*typeSize, 0);

		// We are supposed to have ONE data element per Vertex
		SIBR_ASSERT(fromData.size() == numVertices);
		
		const TTo*	beginptr = reinterpret_cast<const TTo*>(fromData.data());
		const TTo*	endptr = beginptr + (fromData.size()*typeSize);
		return std::vector<TTo>(beginptr, endptr);
	}

	template <typename TTo, typename TFrom>
	static void 	appendVertexData( 
		std::vector<TTo>& toData,
		const std::vector<TFrom>& fromData)
	{
		if (fromData.empty())
			return;

		/*constexpr*/ const uint typeSize = sizeof(TFrom)/sizeof(TTo);
		static_assert((typeSize*sizeof(TTo)) == sizeof(TFrom),
			"Provided types have not a correct size");	

		const TTo* beginptr = reinterpret_cast<const TTo*>(fromData.data());
		const TTo* endptr = beginptr + (fromData.size()*typeSize);
		toData.insert(toData.end(), beginptr, endptr);
	}

	template <typename T>
	static inline uint 	getVectorDataSize( const std::vector<T>& v )
	{
		return (uint)(sizeof(T)*v.size());
	}
	//===========================================================================

	MeshBufferGL::MeshBufferGL( void )
		: _vaoId(0), _indexCount(0), _vertexCount(0)
	{
		_bufferIds.fill(0);
	}

	MeshBufferGL::~MeshBufferGL( void )
	{
		free();
	}

	MeshBufferGL::MeshBufferGL( MeshBufferGL&& other ) :
		_vaoId		(std::move(other._vaoId)),
		_bufferIds	(std::move(other._bufferIds)),
		_indexCount	(std::move(other._indexCount)),
		_vertexCount(std::move(other._vertexCount))
	{
	}

	MeshBufferGL& MeshBufferGL::operator =( MeshBufferGL&& other )
	{
		_vaoId		= std::move(other._vaoId);
		_bufferIds	= std::move(other._bufferIds);
		_indexCount	= std::move(other._indexCount);
		_vertexCount= std::move(other._vertexCount);

		return *this;
	}

	void 	MeshBufferGL::build( const Mesh& mesh )
	{
		if (!_vaoId)
		{
			glGenVertexArrays(1, &_vaoId);
			glGenBuffers(2, &_bufferIds[BUFINDEX]);
		}

		glBindVertexArray(_vaoId);
		
		CHECK_GL_ERROR

		// Create buffer for indices (called elements in opengl)

		std::vector<GLuint>   indices(mesh.triangleArray(), mesh.triangleArray() + mesh.triangles().size()*3);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferIds[BUFINDEX]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW);
		_indexCount = (uint)indices.size();

		CHECK_GL_ERROR

		uint numVertices = (uint)mesh.vertices().size();
		_vertexCount = numVertices;
		//SIBR_DEBUG(mesh.triangles().size());
		std::vector<GLfloat> vertices = prepareVertexData<GLfloat>(
			mesh.vertices(), numVertices);
		std::vector<GLfloat> colors = prepareVertexData<GLfloat>(
			mesh.colors(), numVertices);
		std::vector<GLfloat> texcoords = prepareVertexData<GLfloat>(
			mesh.texCoords(), numVertices);
		std::vector<GLfloat> normals = prepareVertexData<GLfloat>(
			mesh.normals(), numVertices);

		// Following this order:
		//VertexAttribLocation		= 0,
		//ColorAttribLocation		= 1,
		//TexCoordAttribLocation	= 2,
		//NormalAttribLocation		= 3,

		// Every data (from different types) are all put together into vertexData
		std::vector<uint8> 	vertexData;
		vertexData.reserve(
			getVectorDataSize(vertices)	+
			getVectorDataSize(colors)	+
			getVectorDataSize(texcoords)+
			getVectorDataSize(normals));

		appendVertexData(vertexData, vertices);
		appendVertexData(vertexData, colors);
		appendVertexData(vertexData, texcoords);
		appendVertexData(vertexData, normals);

		glBindBuffer(GL_ARRAY_BUFFER, _bufferIds[BUFVERTEX]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint8)*vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
		CHECK_GL_ERROR;

		glVertexAttribPointer(VertexAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, (uint8_t*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(ColorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, (uint8_t*)(0) + getVectorDataSize(vertices));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(TexCoordAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, (uint8_t*)(0) + getVectorDataSize(vertices) + getVectorDataSize(colors));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(NormalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, (uint8_t*)(0) + getVectorDataSize(vertices) + getVectorDataSize(colors) + getVectorDataSize(texcoords));
		glEnableVertexAttribArray(3);

		/// \todo TODO:
		/// We could ignore attrib that are empty (where mesh.colors().empty() == true, don't do anything with this).
		/// This could improve a bit performances.

		glBindVertexArray(0);
	}

	void	MeshBufferGL::free(void)
	{
		if (_bufferIds[0] && _bufferIds[1])
		{
			glDeleteBuffers(2, _bufferIds.data());
			_bufferIds.fill(0);
		}

		if (_vaoId)
		{
			glDeleteVertexArrays(1, &_vaoId);
			_vaoId = 0;
		}
	}

	void  MeshBufferGL::draw( void ) const
	{
		glBindVertexArray(_vaoId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferIds[BUFINDEX]);
		glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, (void*)0); 
		glBindVertexArray(0);
	}

	void MeshBufferGL::draw(unsigned int begin, unsigned int end) const {
		const unsigned int count = end-begin;
		glBindVertexArray(_vaoId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferIds[BUFINDEX]);
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * begin));
		glBindVertexArray(0);
	}

	void  MeshBufferGL::drawTessellated(void) const
	{
		glBindVertexArray(_vaoId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferIds[BUFINDEX]);
		glDrawElements(GL_PATCHES, _indexCount, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

	void  MeshBufferGL::draw_lines(void) const
	{
		glBindVertexArray(_vaoId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferIds[BUFINDEX]);
		glDrawElements(GL_LINES, _indexCount, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

	void  MeshBufferGL::draw_points() const
	{
		glBindVertexArray(_vaoId);
		glDrawArrays(GL_POINTS, 0, _vertexCount);
		glBindVertexArray(0);
	}

	void  MeshBufferGL::draw_points(unsigned int begin, unsigned int end) const
	{
		const unsigned int count = end - begin + 1;
		glBindVertexArray(_vaoId);
		glDrawArrays(GL_POINTS, begin, count);
		glBindVertexArray(0);
	}

	void MeshBufferGL::bind(void) const
	{
		glBindVertexArray(_vaoId);
	}

	void MeshBufferGL::bindIndices(void) const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferIds[BUFINDEX]);
	}

	void MeshBufferGL::unbind(void) const
	{
		glBindVertexArray(0);

	}

	

} // namespace sibr 
