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
		virtual void												replaceProxy(Mesh::Ptr newProxy) = 0;
		virtual void												replaceProxyPtr(Mesh::Ptr newProxy) = 0;
		virtual bool												hasProxy(void) const = 0;
		virtual const Mesh&											proxy(void) const = 0;
		virtual const Mesh::Ptr										proxyPtr(void) const = 0;

	protected:
		IProxyMesh() {};

	};

}