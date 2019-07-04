#include "ImagesGrid.hpp"

namespace sibr
{
	void ImagesGrid::onUpdate(Input & input, const Viewport & vp)
	{
	}

	void ImagesGrid::onRender(const Viewport & viewport)
	{
		viewport.bind();

		viewport.clear(sibr::Vector3f(0.7f, 0.7f, 0.7f));

		if (current_level) {
			draw_utils.gridShader.begin();

			int num_imgs = (int)current_layer->imgs_texture_array.size();
			draw_utils.numImgsGL.set(num_imgs);
			draw_utils.gridGL.send();

			draw_utils.gridTopLeftGL.send(); // .set(viewRectangle.tl());
			draw_utils.gridBottomRightGL.send(); // (viewRectangle.br());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, current_level->handle());
			sibr::RenderUtility::renderScreenQuad();

			draw_utils.gridShader.end();

			for (int i = 0; i < num_imgs; ++i) {
				Vector2f imTl = uvFromMVpixel(MVpixel(i, { 0, 0 }));
				Vector2f imBR = uvFromMVpixel(MVpixel(i, { current_level->w(), current_level->h() }));
				draw_utils.rectangle({ 0, 0, 0 }, imTl, imBR, false, 1.0);
			}
		}

	}

	void ImagesGrid::onRender(IRenderTarget & dst)
	{
		dst.bind();

		Viewport vp(0.0f, 0.0f, (float)dst.w(), (float)dst.h());
		onRender(vp);

		dst.unbind();
	}

	void ImagesGrid::onGUI()
	{
	}

	void DrawUtilities::rectangle(const Vector3f & color, const Vector2f & tl, const Vector2f & br, bool fill, float alpha)
	{
		baseShader.begin();

		scalingGL.set(sibr::Vector2f(1.0f, 1.0f));
		translationGL.set(sibr::Vector2f(0, 0));
		colorGL.set(color);

		auto rectangleMesh = std::make_shared<Mesh>();

		rectangleMesh->vertices({
			{ tl.x(), tl.y() , 0 },
			{ tl.x(), br.y() , 0 },
			{ br.x(), br.y() , 0 },
			{ br.x(), tl.y() , 0 }
			});

		if (fill) {
			rectangleMesh->triangles({
				{ 0,1,2 },
				{ 0,2,3 }
				});

			alphaGL.set(alpha);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			rectangleMesh->render(false, false);
		}

		rectangleMesh->triangles({
			{ 0,0,1 },{ 1,1,2 },{ 2,2,3 },{ 3,3,0 }
			});

		alphaGL.set(1.0f);
		rectangleMesh->render(false, false, sibr::Mesh::LineRenderMode);

		baseShader.end();
	}

	void DrawUtilities::rectanglePixels(const Vector3f & color, const Vector2f & center, const Vector2f & diagonal, bool fill, float alpha, const Vector2f & winSize)
	{
		Vector2f centerUV = center.cwiseQuotient(winSize);
		Vector2f tl = centerUV - 0.5f*diagonal.cwiseQuotient(winSize);
		Vector2f br = centerUV + 0.5f*diagonal.cwiseQuotient(winSize);
		rectangle(color, tl, br, fill, alpha);
	}

	void DrawUtilities::circle(const Vector3f & color, const Vector2f & center, float radius, bool fill, float alpha, const Vector2f & scaling, int precision)
	{
		
		static sibr::Mesh::Vertices vertices;
		static Mesh::Triangles circleTriangles, circleFillTriangles;

		int n = precision;
		if (circleFillTriangles.size() != n) {
			n = precision;
			circleTriangles.resize(n);
			circleFillTriangles.resize(n);
			for (int i = 0; i < n; ++i) {
				int next = (i + 1) % n;
				circleTriangles[i] = sibr::Vector3u(i, i, next);
				circleFillTriangles[i] = sibr::Vector3u(i, next, n);
			}

			vertices.resize(n + 1);
		}

		double base_angle = 2.0*M_PI / (double)n;
		float rho = 0.5f*radius*(float)(1.0 + cos(0.5*base_angle));

		for (int i = 0; i < n; ++i) {
			double angle = i * base_angle;
			vertices[i] = sibr::Vector3f((float)cos(angle), (float)sin(angle), (float)0.0);
		}
		vertices[n] = sibr::Vector3f(0, 0, 0);

		auto circleMesh = std::make_shared<Mesh>();
		auto circleFilledMesh = std::make_shared<Mesh>();
		circleMesh->vertices(vertices);
		circleFilledMesh->vertices(vertices);
		circleMesh->triangles(circleTriangles);
		circleFilledMesh->triangles(circleFillTriangles);

		baseShader.begin();

		translationGL.set(sibr::Vector2f(0, 0));

		colorGL.set(color);
		scalingGL.set(scaling);
		translationGL.set(center);

		if (fill) {
			alphaGL.set(alpha);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			circleFilledMesh->render(false, false);
		}

		alphaGL.set(1.0f);
		circleMesh->render(false, false, sibr::Mesh::LineRenderMode);

		baseShader.end();
	}

