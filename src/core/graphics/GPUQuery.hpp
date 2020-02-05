#pragma once
#include <core/system/Config.hpp>
#include <core/graphics/Config.hpp>

namespace sibr { 

	/**
	 * Provide a buffered wrapper around an OpenGL query object, avoiding manual synchronization.
	 * See section 4.2 of the OpenGL 4.6 specification for more details on the types of queries available
	 * (time elapsed, number of primitives, number of fragment writes...).
	 * 
	 * For example, to get the processing time of a mesh draw call, you can use the following.
	 * In initialisation:
	 *		GPUQuery query(GL_TIME_ELAPSED);
	 * In the rendering loop:
	 *		query.begin();
	 *		mesh.draw();
	 *		query.end();
	 * In your GUI loop:
	 *		const uint64 time = query.value();
	 *		//... display it.
	 *		
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT GPUQuery
	{
		SIBR_CLASS_PTR(GPUQuery);

	public:

		/** Create a query of a given type.
		\param type the OpenGL enum type
		\param count number of buffered internal queries (ideally >= 2).
		*/
		GPUQuery(GLenum type, size_t count = 2);

		/** Start measuring. */
		void begin();

		/** Stop measuring. */
		void end();

		/** Obtain the raw value (time in nanoseconds, number of primitives,...).
		\return the query value.
		*/
		uint64 value();

	private:
		
		std::vector<GLuint> _ids; ///< Internal queries IDs.
		const size_t _count; ///< Number of queries.
		GLenum _type; ///< Type of query.
		size_t _current = 0; ///< Current internla query used.
		bool _observing = false; ///< Are we currently measuring.
	};

} 
