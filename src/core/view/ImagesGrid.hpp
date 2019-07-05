#pragma once
# include "Config.hpp"
#include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>
#include <core/view/ViewBase.hpp>
#include <list>

namespace sibr
{

	SIBR_VIEW_EXPORT Vector2f toGLuv(const Vector2f & uv);

	class SIBR_VIEW_EXPORT DrawUtilities
	{
	public:

		DrawUtilities();

		GLShader baseShader;

		GLuniform<Vector3f> colorGL;
		GLuniform <float> alphaGL;
		GLuniform <Vector2f> scalingGL;
		GLuniform <Vector2f> translationGL;

		GLShader gridShader;

		
		GLuniform <Vector2f> gridGL;
		GLuniform <Vector2f> gridTopLeftGL;
		GLuniform <Vector2f> gridBottomRightGL;
		GLuniform <float> lod;
		GLuniform <int> numImgsGL; 
		GLuniform<bool> flip_texture;

		void rectangle(const Vector3f & color, const Vector2f & tl, const Vector2f & br, bool fill, float alpha, const Viewport & vp = {} );
		void rectanglePixels(const Vector3f & color, const Vector2f & center, const Vector2f & diagonalPixs, bool fill, float alpha, const Viewport & vp);
		void circle(const Vector3f & color, const Vector2f & center, float radius, bool fill, float alpha, const Vector2f & scaling = Vector3f(1, 1), int precision = 50);
		void circlePixels(const Vector3f & color, const Vector2f & center, float radius, bool fill, float alpha, const Vector2f & winSize, int precision = 50);
		void linePixels(const Vector3f & color, const Vector2f & ptA, const Vector2f & ptB, const Vector2f & winSize);

	private:

		void initBaseShader();
		void initGridShader();

	};

	struct QuadData
	{
		Vector2f center = { 0.5, 0.5 };
		Vector2f diagonal = { 0.5, 0.5 };

		Vector2f br() const { return center + diagonal; }
		Vector2f tl() const { return center - diagonal; }
	};

	struct QuadSelectionData
	{
		operator bool() const { return isActive; }
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
		operator bool() const { return isDefined; }

		Vector2i pos;
		int im;
		bool isDefined = false;
	};


	struct GridMapping {

		MVpixel pixFromScreenPos(const Vector2i & pos, const Vector2f & size);

		//uvs in opengl [1,-1]
		Vector2f uvFromMVpixel(const MVpixel & pix, bool use_center = false);

		void updateZoomBox(const Input & input, const sibr::Viewport & vp);
		void updateZoomScroll(const Input & input);
		void updateCenter(const Input & input, const Vector2f & size);
		void updateDrag(const Input & input, const Vector2f & size);

		void displayZoom(const sibr::Viewport & viewport, DrawUtilities & utils);

		void highlightPixel(const MVpixel & pix, const sibr::Viewport & viewport, DrawUtilities & utils, const sibr::Vector3f & color = { 0, 1, 0 }, const sibr::Vector2f & minPixSize = { 10.0f, 10.0f });
		void setupGrid(const Viewport & vp);


		Viewport _vp;
		QuadData viewRectangle;
		QuadSelectionData zoomSelection;
		DragClickData drag;

		int num_per_row = 4;
		Vector2f grid_adjusted;

		Vector2f imSizePixels;
		int num_imgs;
	};

	struct ImageGridLayer {	
		ITexture2DArray::Ptr imgs_texture_array;
		std::string name;
		bool flip_texture = false;
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

		void listImagesLayerGUI();
		void optionsGUI();

		bool name_collision(const std::string & name) const;
		void setupFirstLayer();

		std::list<ImageGridLayer> images_layers;
		std::list<ImageGridLayer>::iterator current_layer;
		ITexture2DArray::Ptr current_level_tex;
		float current_lod = 0;

		MVpixel currentActivePix;

		DrawUtilities draw_utils;


		//dgb
		Viewport db_vp;
		Vector2i db_mp;

	public:
		template<typename T, uint N>
		void addImageLayer(
			const std::string & layer_name,
			const std::vector<Image<T, N> > & images,
			uint flags = 0
		) {
			std::vector<cv::Mat> images_cv(images.size());
			for (size_t im = 0; im < images.size(); ++im) {
				images_cv[im] = images[im].toOpenCVBGR();
			}
			addImageLayer<T, N>(layer_name, images_cv, flags);
		}

		template<typename T, uint N>
		void addImageLayer(
			const std::string & layer_name,
			const std::vector<ImagePtr<T,N>> & images,
			uint flags = 0
		) {
			std::vector<cv::Mat> images_cv(images.size());
			for (size_t im = 0; im < images.size(); ++im) {
				images_cv[im] = images[im]->toOpenCVBGR();
			}
			addImageLayer<T, N>(layer_name, images_cv, flags);
		}

		template<typename T, uint N>
		void addImageLayer(
			const std::string & layer_name,
			const std::shared_ptr<Texture2DArray<T,N>> & images
		) {
			if (name_collision(layer_name)) {
				return;
			}

			ImageGridLayer layer;
			layer.name = layer_name;
			layer.imgs_texture_array = { std::static_pointer_cast<ITexture2DArray>(images) };

			images_layers.push_back(layer);

			setupFirstLayer();
		}

		template<typename T, uint N>
		void addImageLayer(
			const std::string & layer_name,
			const std::vector<cv::Mat> & images,
			uint flags = 0
		) {

			if (!images.size()) {
				return;
			}

			if (name_collision(layer_name)) {
				return;
			}

			ImageGridLayer layer;
			layer.name = layer_name;
			layer.imgs_texture_array = std::make_shared<Texture2DArray<T, N>>(images, flags | SIBR_GPU_AUTOGEN_MIPMAP);
			images_layers.push_back(layer);

			setupFirstLayer();
		}
	};
}