	void DrawUtilities::circlePixels(const Vector3f & color, const Vector2f & center, float radius, bool fill, float alpha, const Vector2f & winSize, int precision)
	{
		Vector2f centerUV = center.cwiseQuotient(winSize);
		Vector2f scaling = radius * Vector2f(1, 1).cwiseQuotient(winSize);

		circle(color, centerUV, 1.0f, fill, alpha, scaling, precision);
	}

	void DrawUtilities::linePixels(const Vector3f & color, const Vector2f & ptA, const Vector2f & ptB, const Vector2f & winSize)
	{
		Vector2f uvA = ptA.cwiseQuotient(winSize);
		Vector2f uvB = ptB.cwiseQuotient(winSize);

		sibr::Mesh line;
		line.vertices({
			{ uvA.x(), uvA.y(), 0.0f },
			{ uvB.x(), uvB.y(), 0.0f }
			});
		line.triangles({
			sibr::Vector3u(0,0,1)
			});

		baseShader.begin();

		scalingGL.set(sibr::Vector2f(1.0f, 1.0f));
		translationGL.set(sibr::Vector2f(0, 0));
		colorGL.set(color);
		alphaGL.set(1.0f);

		line.render(false, false, sibr::Mesh::LineRenderMode);

		baseShader.end();
	}

	void DrawUtilities::initBaseShader()
	{
		const std::string translationScalingVertexShader =
			"#version 420															\n"
			"layout(location = 0) in vec3 in_vertex;								\n"
			"uniform vec2 translation;												\n"
			"uniform vec2 scaling;													\n"
			"void main(void) {														\n"
			"	gl_Position = vec4(scaling*in_vertex.xy+translation,0.0, 1.0);		\n"
			"}																		\n";

		const std::string colorAlphaFragmentShader =
			"#version 420														\n"
			"uniform vec3 color;												\n"
			"uniform float alpha;												\n"
			"out vec4 out_color;												\n"
			"void main(void) {													\n"
			"		out_color = vec4(color,alpha);								\n"
			"}																	\n";

		baseShader.init("InterfaceUtilitiesBaseShader", translationScalingVertexShader, colorAlphaFragmentShader);
		colorGL.init(baseShader, "color");
		alphaGL.init(baseShader, "alpha");
		scalingGL.init(baseShader, "scaling");
		translationGL.init(baseShader, "translation");
	}

	void DrawUtilities::initGridShader()
	{
		const std::string gridVertexShader =
			"#version 420										\n"
			"layout(location = 0) in vec3 in_vertex;			\n"
			"out vec2 uv_coord;									\n"
			"uniform vec2 zoomTL;								\n"
			"uniform vec2 zoomBR;								\n"
			"void main(void) {									\n"
			"	uv_coord = 0.5*in_vertex.xy + vec2(0.5);		\n"
			"	uv_coord = zoomTL + (zoomBR-zoomTL)*uv_coord;	\n"
			"	uv_coord.y = 1.0 - uv_coord.y;					\n"
			"	gl_Position = vec4(in_vertex.xy,0.0, 1.0);		\n"
			"}													\n";


		const std::string gridFragmentShader =
			"#version 420														\n"
			"layout(binding = 0) uniform sampler2DArray texArray;				\n"
			"uniform int numImgs;												\n"
			"uniform vec2 grid;													\n"
			"in vec2 uv_coord;													\n"
			"out vec4 out_color;												\n"
			"void main(void) {													\n"
			"	vec2 uvs = uv_coord;											\n"
			"	uvs =  grid*uvs;												\n"
			"  if( uvs.x < 0 || uvs.y < 0 ) { discard; } 						\n"
			"   vec2 fracs = fract(uvs); 										\n"
			"   vec2 mods = uvs - fracs; 										\n"
			"   int n = int(mods.x + grid.x*mods.y); 							\n"
			" if ( n< 0 || n > numImgs || mods.x >= grid.x || mods.y >= (float(numImgs)/grid.x) + 1) { discard; } else { \n"
			"	out_color = texture(texArray,vec3(fracs.x,fracs.y,n));	}		\n"
			"	//out_color = vec4(n/64.0,0.0,0.0,1.0); }						\n"
			"	//out_color = vec4(uv_coord.x,uv_coord.y,0.0,1.0);	}			\n"
			"}																	\n";


		gridShader.init("InterfaceUtilitiesMultiViewShader", gridVertexShader, gridFragmentShader);
		gridTopLeftGL.init(gridShader, "zoomTL");
		gridBottomRightGL.init(gridShader, "zoomBR");
		numImgsGL.init(gridShader, "numImgs");
		gridGL.init(gridShader, "grid");
	}

