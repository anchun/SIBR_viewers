#pragma once

#include <core/graphics/Texture.hpp>
#include <core/graphics/GUI.hpp>

#include "Config.hpp"

#include <opencv2/opencv.hpp>

//#include <boost/filesystem.hpp>

// must install ffdshow 
#define CV_WRITER_CODEC cv::VideoWriter::fourcc('F','F','D','S')


namespace sibr
{

	
	/**
	* \ingroup sibr_video
	*/
	class SIBR_VIDEO_EXPORT Video
	{
		SIBR_CLASS_PTR(Video);

		Video(const std::string & path = "") : filepath(path) {}
		
	public:
		virtual bool load(const std::string & path);
		const sibr::Vector2i & getResolution();
		cv::Size getResolutionCV();
		int getCurrentFrameNumber();
		void setCurrentFrame(int i);
		int getNumFrames();
		double getFrameRate();
		const Path & getFilepath() const;
		bool isLoaded();
		int getCodec();
		virtual void release();

		cv::Mat getVolume(float time_skiped_begin = 0, float time_skiped_end = 0);
		cv::Mat getVolume(int starting_frame, int ending_frame);

		cv::Mat next();

		cv::VideoCapture & getCVvideo();

		bool exists() const;

	protected:
		
		virtual void checkLoad();

		cv::VideoCapture cap;

		Path filepath;
		sibr::Vector2i resolution;
		int nFrames = 0;
		double frameRate = 0.0;
		int codec = 0;
		bool loaded = false;
	};

	/**
	* \ingroup sibr_video
	*/
	template<uint N>
	struct PingPongTexture {
		using TexPtr = std::shared_ptr<sibr::Texture2D<uchar, N>>;

		TexPtr & getLoadingTex();
		TexPtr & getDisplayTex();

		template<typename ImgType>
		void update(const ImgType & frame);

		template<typename ImgType>
		void updateGPU(const ImgType &  frame);
		
		int displayTex = 1, loadingTex = 1;
		TexPtr ping, pong;
		bool first = true;
	};

	using PingPong4u = PingPongTexture<4>;
	using PingPong3u = PingPongTexture<3>;
	using PingPong1u = PingPongTexture<1>;

	/**
	* \ingroup sibr_video
	*/
	class SIBR_VIDEO_EXPORT VideoPlayer : public Video, public ZoomInterraction
	{

		SIBR_CLASS_PTR(VideoPlayer);

		enum Mode { PAUSE, PLAY, SHOULD_CLOSE };

		using Transformation = std::function<cv::Mat(cv::Mat)>;

	public:
		VideoPlayer(const std::string & filepath = "", const std::function<cv::Mat(cv::Mat)>&  f = [](cv::Mat m) { return m; });
		
		bool load(const std::string & path) override;

		void setTransformation(const Transformation & f) { transformation = f; }
		void setMode(Mode _mode) { mode = _mode; }

		const std::shared_ptr<sibr::Texture2DRGB> & getDisplayTex() const;
		void update();
		void onGui(float ratio_display);
		
		// return true if frame was extracted
		bool updateCPU();

		void updateGPU();

		const cv::Mat & getCurrentFrame() const { return tmpFrame; }

	protected:
		std::shared_ptr<sibr::Texture2DRGB> & getLoadingTex() { return loadingTex ? ping : pong; }
		std::string dbg(int i) const { return i ? "ping" : "pong"; }
		void loadNext();

		Mode mode = PAUSE;
		bool first = true, repeat_when_end = true;

		int displayTex = 1, loadingTex = 1;
		std::shared_ptr<sibr::Texture2DRGB> ping,pong;

		cv::Mat tmpFrame;

		Transformation transformation;

		int current_frame_slider;
	};

	/**
	* \ingroup sibr_video
	*/
	template<typename T, uint N>
	struct MultipleVideoDecoder {
		using TexArray = sibr::Texture2DArray<T,N>;
		using TexArrayPtr = typename TexArray::Ptr;

		void update(const std::vector<sibr::VideoPlayer::Ptr> & videos) {
			updateCPU(videos);
			updateGPU(videos);

			loadingTexArray = (loadingTexArray + 1) % 2;

			if (first) {
				first = false;
			} else {
				displayTexArray = (displayTexArray + 1) % 2;
			}
		}

		void updateCPU(const std::vector<sibr::VideoPlayer::Ptr> & videos) {
			size_t numVids = videos.size();

			for (size_t i = 0; i < numVids; ++i) {
				videos[i]->updateCPU();
			}

		}

		void updateGPU(const std::vector<sibr::VideoPlayer::Ptr> & videos) {
			size_t numVids = videos.size();
			std::vector<cv::Mat> frames(numVids);
			for (size_t i = 0; i < numVids; ++i) {
				if (std::is_same_v<T, uchar> && N == 3) {
					frames[i] = videos[i]->getCurrentFrame();
				} else {
					std::vector<cv::Mat> cs;
					cv::split(videos[i]->getCurrentFrame(), cs);
					frames[i] = cs[0];
				}			
			}

			if (getLoadingTexArray().get()) {
				getLoadingTexArray()->updateFromImages(frames);
			} else {
				getLoadingTexArray() = TexArrayPtr(new TexArray(frames));
			}
		}

