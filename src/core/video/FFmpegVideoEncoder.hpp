#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <string>
#include <core/graphics/Image.hpp>
#include "Video.hpp"
#include "Config.hpp"



namespace sibr {

	/** \ingroup sibr_video
	*/
	class SIBR_VIDEO_EXPORT AVinit {

	public:

		static void checkInit() {
			if (!initDone) {
				std::cout << " ffmpeg register all" << std::endl;
				// Ignore next line warning.
				#pragma warning(suppress : 4996)
				av_register_all();
				initDone = true;
			}
		}

	private:
		static bool initDone;
	};

	// adapted from https://github.com/leixiaohua1020/simplest_ffmpeg_video_encoder/blob/master/simplest_ffmpeg_video_encoder/simplest_ffmpeg_video_encoder.cpp

	class SIBR_VIDEO_EXPORT FFVideoEncoder {

	public:
		FFVideoEncoder(
			const std::string & _filepath,
			double fps,
			const sibr::Vector2i & size
		);

		bool isFine() const;
		void close();

		bool operator << (cv::Mat frame);

		~FFVideoEncoder();

	protected:
		void init(const sibr::Vector2i & size);
		bool encode(AVFrame *frame);

		bool initWasFine = false, needFree = false;
		std::string filepath;
		int w, h;
		int frameCount = 0;
		double fps;

		AVFrame * frameYUV = NULL;
		cv::Mat cvFrameYUV;
		sibr::Vector2i yuSize;

		AVFormatContext* pFormatCtx;
		AVOutputFormat* fmt;
		AVStream* video_st;
		AVCodecContext* pCodecCtx;
		AVCodec* pCodec;
		AVPacket * pkt;

	};

}
