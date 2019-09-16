#ifndef _ULRV2_VIEW_H_
# define _ULRV2_VIEW_H_

# include "Config.hpp"
# include <core/system/Config.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/view/ViewBase.hpp>
# include "core/scene/BasicIBRScene.hpp"
# include <core/renderer/CopyRenderer.hpp>
# include <projects/ulr/renderer/ULRV2Renderer.hpp>
# include <core/renderer/PoissonRenderer.hpp>

namespace sibr { 
	class SIBR_EXP_ULR_EXPORT ULRV2View : public sibr::ViewBase
	{
		SIBR_CLASS_PTR(ULRV2View);

		enum class RenderMode { NORMAL = 0, ONLY_ONE_CAM = 1, LEAVE_ONE_OUT = 2 };

	public:

		ULRV2View( const sibr::BasicIBRScene::Ptr& ibrScene, uint render_w, uint render_h );
		~ULRV2View();

		virtual void onRenderIBR( sibr::IRenderTarget& dst, const sibr::Camera& eye );
		virtual void onUpdate(Input& input);
		virtual void onGUI() override;

		/** Return list of images chosen by ulr  */
		virtual std::vector<uint> chosen_cameras(const sibr::Camera& eye) ;
		virtual std::vector<uint> chosen_camerasNew(const sibr::Camera& eye);
		virtual std::vector<uint> chosen_cameras_angdist(const sibr::Camera& eye);

		/** Set the altMesh and use instead of scene proxy */
		void	altMesh(std::shared_ptr<sibr::Mesh> m)	{ _altMesh = m; }

		void	doOccl(bool val) { _ulr->doOccl(val); }

		std::shared_ptr<sibr::Mesh> 	altMesh()	{ return _altMesh; }

		void	setNumBlend(short int dist, short int angle);

		void	inputRTs(const std::vector<std::shared_ptr<RenderTargetRGBA32F> >& iRTs) { _inputRTs = iRTs;}

		void	setMasks( const std::vector<RenderTargetLum::Ptr>& masks );
		void	loadMasks(
			const sibr::BasicIBRScene::Ptr& ibrScene, int w, int h,
			const std::string& maskDir = "",
			const std::string& preFileName = "",
			const std::string& postFileName = ""
		);
	
		void		setRenderMode(RenderMode mode) { _renderMode = mode; }
		RenderMode	getRenderMode() const { return _renderMode; }

		void		setSingleViewId(int id) { _singleCamId = id; }
		int			getSingleViewId(void)  const { return _singleCamId; }

		void		noPoissonBlend(bool val)	{ _noPoissonBlend = val; }
		bool		noPoissonBlend()	        { return _noPoissonBlend; }

		void computeVisibilityMap(const sibr::ImageL32F & depthMap, sibr::ImageRGBA & out);

		const std::shared_ptr<sibr::BasicIBRScene> getScene() const { return _scene; }

	public:
		ULRV2Renderer::Ptr		_ulr;
		PoissonRenderer::Ptr	_poisson;

	protected:
		// renderers
		
		std::shared_ptr<sibr::BasicIBRScene> _scene;
		std::shared_ptr<sibr::Mesh>	_altMesh;
		// For the cases when using a different mesh than the scene
		int _numDistUlr, _numAnglUlr;

		// input RTs -- usually RGB but can be alpha or other
		std::vector<std::shared_ptr<RenderTargetRGBA32F> > _inputRTs;

		//! Runtime status of the poisson blend.
		bool								_noPoissonBlend = false;

		RenderTargetRGBA::Ptr				_blendRT;
		RenderTargetRGBA::Ptr				_poissonRT;

		RenderMode _renderMode;
		int _singleCamId;

		//TT to switch with alternate shader with tab
		bool testAltlULRShader;
	};

} /*namespace sibr*/ 

#endif // _ULRV2_VIEW_H_
