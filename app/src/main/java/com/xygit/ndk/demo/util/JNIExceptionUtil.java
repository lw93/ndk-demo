package com.xygit.ndk.demo.util;

/**
 * java 异常抛出接口
 *
 * @author Created by xiuyaun
 * @time on 2018/12/23
 */

public class JNIExceptionUtil {

    public void threwJNIException(Throwable throwable) throws Throwable {
        throwable.printStackTrace();
        throw throwable;
    }
}
