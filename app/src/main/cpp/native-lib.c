#include "native-lib.h"
#include<string.h>
//void native_format_log(const char *tag, const char *message) {
//#ifdef Debug
//#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__);
//#endif
//#ifdef Debug
//    LOGE(message);
//#endif
//}

int scaleImg(AVCodecContext *pContext, AVFrame *pFrame, AVFrame *pAVFrame, int i, int j);

void native_format_log(const char *tag, const char *log, ...) {
#ifdef Debug
    va_list arg;
    va_start(arg, log);
    __android_log_vprint(ANDROID_LOG_ERROR, tag, log, arg);
    va_end(arg);
#endif
}

void native_threw_exception(JNIEnv *env) {
    jclass clazz = (*env)->FindClass(env, "com/xygit/ndk/demo/util/JNIExceptionUtil");
    if (NULL != clazz) {
        jthrowable throwable = (*env)->ExceptionOccurred(env);
        jmethodID methodId = (*env)->GetStaticMethodID(env, clazz, "threwJNIException",
                                                       "(Ljava/lang/Throwable;)V");
        if (NULL != throwable) {
            (*env)->ExceptionClear(env);
            (*env)->CallStaticVoidMethod(env, clazz, methodId, throwable);
        }
        (*env)->DeleteLocalRef(env, throwable);
        (*env)->DeleteLocalRef(env, clazz);
    }
}


