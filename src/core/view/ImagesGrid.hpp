#pragma once
# include "Config.hpp"
#include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>
#include <core/view/ViewBase.hpp>
#include <list>

namespace sibr
{

	class SIBR_VIEW_EXPORT DrawUtilities
	{
	public:
		GLShader baseShader;

		GLuniform<Vector3f> colorGL;
		GLuniform <float> alphaGL;
		GLuniform <Vector2f> scalingGL;
		GLuniform <Vector2f> translationGL;

		GLShader gridShader;

		GLuniform <int> numImgsGL;
		GLuniform <Vector2f> gridGL;
		GLuniform <Vector2f> gridTopLeftGL;
		GLuniform <Vector2f> gridBottomRightGL;

		void rectangle(const Vector3f & color, const Vector2f & tl, const Vector2f & br, bool fill, float alpha);
		void rectanglePixels(const Vector3f & color, const Vector2f & center, const Vector2f & diagonal, bool fill, float alpha, const Vector2f & winSize);
		void circle(const Vector3f & color, const Vector2f & center, float radius, bool fill, float alpha, const Vector2f & scaling = Vector3f(1, 1), int precision = 50);
		void circlePixels(const Vector3f & color, const Vector2f & center, float radius, bool fill, float alpha, const Vector2f & winSize, int precision = 50);
		void linePixels(const Vector3f & color, const Vector2f & ptA, const Vector2f & ptB, const Vector2f & winSize);

	private:

		void initBaseShader();
		void initGridShader();

	};

	struct QuadData
	{
		QuadData() : center({ 0.5f, 0.5f }), diagonal({ 0.5f, 0.5f }) {}
		Vector2f center;
		Vector2f diagonal;

		Vector2f br() const { return center + diagonal; }
		Vector2f tl() const { return center - diagonal; }
	};

	struct QuadSelectionData
	{
		Vector2i first;
		Vector2i second;
		bool isActive = false;
	};

	struct DragClickData
	{
		Vector2f center;
		Vector2i position;
		bool isActive = false;
	};

	struct MVpixel {
		MVpixel() : isDefined(false) {}
		MVpixel(int i, const Vector2i & px) : im(i), pos(px), isDefined(true) {}
			
		Vector2i pos;
		int im;
		bool isDefined = false;
	};


	struct GridMapping {

		MVpixel pixFromScreenPos(const Vector2i & pos, const Vector2f & size);
		Vector2f uvFromMVpixel(const MVpixel & pix);

		void updateZoomBox(const Input & input, const Vector2f & size);
		void updateZoomScroll(const Input & input);
		void updateCenter(const Input & input, const Vector2f & size);
		void updateDrag(const Input & input, const Vector2f & size);

		QuadData viewRectangle;
		QuadSelectionData zoomSelection;
		DragClickData drag;

		Vector2i grid;
		Vector2f imSizePixels;
		float imRatio;
		int num_imgs;
	};

	struct ImageGridLayer {	
		std::vector<ITexture2DArray::Ptr> imgs_texture_array;
		std::string name;
	};

	class SIBR_VIEW_EXPORT ImagesGrid : public ViewBase, GridMapping 
	{

	public:
		//ViewBase interface
		virtual void	onUpdate(Input& input, const Viewport & vp) override;
		virtual void	onRender(const Viewport & viewport) override;
		virtual void	onRender(IRenderTarget & dst);
		virtual void	onGUI() override;

	protected:
		std::list<ImageGridLayer> images_layers;
		std::list<ImageGridLayer>::iterator current_layer;
		ITexture2DArray::Ptr current_level;
		//uint current_level = 0;

		DrawUtilities draw_utils;

	public:
		template<typename T, uint N>
		void addImageLayer(
			const std::string & layer_name,
			const std::vector<Image<T, N> > & images,
			uint num_levels = 1,
			uint flags = 0
		) {
			std::vector<cv::Mat> images_cv(images.size());
			for (size_t im = 0; im < images.size(); ++im) {
				images_cv[im] = images[im].toOpenCV();
			}
			addImageLayer<T, N>(images_cv);
		}

		template<typename T, uint N>
		void addImageLayer(
			const std::string & layer_name,
			const std::vector<ImagePtr<T,N>> & images,
			uint num_levels = 1,
			uint flags = 0
		) {
			std::vector<cv::Mat> images_cv(images.size());
			for (size_t im = 0; im < images.size(); ++im) {
				images_cv[im] = images[im]->toOpenCV();
			}
			addImageLayer<T, N>(images_cv);
		}

		template<typename T, uint N>
		void addImageLayer(
			const std::string & layer_name,
			const std::vector<cv::Mat> & images,
			uint num_levels = 1,
			uint flags = 0
		) {
			if (images_layers.count(layer_name)) {
				return;
			}

			std::vector<std::vector<cv::Mat>> pyrs(images.size());
			for (int im = 0; im < (int)images.size(); ++im) {
				cv::buildPyramid(images[im], pyrs[im], num_levels);
			}

			ImageGridLayer layer;
			layer.name = layer_name;
			layer.imgs_texture_array.resize(num_levels);
			for (uint l = 0; l < num_levels; ++l) {
				std::vector<cv::Mat> current_level;
				for (const auto & pyr : pyrs) {
					current_level.push_back(pyr[l]);
				}
				auto tex = std::make_shared<Texture2DArray<T, N>>(current_level, flags);
				layer.imgs_texture_array[l] = std::static_pointer_cast<ITexture2DArray>(tex);
			}

			images_layers.push_back(layer);

			if (images_layers.size() == 1) {
				current_layer = images_layers.begin();
			}
		}
	};
}