		TexArrayPtr & getLoadingTexArray() { return loadingTexArray ? ping : pong; }
		const TexArrayPtr & getDisplayTexArray() const { return displayTexArray ? ping : pong; }

		bool first = true;
		int loadingTexArray = 1, displayTexArray = 1;
		TexArrayPtr ping, pong;
	};

	using MultipleVideoDecoder1u = MultipleVideoDecoder<uchar, 1>;
	using MultipleVideoDecoder3u = MultipleVideoDecoder<uchar, 3>;

	/**
	* \ingroup sibr_video
	*/
	template<typename T, uint N>
	struct MultipleVideoDecoderArray : public MultipleVideoDecoder<T,N> {
		using TexArray = sibr::Texture2DArray<T, N>;
		using TexArrayPtr = typename TexArray::Ptr;

		void update(const std::vector<sibr::VideoPlayer::Ptr> & videos, const std::vector<int> & slices) {
			updateCPU(videos, slices);
			updateGPU(videos, slices);

			loadingTexArray = (loadingTexArray + 1) % 2;

			if (first) {
				first = false;
			} else {
				displayTexArray = (displayTexArray + 1) % 2;
			}
		}

		void updateCPU(const std::vector<sibr::VideoPlayer::Ptr> & videos, const std::vector<int> & slices) {
#pragma omp parallel for num_threads(4)
			for (int i = 0; i < (int)slices.size(); ++i) {
				//std::cout << slice << " " << std::flush;
				videos[slices[i]]->updateCPU();
			}
		}

		void updateGPU(const std::vector<sibr::VideoPlayer::Ptr> & videos, const std::vector<int> & slices) {
			int numVids = (int)videos.size();
			int numSlices = (int)slices.size();

			//std::cout << "update GPU " << std::flush;

			std::vector<cv::Mat> frames(numVids);
			for (int s = 0; s < numSlices; ++s) {
				if (std::is_same_v<T, uchar> && N == 3) {
					frames[slices[s]] = videos[slices[s]]->getCurrentFrame();
				} else {
					std::vector<cv::Mat> cs;
					cv::split(videos[slices[s]]->getCurrentFrame(), cs);
					frames[slices[s]] = cs[0];
					//videos[slices[s]]->getCurrentFrame().convertTo(frames[slices[s]], sibr::getOpenCVtype<T, N>);
				}
				//std::cout << s << " " << slices[s] << " " << std::flush;
			} 

			if (!getLoadingTexArray().get()) {
				getLoadingTexArray() = TexArrayPtr(new TexArray((uint)videos.size(), SIBR_GPU_LINEAR_SAMPLING));
			}

			CHECK_GL_ERROR;
			getLoadingTexArray()->updateSlices(frames, slices);
		}

	};

	using MultipleVideoDecoderArray1u = MultipleVideoDecoderArray<uchar, 1>;
	using MultipleVideoDecoderArray3u = MultipleVideoDecoderArray<uchar, 3>;

	template<uint N>
	std::shared_ptr<sibr::Texture2D<uchar, N>> & PingPongTexture<N>::getLoadingTex()
	{
		return loadingTex ? ping : pong;
	}

	template<uint N>
	std::shared_ptr<sibr::Texture2D<uchar,N>> & PingPongTexture<N>::getDisplayTex()
	{
		return displayTex ? ping : pong;
	}

	template<uint N> template<typename ImgType>
	void PingPongTexture<N>::update(const ImgType & frame)
	{
		if (first) {
			updateGPU(frame);
			loadingTex = (loadingTex + 1) % 2;
			first = false;
			return;
		}

		updateGPU(frame);

		displayTex = (displayTex + 1) % 2;
		loadingTex = (loadingTex + 1) % 2;
	}

	template<uint N> template<typename ImgType>
	void PingPongTexture<N>::updateGPU(const ImgType & frame)
	{
		if (getLoadingTex()) {
			getLoadingTex()->update(frame);
		} else {
			getLoadingTex() = TexPtr(new sibr::Texture2D<uchar, N>(frame, SIBR_GPU_LINEAR_SAMPLING));
		}
	}

 } // namespace sibr


  /*
  threads test



  class AtomicBool
  {
  protected:
  std::atomic<bool> a;

  public:
  AtomicBool() : a() {}

  AtomicBool(const bool &other) : a(other) {}

  AtomicBool(const AtomicBool &other) : a(other.a.load()) {}

  AtomicBool &operator=(const AtomicBool &other) { a.store(other.a.load()); return *this; }

  operator bool () const { return a.load(); }
  };

  struct WriteRead {
  AtomicBool needsRead = false, needsWrite = false, endWrite = false;
  };

  int mode_int = 0;

  WriteRead loadingStatus;

  AtomicBool firstTex = true, thread_ongoing = false;

  std::shared_ptr<std::thread> loadNextPtr;
  void loadNextMain();


  //~VideoPlayer();


  */