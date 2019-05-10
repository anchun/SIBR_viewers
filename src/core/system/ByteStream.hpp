
#ifndef __SIBR_COMMON_BYTESTREAM_HPP__
# define __SIBR_COMMON_BYTESTREAM_HPP__

# include <vector>
# include <iomanip>
# include "core/system/Config.hpp"


namespace sibr
{
	/// Be sure to use STL objects from client's dll version by exporting this declaration (see warning C4251)
	//template class SIBR_EXPORT std::vector<uint8>;

	///
	/// Used to manipulate stream of bytes.
	/// Notes:
	///  - You can activate a exception launcher for warning when something
	/// goes wrong (actually not implemented yet).
	///  - This ByteStream stores integer using the network byte order (which
	/// is big endian).
	/// \ingroup sibr_system
	///
	class SIBR_SYSTEM_EXPORT ByteStream
	{
	public:
		enum Endianness
		{
			BigEndian = 0, // always default
			LittleEndian
		};

	public:
		typedef std::vector<uint8>	bytes;	///< type used for storing bytes

		ByteStream( void ) : _readPos(0), _valid(true) /*,_endianness(BigEndian)*/ { }

		/// Load all bytes from a file using the given \param filename
		bool load( const std::string& filename );
		/// Save all bytes to a file using the given \param filename
		void	saveToFile( const std::string& filename );

		/// Append data to the current buffer
		void push(const void* data, uint size);

		operator bool( void ) const { return _valid; }

		/// Write data to the byte stream
		ByteStream& operator <<( bool b );
		ByteStream& operator <<( int8 i );
		ByteStream& operator <<( int16 i );
		ByteStream& operator <<( int32 i );
		ByteStream& operator <<( int64 i );
		ByteStream& operator <<( uint8 i );
		ByteStream& operator <<( uint16 i );
		ByteStream& operator <<( uint32 i );
		ByteStream& operator <<( uint64 i );
		ByteStream& operator <<( const std::string& str );
		/// Write a floating-point to the byte stream
		/// Note it uses endianess as integner (not specified in IEEE 754)
		ByteStream& operator <<( float f );
		ByteStream& operator <<(double d);

		/// Read data from the byte stream
		inline ByteStream& operator >>( bool & b );
		inline ByteStream& operator >>( int8& i );
		inline ByteStream& operator >>( int16& i );
		inline ByteStream& operator >>( int32& i );
		inline ByteStream& operator >>( int64& i );
		inline ByteStream& operator >>( uint8& i );
		inline ByteStream& operator >>( uint16& i );
		inline ByteStream& operator >>( uint32& i );
		inline ByteStream& operator >>( uint64 & i );
		inline ByteStream& operator >>( std::string& str );
		/// Read a floating-point number from the byte stream
		/// Note it uses endianess as integner (not specified in IEEE 754)
		inline ByteStream& operator >>( float& f ); ///< (use endianess as an integer)
		inline ByteStream& operator >>(double& d);

		/// Return the number of bytes that have not been read yet.
		inline size_t	readableSize( void ) const;
		/// Return the total number of bytes in the buffer used by the stream
		inline size_t	bufferSize( void ) const;
		/// Return a pointer to the buffer
		inline const uint8*	buffer( void ) const { return &_buffer[0]; }

		// Don't want to include network-related libs (and all their stuffs), so I will
		// use my own implementation of htonl/htons, ntohl/ntohs.

		/// Convert an uint32 from host to network byte order (which is big endian)
		static uint64	htonll(uint64 n);
		/// Convert an uint32 from host to network byte order (which is big endian)
		static uint32	htonl( uint32 n );
		/// Convert an uint16 from host to network byte order (which is big endian)
		static uint16	htons( uint16 n );
		/// Convert an uint32 from network to host byte order
		inline static uint64	ntohll(uint64 n);
		/// Convert an uint32 from network to host byte order
		inline static uint32	ntohl( uint32 n );
		/// Convert an uint16 from network to host byte order
		inline static uint16	ntohs( uint16 n );
		/// Return TRUE if the current system runs using Big Endian
		static bool systemIsBigEndian( void );

		// (Not used for now: future features to change how are stored float)
		// void		setEndianness( Endianness e );
		// Endianness	getEndianness( void );

		/// Dump the buffer contents. (used for debugging purposes)
		void			memoryDump( void ) const;
	private:
		/// Test if we can read \param n bytes in the buffer.
		/// Return false if fails (and set valid flag to false).
		inline bool		testSize( uint n );

		bytes		_buffer;	///< the whole stream

		/// Current position in the buffer when reading.
		///
		/// It's more efficient to more a small int cursor when reading
		/// than remove bytes at the beginning of the buffer (involving
		/// change in buffer size, allocation, move of elements, etc).
		uint32		_readPos;
		bool		_valid;		///< tells if no error occured when reading
		// Endianness	_endianness;
		};

		///// INLINE FUNCTIONS /////
		size_t	ByteStream::readableSize( void ) const {
			return bufferSize() - _readPos;
		}
		size_t	ByteStream::bufferSize( void ) const {
			return _buffer.size();
		}

