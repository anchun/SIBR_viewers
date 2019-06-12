#include "FFMpegVideoEncoder.hpp"

#define QQ(rat) (rat.num/(double)rat.den)

namespace sibr {
	bool AVinit::initDone = false;

	FFVideoEncoder::FFVideoEncoder(
		const std::string & _filepath,
		double _fps,
		const sibr::Vector2i & size
	) : filepath(_filepath), fps(_fps)
	{
		AVinit::checkInit();
		init(size);
	}

	bool FFVideoEncoder::isFine() const
	{
		return initWasFine;
	}

	void FFVideoEncoder::close()
	{
		if (av_write_trailer(pFormatCtx) < 0) {
			SIBR_WRG << " Can not av_write_trailer " << std::endl;
		}

		if (video_st) {
			avcodec_close(video_st->codec);
			av_free(frameYUV);
		}
		avio_close(pFormatCtx->pb);
		avformat_free_context(pFormatCtx);

		needFree = false;
	}

	FFVideoEncoder::~FFVideoEncoder()
	{
		if (needFree) {
			close();
		}

	}

	void FFVideoEncoder::init(const sibr::Vector2i & size)
	{
		w = size[0];
		h = size[1];

		auto out_file = filepath.c_str();

		int res;

		pFormatCtx = avformat_alloc_context();

		fmt = av_guess_format(NULL, out_file, NULL);
		pFormatCtx->oformat = fmt;

		std::cout << " found format codec " << pFormatCtx->oformat->video_codec << " " <<
			(pFormatCtx->oformat->video_codec  == AV_CODEC_ID_H264 )  << std::endl;

		if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
			SIBR_WRG << " could not open file " << filepath << std::endl;
			return;
		}

		pCodec = avcodec_find_encoder(pFormatCtx->oformat->video_codec);
		if (!pCodec) {
			SIBR_WRG << " Can not find encoder " << std::endl;
			return;
		}

		video_st = avformat_new_stream(pFormatCtx, pCodec);

		if (video_st == NULL) {
			SIBR_WRG << " could not avformat_new_stream " << std::endl;
		}

		pCodecCtx = video_st->codec;
		pCodecCtx->codec_id = fmt->video_codec;
		pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
		pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
		pCodecCtx->width = w;
		pCodecCtx->height = h;
		pCodecCtx->gop_size = 10;
		pCodecCtx->time_base.num = 1;
		pCodecCtx->time_base.den = (int)std::round(fps);

		std::cout << "size : " << w << " " << h << std::endl;

		AVDictionary *param = 0;
		//H.264
		if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
			av_dict_set(&param, "preset", "slow", 0);
			av_dict_set(&param, "tune", "zerolatency", 0);
		}

		av_dump_format(pFormatCtx, 0, out_file, 1);

		res = avcodec_open2(pCodecCtx, pCodec, &param);
		if ( res < 0) {
			SIBR_WRG << " Failed to open encoder, error : " << res << std::endl;
			return;
		}

		avformat_write_header(pFormatCtx, NULL);

		frameYUV = av_frame_alloc();

		frameYUV->format = (int)pCodecCtx->pix_fmt;
		frameYUV->width = w;
		frameYUV->height = h;
		frameYUV->linesize[0] = w;
		frameYUV->linesize[1] = w / 2;
		frameYUV->linesize[2] = w / 2;

		yuSize[0] = frameYUV->linesize[0] * h;
		yuSize[1] = frameYUV->linesize[1] * h / 2;

		pkt = av_packet_alloc();

		initWasFine = true;
		needFree = true;
	}


	bool FFVideoEncoder::operator<<(cv::Mat frame)
	{
		if (!video_st) {
			return false;
		} else if (frame.cols != w || frame.rows != h) {
			return false;
		}

		cv::cvtColor(frame, cvFrameYUV, cv::COLOR_BGR2YUV_I420);
		frameYUV->data[0] = cvFrameYUV.data;
		frameYUV->data[1] = frameYUV->data[0] + yuSize[0];
		frameYUV->data[2] = frameYUV->data[1] + yuSize[1];

		//frameYUV->pts = (1.0 / std::round(fps)) *frameCount * 90;
		frameYUV->pts = (int)(frameCount*(video_st->time_base.den) / ((video_st->time_base.num) * std::round(fps)));
		++frameCount;

		return encode(frameYUV);
	}


	bool FFVideoEncoder::encode(AVFrame * frame)
	{
		int got_picture = 0;

		int ret = avcodec_encode_video2(pCodecCtx, pkt, frameYUV, &got_picture);
		if (ret < 0) {
			SIBR_WRG << " Failed to encode " << std::endl;
			return false;
		}
		if (got_picture == 1) {
			pkt->stream_index = video_st->index;
			ret = av_write_frame(pFormatCtx, pkt);
			av_packet_unref(pkt);
		}

		return true;
	}

}
