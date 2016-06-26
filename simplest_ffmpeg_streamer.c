#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif

/* 定义输出Format, stream and so on */
AVFormatContext* pFormatCtx = NULL;
AVOutputFormat* fmt = NULL;
AVStream* audio_st = NULL;
AVCodecContext* pCodecCtx = NULL;
AVCodec* pCodec = NULL;

uint8_t* frame_buf = NULL;
AVFrame* FFrame = NULL;
AVPacket spkt;

int got_frame = 0;
int ret = 0;
int size = 0;

//int i = 0;
/* 初始化流过滤器 */
AVBitStreamFilterContext* faacbsfc = NULL;
char dsi[2] = {0};

int flush_encoder(AVFormatContext *fmt_ctx, AVBitStreamFilterContext** faacbsfc, unsigned int stream_index){
	int ret;
	int got_frame;
	AVPacket enc_pkt;
	if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
			CODEC_CAP_DELAY))
		return 0;
	while (1) {
		enc_pkt.data = NULL;
		enc_pkt.size = 0;
		av_init_packet(&enc_pkt);
		ret = avcodec_encode_audio2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt,
				NULL, &got_frame);
		av_frame_free(NULL);
		if (ret < 0)
			break;
		if (!got_frame){
			ret = 0;
			break;
		}
		printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n",enc_pkt.size);
		/* mux encoded frame */
		av_bitstream_filter_filter(*faacbsfc, fmt_ctx->streams[1]->codec, NULL, &enc_pkt.data, &enc_pkt.size, enc_pkt.data, enc_pkt.size, 0);
		ret = av_write_frame(fmt_ctx, &enc_pkt);
		if (ret < 0)
			break;
	}
	return ret;
}

int get_sr_index(unsigned int sampling_que) {
	switch(sampling_que) {
	case 44100: return 4;
	}
}

void make_dsi(unsigned int sampling, unsigned int channel, unsigned char *dsi) {
	unsigned object_type = 2;
	dsi[0] = (object_type << 3) | (sampling >> 1);
	dsi[1] = (sampling<<7) | (channel<<3);
}

JNIEXPORT jint JNICALL Java_com_qichexiaozi_service_Video_streamAAcInit
(JNIEnv *env, jobject obj, jstring outfile) {
	const char *coutfile = (*env)->GetStringUTFChars(env, outfile, NULL);

	/* 注册相应的服务 */
	av_register_all();
	avformat_network_init();

	/* 开启aac滤镜 */
	faacbsfc =  av_bitstream_filter_init("aac_adtstoasc");

	/* 打开输出流 */
	avformat_alloc_output_context2(&pFormatCtx, NULL, "flv", coutfile);

	fmt = pFormatCtx->oformat;

	//Open output URL
	if (avio_open(&pFormatCtx->pb,coutfile, AVIO_FLAG_READ_WRITE) < 0){
		printf("Failed to open output file!\n");
		return -1;
	}

	make_dsi(4, 2, dsi);
	audio_st = avformat_new_stream(pFormatCtx, 0);
	if (audio_st == NULL){
		return -1;
	}
	audio_st->codec->extradata = (uint8_t*)dsi;
	audio_st->codec->extradata_size = 2;

	pCodecCtx = audio_st->codec;
	pCodecCtx->codec_id = AV_CODEC_ID_AAC;
	pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
	pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
	pCodecCtx->sample_rate = 44100;
	pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
	pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
	pCodecCtx->bit_rate = 64000;
	pCodecCtx->codec_tag = 0;
	pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

	//Show some information
	av_dump_format(pFormatCtx, 0, coutfile, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec){
		printf("Can not find encoder!\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
		printf("Failed to open encoder!\n");
		return -1;
	}
}

JNIEXPORT jint JNICALL Java_com_qichexiaozi_service_Video_streamAAcSetFrame
(JNIEnv *env, jobject obj) {
	LOGE("malloc----");
	FFrame = av_frame_alloc();
	FFrame->nb_samples = pCodecCtx->frame_size;
	FFrame->format = pCodecCtx->sample_fmt;

	size = av_samples_get_buffer_size(NULL, pCodecCtx->channels, pCodecCtx->frame_size, pCodecCtx->sample_fmt, 1);
	frame_buf = (uint8_t *)av_malloc(size*4);
	avcodec_fill_audio_frame(FFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,(const uint8_t*)frame_buf, size, 1);
	avformat_write_header(pFormatCtx, NULL);
	av_new_packet(&spkt, size);

	return size;
}

/*
JNIEXPORT jint JNICALL Java_com_qichexiaozi_service_Video_streamAAcSendData
//(JNIEnv *env, jobject obj, jbyteArray pcmData, jint dataSize) {
	(JNIEnv *env, jobject obj) {

}*/

JNIEXPORT jint JNICALL Java_com_qichexiaozi_service_Video_streamAAcTest
(JNIEnv *env, jobject obj, jbyteArray pcmData, jint dataSize) {

	char* zPcmData = (char*)(*env)->GetByteArrayElements(env, pcmData, NULL);

	memset(frame_buf, 0, size*4);
	memcpy(frame_buf, zPcmData, dataSize);


	FFrame->data[0] = frame_buf;  //PCM Data
	//FFrame->pts = (i++)*100;
	FFrame->pts = av_frame_get_best_effort_timestamp(FFrame);
	got_frame = 0;

	//Encode
	ret = avcodec_encode_audio2(pCodecCtx, &spkt, FFrame, &got_frame);
	if(ret < 0){
		printf("Failed to encode!\n");
		return -1;
	}


	if (got_frame==1){
		spkt.stream_index = audio_st->index;
		LOGI("spkt.data:%s\n",spkt.data);
		LOGI("spkt.size:%d\n",spkt.size);
		av_bitstream_filter_filter(faacbsfc, pCodecCtx, NULL, &spkt.data, &spkt.size, spkt.data, spkt.size, 0);
		ret = av_interleaved_write_frame(pFormatCtx, &spkt);
		av_free_packet(&spkt);
	}

	(*env)->ReleaseByteArrayElements(env, pcmData, zPcmData, 0);
	return 1;
}

JNIEXPORT jint JNICALL Java_com_qichexiaozi_service_Video_streamAAcDataEnd
(JNIEnv *env, jobject obj) {
	//Flush Encoder
	ret = flush_encoder(pFormatCtx, &faacbsfc, 0);
	if (ret < 0) {
		printf("Flushing encoder failed\n");
		return -1;
	}

	//Write Trailer
	av_write_trailer(pFormatCtx);

	//Clean


	return 0;
}

JNIEXPORT jint JNICALL Java_com_qichexiaozi_service_Video_streamAAcCleanMem
(JNIEnv *env, jobject obj) {
	if (audio_st){
		//avcodec_close(pFormatCtx->streams[1]->codec);
		avcodec_close(audio_st->codec);
		av_free(FFrame);
		av_free(frame_buf);
	}


	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

}

