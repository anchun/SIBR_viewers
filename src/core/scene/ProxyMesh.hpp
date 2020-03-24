#pragma once

#include "core/scene/Config.hpp"
#include "core/scene/ParseData.hpp"
#include "core/graphics/Mesh.hpp"

namespace sibr {
	/**
	\ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT ProxyMesh {
		SIBR_DISALLOW_COPY(ProxyMesh);
	public:
		typedef std::shared_ptr<ProxyMesh>					Ptr;

		ProxyMesh::ProxyMesh() {};
		void												loadFromData(const ParseData::Ptr & data);
		void												replaceProxy(std::shared_ptr<sibr::Mesh> newProxy);
		void												replaceProxyPtr(Mesh::Ptr newProxy);
		bool												hasProxy(void) const;
		const Mesh&											proxy(void) const;
		const Mesh::Ptr										proxyPtr(void) const;
		~ProxyMesh() {};

	protected:

		Mesh::Ptr											_proxy;

	};

	inline bool												sibr::ProxyMesh::hasProxy(void) const
	{
		return _proxy && !_proxy->vertices().empty();
	}

	inline const Mesh &										ProxyMesh::proxy(void) const
	{
		return *_proxy;
	}

	inline const Mesh::Ptr										ProxyMesh::proxyPtr(void) const
	{
		return _proxy;
	}

}