JNIEXPORT void JNICALL
Java_com_xygit_ndk_demo_jni_AvPlayApi_startAudio(JNIEnv *env, jobject obj, jstring input,
                                                 jstring output) {//音频
    const char *TAG = "Java_com_xygit_ndk_demo_jni_AvPlayApi_startAudio";
    const char *input_cstr = (*env)->GetStringUTFChars(env, input, NULL);
    const char *output_cstr = (*env)->GetStringUTFChars(env, output, NULL);
    //注册所有组件
    av_register_all();
    //获取上下文等信息
    //AVFormatContext 是封装格式上下文结构体,统领全局,保存视频文件的封装格式信息
    AVFormatContext *avFormatContext = avformat_alloc_context();
    // Open an input stream and read the header. The codecs are not opened.
    //* The stream must be closed with avformat_close_input().
    //打开输入文件
    if (avformat_open_input(&avFormatContext, input_cstr, NULL, NULL) != 0) {
        native_format_log(TAG, "无法打开音频文件");
        return;
    }
    //获取文件信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        native_format_log(TAG, "无法获取文件信息");
        return;
    }
    //获取流索引位置
    int i = 0, audio_frame_idx = -1;
    for (; i < avFormatContext->nb_streams; ++i) {
        //如果AVStream  pFormatCtx->streams[i] 视频文件中每个视频（音频）流对应一个该结构体
        //AVCodecContext   pFormatCtx->streams[i]->codec-> 编码器上下文结构体，保存了视频（音频）编解码相关信息
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_frame_idx = i;
            break;
        }
    }
    AVStream *avStream = avFormatContext->streams[audio_frame_idx];
    AVCodecContext *avCodecContext = avStream->codec;
    //获取解码器
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);//查找解码器
    if (NULL == avCodec) {
        native_format_log(TAG, "未找到解码器");
        return;
    }
    //打开解码器。
    //int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        native_format_log(TAG, "解码器打开失败");
        return;
    }
    //int av_read_frame(AVFormatContext *s, AVPacket *pkt);
    //AVPacket 存储一帧压缩编码数据。的结构体 需要申请堆中的空间
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    //AvFrame  存储一帧解码后像素（采样）数据。需要申请空间
    AVFrame *avFrame = av_frame_alloc();
    SwrContext *swrContext = swr_alloc();
    //采样格式
    enum AVSampleFormat in_sample_format = avCodecContext->sample_fmt;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_format = AV_SAMPLE_FMT_S16;
    //获取原声音的声道布局
    int64_t in_ch_layout = avCodecContext->channel_layout;
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;//立体声
    //输出输入采样频率
    int out_sample_rate = 44100;
    int in_sample_rate = 44100;
    swr_alloc_set_opts(
            swrContext,
            out_ch_layout,//声音模式 左右声道,立体声等等
            out_sample_format,
            out_sample_rate,
            in_ch_layout,
            in_sample_format,
            in_sample_rate,
            0, NULL
    );
    swr_init(swrContext);
    //16bit 44100 PCM 数据
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRME_SIZE);
    FILE *fp_pcm = fopen(output_cstr, "wb");
    int errNum = 0;
    if (fp_pcm == NULL) {
        errNum = errno;
        native_format_log(TAG, "open fail errno = %d reason = %s \n", errNum, strerror(errNum));
    }
    //声道的个数
    int dst_nb_channels = avCodecContext->channels;
    int got_frame_ptr = 0, index = 0;
    //从输入文件读取一帧压缩数据
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        //                       将每一帧packet中的数据,写到Frame中
        int result = avcodec_decode_audio4(avCodecContext, avFrame, &got_frame_ptr, avPacket);
        if (result < 0) {
            native_format_log(TAG, "解码完成");
        }

        /*got_frame_ptr 是一个指针,表示是否完成一阵的解码
                Zero if no frame could be decoded, otherwise it is
                no given decode call is guaranteed to produce a frame*/

        if (got_frame_ptr > 0) {
            native_format_log(TAG, "正在解码%2d", index++);
            //warning: passing argument 4 of 'swr_convert' from incompatible pointer type [enabled by default]
            swr_convert(swrContext, &out_buffer, MAX_AUDIO_FRME_SIZE,
                        (const uint8_t **) avFrame->data,
                        avFrame->nb_samples);
            int dst_bufsize = av_samples_get_buffer_size(NULL, dst_nb_channels, avFrame->nb_samples,
                                                         out_sample_format, 1);
            if (dst_bufsize < 0) {
                native_format_log(TAG, "获取buffer大小失败");
                return;
            }
            fwrite(out_buffer, 1, sizeof(dst_bufsize), fp_pcm);
            native_threw_exception(env);
        }
        av_free(avPacket);
    }
    //释放文件流
    fclose(fp_pcm);
    //释放Frame空间
    av_frame_free(&avFrame);
    //释放缓冲区
    av_free(out_buffer);
    //释放采样上下文
    swr_free(&swrContext);
    //释放解码上下文
    avcodec_close(avCodecContext);
    //释放封装格式上下文
    avformat_close_input(&avFormatContext);
    (*env)->ReleaseStringUTFChars(env, input, input_cstr);
    (*env)->ReleaseStringUTFChars(env, output, output_cstr);
}

int scaleImg(AVCodecContext *pContext, AVFrame *src_picture, AVFrame *dst_picture, int nDstH,
             int nDstW) {
    const char *TAG = "scaleImg";
    int i, nSrcStride[3], nDstStride[3], nSrcH = pContext->height, nSrcW = pContext->width;
    struct SwsContext *m_pSwsContext;
    uint8_t *pSrcBuff[3] = {src_picture->data[0], src_picture->data[1], src_picture->data[2]};
    nSrcStride[0] = nSrcW;
    nSrcStride[1] = nSrcW / 2;
    nSrcStride[2] = nSrcW / 2;

    dst_picture->linesize[0] = nDstH;
    dst_picture->linesize[1] = nDstH / 2;
    dst_picture->linesize[2] = nDstH / 2;
    native_format_log(TAG, "nSrcW%2d\n", nSrcW);
    m_pSwsContext = sws_getContext(nSrcW, nSrcH, PIX_FMT_YUV420P10, nDstW, nDstH, PIX_FMT_YUV420P10,
                                   SWS_BICUBIC, NULL, NULL, NULL);
    if (NULL == m_pSwsContext) {
        native_format_log(TAG, "ffmpeg get context error");
        exit(-1);
    }
    sws_scale(m_pSwsContext, src_picture->data, src_picture->linesize, 0, pContext->height,
              dst_picture->data, dst_picture->linesize);
    native_format_log(TAG, "line0:%d line1:%d line2:%d\n", dst_picture->linesize[0],
                      dst_picture->linesize[1], dst_picture->linesize[2]);
    sws_freeContext(m_pSwsContext);
    native_format_log(TAG, "缩放成功");
    return 1;
}

