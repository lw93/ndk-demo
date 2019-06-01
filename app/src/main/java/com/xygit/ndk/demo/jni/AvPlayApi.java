package com.xygit.ndk.demo.jni;

import com.getkeepsafe.relinker.ReLinker;
import com.xygit.ndk.demo.base.BaseApplication;

/**
 * 音视频工具类
 *
 * @author Created by xiuyaun
 * @time on 2018/11/20
 */

public class AvPlayApi {
    static {
        ReLinker.loadLibrary(BaseApplication.getApplication(), "native-lib");
    }

    public native void startAudio(String input, String output);

    public native void startVedio(String input, Object surface);

}
