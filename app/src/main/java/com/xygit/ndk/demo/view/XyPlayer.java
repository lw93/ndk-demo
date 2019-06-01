package com.xygit.ndk.demo.view;

import android.content.Context;
import android.graphics.PixelFormat;
import android.os.Environment;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;

import com.xygit.ndk.demo.jni.AvPlayApi;
import com.xygit.ndk.demo.jni.JniUtils;
import com.xygit.ndk.demo.util.MediaFileUtil;

import java.io.File;

/**
 * 自定义播放器
 *
 * @author Created by xiuyaun
 * @time on 2018/11/20
 */

public class XyPlayer extends SurfaceView {

    static {
        JniUtils.loadLibs();
    }

    private static final AvPlayApi ffmpegApi = new AvPlayApi();

    public XyPlayer(Context context) {
        super(context);
        init();
    }

    public XyPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public XyPlayer(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        //初始化，SufaceView绘制的像素格式
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
//        setLayerType(View.LAYER_TYPE_SOFTWARE, null);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        // 获取宽-测量规则的模式和大小
        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);

        // 获取高-测量规则的模式和大小
        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);

        // 设置wrap_content的默认宽 / 高值
        // 默认宽/高的设定并无固定依据,根据需要灵活设置
        // 类似TextView,ImageView等针对wrap_content均在onMeasure()对设置默认宽 / 高值有特殊处理,具体读者可以自行查看
        int mHeight = 480;
        int mWidth = 480;
        // 当布局参数设置为wrap_content时，设置默认值
        if (getLayoutParams().width == ViewGroup.LayoutParams.WRAP_CONTENT && getLayoutParams().height == ViewGroup.LayoutParams.WRAP_CONTENT) {
            setMeasuredDimension(mWidth, mHeight);
            // 宽 / 高任意一个布局参数为= wrap_content时，都设置默认值
        } else if (getLayoutParams().width == ViewGroup.LayoutParams.WRAP_CONTENT) {
            setMeasuredDimension(mWidth, heightSize);
        } else if (getLayoutParams().height == ViewGroup.LayoutParams.WRAP_CONTENT) {
            setMeasuredDimension(widthSize, mHeight);
        }
    }

    public void start(String path) {
        String tmp = path.substring(0, path.lastIndexOf("."));
        Log.e("PATH_TAG", tmp);
        Log.e("PATH_TAG2", new File(Environment.getExternalStorageDirectory(), "/1/任然-空空如也.mp3").getAbsolutePath());


        if (MediaFileUtil.isAudioFileType(path)) {
            ffmpegApi.startAudio(path, tmp + "_test.mp3");
//            ffmpegApi.startAudio(new File(Environment.getExternalStorageDirectory(), "/1/任然-空空如也.mp3").getAbsolutePath(), new File(Environment.getExternalStorageDirectory(), "/1/任然-空空如也_test.mp3").getAbsolutePath());
        } else if (MediaFileUtil.isVideoFileType(path)) {
            ffmpegApi.startVedio(path, getHolder().getSurface());
        } else {

        }
    }
}
