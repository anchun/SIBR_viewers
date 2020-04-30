#pragma once

#include "core/scene/Config.hpp"
#include "core/scene/IParseData.hpp"
#include "core/graphics/Mesh.hpp"

namespace sibr {
	/**
	\ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT IProxyMesh {
		SIBR_DISALLOW_COPY(IProxyMesh);
	public:
		typedef std::shared_ptr<IProxyMesh>					Ptr;

		virtual void												loadFromData(const IParseData::Ptr & data) = 0;
		virtual void												replaceProxy(std::shared_ptr<sibr::Mesh> newProxy) = 0;
		virtual void												replaceProxyPtr(Mesh::Ptr newProxy) = 0;
		virtual bool												hasProxy(void) const;
		virtual const Mesh&											proxy(void) const;
		virtual const Mesh::Ptr										proxyPtr(void) const;

	protected:
		IProxyMesh() {};

		Mesh::Ptr											_proxy;

	};

	inline bool												sibr::IProxyMesh::hasProxy(void) const
	{
		return _proxy && !_proxy->vertices().empty();
	}

	inline const Mesh &										IProxyMesh::proxy(void) const
	{
		return *_proxy;
	}

	inline const Mesh::Ptr										IProxyMesh::proxyPtr(void) const
	{
		return _proxy;
	}

}