JNIEXPORT void JNICALL
Java_com_xygit_ndk_demo_jni_AvPlayApi_startVedio(JNIEnv *env, jobject obj, jstring input,
                                                 jobject surface) {//视频
    const char *TAG = "Java_com_xygit_ndk_demo_jni_AvPlayApi_startVedio";
    const char *input_cstr = (*env)->GetStringUTFChars(env, input, NULL);
    //注册所有组件
    av_register_all();
    //获取封装格式上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //打开输入视频文件
    if (avformat_open_input(&avFormatContext, input_cstr, NULL, NULL) != 0) {
        native_format_log(TAG, "打开输入文件视频失败");
        return;
    }
    //获取视频信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        native_format_log(TAG, "获取视频信息失败");
        return;
    }
    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int i = 0, video_stream_indx = -1;
    for (; i < avFormatContext->nb_streams; ++i) {
        //根据类型判断，是否是视频流
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_indx = i;
            break;
        }
    }
    //获取视频解码器
    AVCodecContext *avCodecContext = avFormatContext->streams[video_stream_indx]->codec;
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (NULL == avCodec) {
        native_format_log(TAG, "无法解码");
        return;
    }
    //打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        native_format_log(TAG, "解码器无法打开");
        return;
    }
    //编码数据
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    //像素数据(解码数据)
    AVFrame *yuvFrame = av_frame_alloc();
    AVFrame *rgbFrame = av_frame_alloc();
    //scaleImage 缩放视频
    if (scaleImg(avCodecContext, yuvFrame, rgbFrame, avCodecContext->width / 6,
                 avCodecContext->width / 6) != 1) {
        native_format_log(TAG, "视频缩放失败");
        return;
    }
    //native绘制
    //窗体
    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, surface);
    //绘制时的缓冲区
    ANativeWindow_Buffer outBuffer;
    int len, got_frame, framecount = 0;
    //一阵一阵读取压缩的视频数据AVPacket
    while (av_read_frame(avFormatContext, avPacket) > 0) {
        //解码AVPacket->AVFrame
        len = avcodec_decode_video2(avCodecContext, yuvFrame, &got_frame, avPacket);

        //Zero if no frame could be decompressed
        //非零，正在解码
        if (got_frame) {
            native_format_log(TAG, "视频正在解码%2d帧", framecount++);
            //lock
            //设置缓冲区的属性（宽、高、像素格式）
            ANativeWindow_setBuffersGeometry(aNativeWindow, avCodecContext->width,
                                             avCodecContext->height, WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(aNativeWindow, &outBuffer, NULL);
            //设置rgb_frame的属性（像素格式、宽高）和缓冲区
            //rgb_frame缓冲区与outBuffer.bits是同一块内存
            avpicture_fill((AVPicture *) rgbFrame, outBuffer.bits, AV_PIX_FMT_RGBA,
                           avCodecContext->width, avCodecContext->height);
            //YUV->RGBA_8888
            I420ToRGBA(yuvFrame->data[0], yuvFrame->linesize[0],
                       yuvFrame->data[2], yuvFrame->linesize[2],
                       yuvFrame->data[1], yuvFrame->linesize[1],
                       rgbFrame->data[0], rgbFrame->linesize[0],
                       avCodecContext->width, avCodecContext->height);
            //unlock
            ANativeWindow_unlockAndPost(aNativeWindow);
            usleep(1000 * 16);
        }
        av_free_packet(avPacket);
    }
    ANativeWindow_release(aNativeWindow);
    av_frame_free(&yuvFrame);
    avcodec_close(avCodecContext);
    avformat_free_context(avFormatContext);
    (*env)->ReleaseStringUTFChars(env, input, input_cstr);
}