		uint64	ByteStream::ntohll(uint64 n) {
			return htonll(n);
		}
		uint32	ByteStream::ntohl( uint32 n ) {
			return htonl(n);
		}
		uint16	ByteStream::ntohs( uint16 n ) {
			return htons(n);
		}
		bool		ByteStream::testSize( uint n ) {
			return (_valid = (_valid && (readableSize() >= n)));
		}

		ByteStream& ByteStream::operator >>( bool& b ) {
			uint8 i;
			ByteStream::operator >>(i);
			b = (i != 0);
			return *this;
		}
		ByteStream& ByteStream::operator >>( int8& i ) {
			if (testSize(sizeof(i)))
			{
				i = *reinterpret_cast<int8*>(&_buffer[_readPos]);
				_readPos += sizeof(i);
			}
			return *this;
		}
		ByteStream& ByteStream::operator >>( int16& i ) {
			if (testSize(sizeof(i)))
			{
				i = ntohs(*reinterpret_cast<int16*>(&_buffer[_readPos]));
				_readPos += sizeof(i);
			}
			return *this;
		}

		ByteStream& ByteStream::operator >>( int32& i )  {
			if (testSize(sizeof(i)))
			{
				i = ntohl(*reinterpret_cast<int32*>(&_buffer[_readPos]));
				_readPos += sizeof(i);
			}
			return *this;
		}
		ByteStream& ByteStream::operator >>(int64& i) {
			if (testSize(sizeof(i)))
			{
				i = ntohll(*reinterpret_cast<int64*>(&_buffer[_readPos]));
				_readPos += sizeof(i);
			}
			return *this;
		}

		ByteStream& ByteStream::operator >>( uint8& i )  {
			if (testSize(sizeof(i)))
			{
				i = *reinterpret_cast<uint8*>(&_buffer[_readPos]);
				_readPos += sizeof(i);
			}
			return *this;
		}
		ByteStream& ByteStream::operator >>( uint16& i ) {
			if (testSize(sizeof(i)))
			{
				i = ntohs(*reinterpret_cast<uint16*>(&_buffer[_readPos]));
				_readPos += sizeof(i);
			}
			return *this;
		}
		ByteStream& ByteStream::operator >>( uint32& i ) {
			if (testSize(sizeof(i)))
			{
				i = ntohl(*reinterpret_cast<uint32*>(&_buffer[_readPos]));
				_readPos += sizeof(i);
			}
			return *this;
		}
		ByteStream& ByteStream::operator >>(uint64& i) {
			if (testSize(sizeof(i)))
			{
				i = ntohll(*reinterpret_cast<uint64*>(&_buffer[_readPos]));
				_readPos += sizeof(i);
			}
			return *this;
		}
		ByteStream& ByteStream::operator >>( std::string& str ) {
			uint32 size;
			operator >> (size);
			str.resize(size);

			if (testSize(sizeof(char)*size))
			{
				str.insert(str.begin(), 
					reinterpret_cast<char*>(&_buffer[_readPos]), 
					reinterpret_cast<char*>(&_buffer[_readPos + size]));
				_readPos += sizeof(char)*size;
			}
			return *this;
		}
		ByteStream& ByteStream::operator >>( float& f ) {
			int32 p;
			ByteStream::operator>> (p);
			char* ptr = (char*)&p;
			float *fptr = (float*)ptr;
			if (_valid)
				f = *fptr;
			return *this;
		}
		ByteStream& ByteStream::operator >>( double& d ) {
			int64 p;
			ByteStream::operator>> (p);
			char* ptr = (char*)&p;
			double *fptr = (double*)ptr;
			if (_valid)
				d = *fptr;
			return *this;
		}

		//// Function used to test this class (might be still useful to test future improvement)
		/*/
		static void unitTestByteStream( void )
		{
			SIBR_LOG << "[work in progress] - testing ByteStream" << std::endl;

			struct SomeData {
				bool b;
				int8 s8;
				uint8 u8;
				int16 s16;
				uint16 u16;
				int32 s32;
				uint32 u32;
				float f;

				void dump( void ) {
					SIBR_DEBUG(b);
					SIBR_DEBUG(s8);
					SIBR_DEBUG(u8);
					SIBR_DEBUG(s16);
					SIBR_DEBUG(u16);
					SIBR_DEBUG(s32);
					SIBR_DEBUG(u32);
					SIBR_DEBUG(f);
				}
			};

			SomeData di = {true, -30, 120, -12000, 23000, -1234567, 2345678, 2.8f};
			std::cout << "Dumping DataIN:" << std::endl;
			di.dump();

			sibr::ByteStream bytes;

			bytes << di.b << di.s8 << di.u8 << di.s16 << di.u16 << di.s32 << di.u32 << di.f;
			SomeData dout = {false, 0, 0, 0, 0, 0, 0, 0.f};
			std::cout << "Dumping DataOUT:" << std::endl;
			dout.dump();

			bytes.memoryDump();


			bytes >> dout.b >> dout.s8 >> dout.u8 >> dout.s16 >> dout.u16 >> dout.s32 >> dout.u32 >> dout.f;
			std::cout << "Dumping DataOUT:" << std::endl;
			dout.dump();
		}
		//*/

	} // namespace sibr

#endif // __SIBR_COMMON_BYTESTREAM_HPP__
