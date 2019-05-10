#ifndef __SIBR_EXP_ULRV2_RENDERER_HPP__
# define __SIBR_EXP_ULRV2_RENDERER_HPP__

# include "Config.hpp"
# include <core/system/Config.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Shader.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/renderer/RenderMaskHolder.hpp>

namespace sibr { 
	class SIBR_EXP_ULR_EXPORT ULRV2Renderer : public RenderMaskHolder
	{
		SIBR_CLASS_PTR(ULRV2Renderer);

		//ULRV2Renderer(const sibr::IBRScene::Ptr& ibrScene, const unsigned int maxCams = 0, const std::string & fShader = "ulr_v2", const std::string & vShader = "ulr_v2", const bool facecull = true);
		ULRV2Renderer(const std::vector<sibr::InputCamera> & cameras, const uint w, const uint h, const unsigned int maxCams = 0, const std::string & fShader = "ulr_v2", const std::string & vShader = "ulr_v2", const bool facecull = true);

		//requires _nCams to be set up
		void setupULRshader(const std::string & fShader = "ulr_v2", const std::string & vShader = "ulr_v2");

		void process(const std::vector<uint>& imgs_ulr, const sibr::Camera& eye,
			const sibr::BasicIBRScene::Ptr& scene,
			std::shared_ptr<sibr::Mesh>& altMesh,
			const std::vector<std::shared_ptr<RenderTargetRGBA32F> >& inputRTs,
			IRenderTarget& dst);

		void doOccl(bool val) { _doOccl = val; }
		float & epsilonOcclusion() { return _epsilonOcclusion; }
		void setAreMasksBinary(bool val) { _areMasksBinary = val; }
		void setDoInvertMasks(bool val) { _doInvertMasks = val; }
		void setDiscardBlackPixels(bool val) { _discardBlackPixels = val; }
		void setCulling(bool val) { _shouldCull = val; }
		
		Texture2DArrayLum32F * & getSoftVisibilityMaps(void) { return soft_visibility_maps; }
		sibr::GLuniform<float> & getSoftVisibilityThreshold() { return _soft_visibility_threshold; }

		sibr::GLShader * getProgram() { return &_ulrShader; }
		size_t getNumCams() { return _numCams; }

	public:
		sibr::RenderTargetRGBA32F::Ptr _depthRT;

	private:
		
		sibr::GLShader _ulrShader;
		sibr::GLShader _depthShader;

		std::vector<sibr::GLParameter>	_icamProj;
		std::vector<sibr::GLParameter>	_icamPos;
		std::vector<sibr::GLParameter>	_icamDir;
		std::vector<sibr::GLParameter>	_inputRGB;
		std::vector<sibr::GLParameter>	_masks;
		std::vector<sibr::GLuniform<int> >	_selected_cams;

		Texture2DArrayLum32F * soft_visibility_maps;
		sibr::GLuniform<float> _soft_visibility_threshold;
		sibr::GLuniform<bool> _use_soft_visibility;

		sibr::GLParameter _occTest;
		sibr::GLParameter _areMasksBinaryGL;
		sibr::GLParameter _doInvertMasksGL;
		sibr::GLParameter _discardBlackPixelsGL;
		sibr::GLParameter _doMask;
		sibr::GLParameter _ncamPos;
		sibr::GLParameter _camCount;
		sibr::GLParameter _proj;
		sibr::GLuniform<float> _epsilonOcclusion;

		bool	_doOccl;
		bool	_areMasksBinary;
		bool	_doInvertMasks;
		bool	_discardBlackPixels;
		bool _shouldCull;
		size_t _numCams;


	
   };

} /*namespace sibr*/ 
#endif /* define */