	MVpixel GridMapping::pixFromScreenPos(const Vector2i & pos, const Vector2f & size)
	{
		Vector2f uvScreen = pos.cast<float>() + 0.5f*Vector2f(1, 1).cwiseQuotient(size);

		Vector2f posF = viewRectangle.tl() + (viewRectangle.br() - viewRectangle.tl()).cwiseProduct(uvScreen);
		posF.y() = 1.0f - posF.y();

		posF = posF.cwiseProduct(grid.cast<float>());

		//std::cout << posF.transpose() << " " << numImgs << std::endl;

		if (posF.x() < 0 || posF.y() < 0 || posF.x() >= grid.x() /* || posF.y() >= grid.y()  */) {
			return MVpixel();
		}

		int x = (int)std::floor(posF.x());
		int y = (int)std::floor(posF.y());

		int n = x + grid.x() * y;
		if (n >= num_imgs) {
			return MVpixel();
		}

		Vector2f frac = posF - Vector2f(x, y);
		int j = (int)std::floor(frac.x()*imSizePixels.x());
		int i = (int)std::floor(frac.y()*imSizePixels.y());
		return MVpixel(n, Vector2i(j, i));
	}

	Vector2f GridMapping::uvFromMVpixel(const MVpixel & pix)
	{
		Vector2f pos = ((pix.pos.cast<float>() + Vector2f(0.5, 0.5)).cwiseQuotient(imSizePixels) +
			Vector2f(pix.im % grid.x(), pix.im / grid.x())).cwiseQuotient(grid.cast<float>());
		pos.y() = 1.0f - pos.y();
		return (pos - viewRectangle.tl()).cwiseQuotient(viewRectangle.br() - viewRectangle.tl());
	}

	void GridMapping::updateZoomBox(const Input & input, const Vector2f & size)
	{
		if (input.key().isPressed(sibr::Key::Q)) {
			viewRectangle.center = Vector2f(0.5, 0.5);
			viewRectangle.diagonal = Vector2f(0.5, 0.5);
		}

		if (input.mouseButton().isPressed(Mouse::Code::Right) && !zoomSelection.isActive) {
			zoomSelection.isActive = true;
			zoomSelection.first = input.mousePosition();
			zoomSelection.first.y() = (int)size.y() - zoomSelection.first.y() - 1;
		}
		if (input.mouseButton().isActivated(Mouse::Code::Right) && zoomSelection.isActive) {
			zoomSelection.second = input.mousePosition();
			zoomSelection.second.y() = (int)size.y() - zoomSelection.second.y() - 1;
		}
		if (input.mouseButton().isReleased(Mouse::Code::Right) && zoomSelection.isActive) {
			Vector2f currentTL = (zoomSelection.first.cwiseMin(zoomSelection.second)).cast<float>();
			Vector2f currentBR = (zoomSelection.first.cwiseMax(zoomSelection.second)).cast<float>();

			if (((currentBR - currentTL).array() > Vector2f(10, 10).array()).all()) {
				Vector2f tlPix = viewRectangle.tl().cwiseProduct(size) + (viewRectangle.br() - viewRectangle.tl()).cwiseProduct(currentTL);
				Vector2f brPix = viewRectangle.tl().cwiseProduct(size) + (viewRectangle.br() - viewRectangle.tl()).cwiseProduct(currentBR);

				Vector2f center = 0.5f*(brPix + tlPix);
				Vector2f diag = 0.5f*(brPix - tlPix);

				float new_ratio = diag.x() / diag.y();
				float target_ratio = imRatio;
				if (new_ratio > target_ratio) {
					diag.y() = diag.x() / target_ratio;
				} else {
					diag.x() = diag.y() * target_ratio;
				}

				viewRectangle.center = center.cwiseQuotient(size);
				viewRectangle.diagonal = diag.cwiseQuotient(size);
				zoomSelection.isActive = false;
			}
		}
	}

	void GridMapping::updateZoomScroll(const Input & input)
	{
		double scroll = input.mouseScroll();
		if (scroll) {
			float ratio = (scroll > 0 ? 0.75f : 1.33f);
			if (input.key().isActivated(sibr::Key::LeftControl)) {
				ratio *= ratio;
			}
			viewRectangle.diagonal *= ratio;
		}
	}

	void GridMapping::updateCenter(const Input & input, const Vector2f & size)
	{
	}

	void GridMapping::updateDrag(const Input & input, const Vector2f & size)
	{
		if (input.mouseButton().isPressed(sibr::Mouse::Left)) {
			drag.isActive = true;
			drag.position = input.mousePosition();
			drag.center = viewRectangle.center;
		} else if (drag.isActive && input.mouseButton().isReleased(sibr::Mouse::Left)) {
			drag.isActive = false;
		}
		if (drag.isActive && input.mouseButton().isActivated(sibr::Mouse::Left)) {
			sibr::Vector2f translation = (input.mousePosition() - drag.position).cast<float>().cwiseQuotient(size).cwiseProduct(viewRectangle.br() - viewRectangle.tl());
			translation.y() = -translation.y();
			viewRectangle.center = drag.center - translation;
		}
	}

}
	