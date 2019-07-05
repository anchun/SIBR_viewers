#ifndef _ULR_VIEW_H_
# define _ULR_VIEW_H_

# include "Config.hpp"
# include <core/system/Config.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/view/BasicIBRScene.hpp>
# include <core/renderer/CopyRenderer.hpp>
# include <projects/ulr/renderer/ULRRenderer.hpp>
# include <core/view/ViewBase.hpp>

namespace sibr { 
	class SIBR_EXP_ULR_EXPORT ULRView : public sibr::ViewBase
	{
		SIBR_CLASS_PTR( ULRView );

	public:
		ULRView( const sibr::BasicIBRScene::Ptr& ibrScene, uint render_w, uint render_h );
		~ULRView();

		virtual void onRenderIBR( sibr::IRenderTarget& dst, const sibr::Camera& eye );

		/** Return list of images chosen by ulr  */
		virtual std::vector<uint> chosen_cameras(const sibr::Camera& eye) ;

		/** Set the altMesh and use instead of scene proxy */
		void	altMesh(std::shared_ptr<sibr::Mesh> m)	{ _altMesh = m; }

		void	doOccl(bool val) { _ulr->doOccl(val); }

		std::shared_ptr<sibr::Mesh> 	altMesh()	{ return _altMesh; }

		void	setNumBlend(short int dist, short int angle) { _numDistUlr = dist, _numAnglUlr = angle; }

		void	inputRTs(const std::vector<std::shared_ptr<RenderTargetRGBA32F> >& iRTs) { _inputRTs = iRTs;}

		void	setMasks( const std::vector<RenderTargetLum::Ptr>& masks);

	protected:
		// renderers
		ULRRenderer::Ptr		_ulr;

		std::shared_ptr<sibr::BasicIBRScene> _scene;
		std::shared_ptr<sibr::Mesh>	_altMesh;
		// For the cases when using a different mesh than the scene
		short int _numDistUlr, _numAnglUlr;

		// input RTs -- usually RGB but can be alpha or other
		std::vector<std::shared_ptr<RenderTargetRGBA32F> > _inputRTs;

	};

} /*namespace sibr*/ 

#endif // _ULR_VIEW_H_
