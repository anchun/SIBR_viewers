
#ifndef __SIBR_ASSETS_IMAGELISTFILE_HPP__
# define __SIBR_ASSETS_IMAGELISTFILE_HPP__


# include "core/graphics/Image.hpp"
# include "core/assets/Config.hpp"
# include "core/assets/IFileLoader.hpp"
# include "core/assets/ActiveImageFile.hpp"

namespace sibr
{
	/**
	Represents a listing of input images.
	\ingroup sibr_assets
	*/
	class SIBR_ASSETS_EXPORT ImageListFile : public IFileLoader
	{
	public:
		struct Infos
		{
			std::string		filename;
			uint			width;
			uint			height;
		};

	public:
		bool load( const std::string& filename , bool verbose = true);
		
		const std::vector<Infos>&	infos( void ) const { return _infos; }
		const std::string&			basename( void ) const { return _basename; }

		template <class TImage>
		std::vector<TImage>			loadImages( void ) const;
		template <class TImage>
		std::vector<TImage>			loadImages( const ActiveImageFile& ) const;
		

	private:
		std::vector<Infos>		_infos;
		std::string				_basename;

	};

	///// DEFINITIONS /////


	template <class TImage>
	std::vector<TImage>			ImageListFile::loadImages( const ActiveImageFile& ac ) const {
		std::vector<TImage> out;

		SIBR_LOG << "[ImageListFile] loading images";
		out.resize(_infos.size());
		if (_infos.empty() == false)
		{
			#pragma omp parallel for
			for (int i = 0; i < _infos.size(); ++i)
				if( ac.active()[i] )
					out[i].load(_basename + "/" + _infos.at(i).filename, false);
		}
		else
			SIBR_WRG << "cannot load images (ImageListFile is empty. Did you use ImageListFile::load(...) before ?";

		std::cout << std::endl;
		return out;
	}

	template <class TImage>
	std::vector<TImage>			ImageListFile::loadImages( void ) const {
		std::vector<TImage> out;

		std::cerr << "[ImageListFile] loading images";
		out.resize(_infos.size());
		if (_infos.empty() == false)
		{
			#pragma omp parallel for
			for (int i = 0; i < _infos.size(); ++i)
				out[i].load(_basename + "/" + _infos.at(i).filename, false);
		}
		else
			SIBR_WRG << "cannot load images (ImageListFile is empty. Did you use ImageListFile::load(...) before ?";

		return out;
	}

} // namespace sibr

#endif // __SIBR_ASSETS_IMAGELISTFILE_HPP__
