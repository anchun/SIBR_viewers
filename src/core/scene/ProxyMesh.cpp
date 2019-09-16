#include "ProxyMesh.hpp"


namespace sibr {

	void ProxyMesh::loadFromData(const ParseData::Ptr & data)
	{
		_proxy.reset(new Mesh());
		if (!_proxy->load(data->meshPath()) && !_proxy->load(data->meshPath() + ".ply") && !_proxy->load(data->meshPath() + ".obj")) {
			SIBR_ERR << "proxy model not found at " << data->meshPath() << std::endl;
		}
		if (!_proxy->hasNormals()) {
			SIBR_WRG << "generating normals for the proxy (no normals found in the proxy file)" << std::endl;
			_proxy->generateNormals();
		}
	}

	void ProxyMesh::replaceProxy(std::shared_ptr<Mesh> newProxy)
	{
		_proxy.reset(new Mesh());
		_proxy->vertices(newProxy->vertices());
		_proxy->normals(newProxy->normals());
		_proxy->colors(newProxy->colors());
		_proxy->triangles(newProxy->triangles());
		_proxy->texCoords(newProxy->texCoords());

		// Used by inputImageRT init() and debug rendering
		if (!_proxy->hasNormals())
		{
			SIBR_WRG << "generating normals for the proxy (no normals found in the proxy file)" << std::endl;
			_proxy->generateNormals();
		}

	}

	void ProxyMesh::replaceProxyPtr(Mesh::Ptr newProxy)
	{
		_proxy = newProxy;
	}


}

