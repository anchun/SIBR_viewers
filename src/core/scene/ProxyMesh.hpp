#pragma once

#include "core/scene/IProxyMesh.hpp"

namespace sibr {
	/**
	\ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT ProxyMesh : public IProxyMesh {
		SIBR_DISALLOW_COPY(ProxyMesh);
	public:
		typedef std::shared_ptr<ProxyMesh>					Ptr;

		ProxyMesh::ProxyMesh() {};
		~ProxyMesh() {};
		void												loadFromData(const IParseData::Ptr & data) override;
		void												replaceProxy(std::shared_ptr<sibr::Mesh> newProxy) override;
		void												replaceProxyPtr(Mesh::Ptr newProxy) override;

	};

}