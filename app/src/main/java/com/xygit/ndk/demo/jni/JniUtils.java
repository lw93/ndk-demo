package com.xygit.ndk.demo.jni;

import com.getkeepsafe.relinker.ReLinker;
import com.xygit.ndk.demo.base.BaseApplication;

/**
 * 第三方 so 库
 *
 * @author Created by xiuyaun
 * @time on 2018/11/14
 */

public class JniUtils {

    public static void loadLibs() {
        //ffmpeg 库
        ReLinker.loadLibrary(BaseApplication.getApplication(), "avutil-54");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "swresample-1");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "avcodec-56");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "avformat-56");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "swscale-3");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "postproc-53");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "avfilter-5");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "avdevice-56");
        ReLinker.loadLibrary(BaseApplication.getApplication(), "avdevice-56");
        //libyuv 库
        ReLinker.loadLibrary(BaseApplication.getApplication(), "yuv");

    }
}
