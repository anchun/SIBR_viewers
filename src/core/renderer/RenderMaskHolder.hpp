
#ifndef __SIBR_EXP_RENDEDER_RENDERMASKHOLDER_HPP___
# define __SIBR_EXP_RENDEDER_RENDERMASKHOLDER_HPP___

# include "Config.hpp"
# include <core/graphics/RenderTarget.hpp>
# include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Image.hpp>
# include <core/scene/BasicIBRScene.hpp>

namespace sibr { 

	/**
	\note Might want to use textures instead of RTs here.
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT RenderMaskHolder
	{
		typedef	RenderTargetLum::Ptr	MaskPtr;
	public:
		void							setMasks( const std::vector<MaskPtr>& masks );
		const std::vector<MaskPtr>&		getMasks( void ) const;
		bool							useMasks( void ) const;
		void 							loadMasks(
											const sibr::BasicIBRScene::Ptr& ibrScene, 
											const std::string& maskDir, const std::string& preFileName, 
											const std::string& postFileName, int w, int h);

	    void 							uploadMaskGPU(sibr::ImageL8& img, int i, std::vector<RenderTargetLum::Ptr> & masks, bool invert) ;

	private:
		std::vector<MaskPtr>	_masks;
	};

	/**
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT RenderMaskHolderArray
	{
		using MaskArray = sibr::Texture2DArrayLum;
		using MaskArrayPtr = MaskArray::Ptr;

	public:
		void							setMasks(const MaskArrayPtr& masks);
		const MaskArrayPtr &				getMasks(void) const;
		void 							loadMasks(
			const sibr::BasicIBRScene::Ptr& ibrScene,
			const std::string& maskDir = "", const std::string& preFileName = "masks" ,
			const std::string& postFileName = "", int w = -1, int h = -1
		);

	protected:
		MaskArrayPtr _masks;
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDEDER_RENDERMASKHOLDER_HPP___
