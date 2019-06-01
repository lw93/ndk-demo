package com.xygit.ndk.demo.base;

import android.annotation.SuppressLint;
import android.app.Application;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Environment;

import cn.onlinecache.breakpad.NativeBreakpad;

/**
 * @author Created by xiuyaun
 * @time on 2018/11/15
 */

public class BaseApplication extends Application {

    public static final String ROOTPATH = "/xygit";

    @SuppressLint("StaticFieldLeak")
    protected static Application application;

    public static Application getApplication() {
        return application;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        application = this;
        if (Environment.MEDIA_MOUNTED.equalsIgnoreCase(Environment.getExternalStorageState())) {
            NativeBreakpad.init(getExternalCacheDir().getAbsolutePath() + ROOTPATH);
        } else {
            NativeBreakpad.init(getCacheDir().getAbsolutePath() + ROOTPATH);
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
    }

    @Override
    public void onTrimMemory(int level) {
        super.onTrimMemory(level);
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
    }

}
