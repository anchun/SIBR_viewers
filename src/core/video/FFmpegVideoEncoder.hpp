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

	
	/** Video encoder using ffmpeg.
	Adapted from adapted from https://github.com/leixiaohua1020/simplest_ffmpeg_video_encoder/blob/master/simplest_ffmpeg_video_encoder/simplest_ffmpeg_video_encoder.cpp
	*/
	class SIBR_VIDEO_EXPORT FFVideoEncoder {

	public:

		/** Constructor.
		\param _filepath destination file, the extension will be used to infer the container type.
		\param fps target video framerate
		\param size target video size, prefer using power of 2 dimensions
		*/
		FFVideoEncoder(
			const std::string & _filepath,
			double fps,
			const sibr::Vector2i & size
		);

		/** \return true if the encoder was properly setup. */
		bool isFine() const;

		/** Close the file. */
		void close();

		/** Encode a frame.
		\param frame the frame to encode
		\return a success flag 
		*/
		bool operator << (cv::Mat frame);

		/** Encode a frame.
		\param frame the frame to encode
		\return a success flag 
		*/
		bool operator << (const sibr::ImageRGB & frame);

		/// Destructor.
		~FFVideoEncoder();

	protected:

		/** Setup the encoder.
		\param size the video target size, prfer using power of two.
		*/
		void init(const sibr::Vector2i & size);
		
		/** Encode a frame to the file.
		\param frame the frame to encode
		\return a success flag.
		*/
		bool encode(AVFrame *frame);

		bool initWasFine = false; ///< Was the encoder init properly.
		bool needFree = false; ///< Is the file open.
		std::string filepath; ///< Destination path.
		int w, h; ///< Dimensions.
		int frameCount = 0; ///< Current frame.
		double fps; ///< Framerate.

		AVFrame * frameYUV = NULL; ///< Working frame.
		cv::Mat cvFrameYUV; ///< Working frame data.
		sibr::Vector2i yuSize; ///< Working size.

		AVFormatContext* pFormatCtx; ///< Format context.
		AVOutputFormat* fmt; ///< Output format.
		AVStream* video_st; ///< Output stream.
		AVCodecContext* pCodecCtx; ///< Codec context.
		AVCodec* pCodec; ///< Codec.
		AVPacket * pkt; ///< Encoding packet.
		
		static bool ffmpegInitDone; ///< FFMPEG initialization status.

	};

}
