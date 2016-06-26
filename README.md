# Android-aac-rtmp
使用FFmpeg做语音直、点播方案。使用RTMP进行推流，同时搭建nginx+rtmp+hls服务器


# 一起使用FFmpeg

@(FFmpeg)[直播|北京汽车小子信息科技有限公司]

**FFmpeg**是一套可以用来记录、转换数字音频、视频，并能将其转化为流的开源计算机程序。采用LGPL或GPL许可证。它提供了录制、转换以及流化音视频的完整解决方案。它包含了非常先进的音频/视频编解码库libavcodec，为了保证高可移植性和编解码质量，libavcodec里很多code都是从头开发的。
FFmpeg在Linux平台下开发，但它同样也可以在其它操作系统环境中编译运行，包括Windows、Mac OS X等。这个项目最早由Fabrice Bellard发起，现在由Michael Niedermayer维护。许多FFmpeg的开发人员都来自MPlayer项目，而且当前FFmpeg也是放在MPlayer项目组的服务器上。项目的名称来自MPEG视频编码标准，前面的"FF"代表"Fast Forward"


----------

[TOC]

## FFmpeg

>多媒体视频处理工具FFmpeg有非常强大的功能包括视频采集功能、视频格式转换、视频抓图、给视频加水印等。

通过分享此代码希望更多的开发人员加入FFmpeg阵营。 您也可以去官网 [FFmpeg官网](http://http://ffmpeg.org) 查看最新的源码.

### 代码Demo
```cpp
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
```

### 接口
| 函数      |    参数 |
| :-------- | --------:|
| Video_streamAAcInit | |
|Video_streamAAcSetFrame|
|Video_streamAAcSendData|
|Video_streamAAcDataEnd|
|Video_streamAAcCleanMem|


### 流程
待续

### FFmpeg+h264+fdk-aac交叉编译
待续

###注意
> **注意:**首先一定先将客户端连接到服务器:（RTMP协议）

> - 下面就可以使用进行推流的服务（将采集过来的PCM读取出来，放入API即可进行推流）





