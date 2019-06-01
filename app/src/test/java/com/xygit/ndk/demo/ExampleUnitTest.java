package com.xygit.ndk.demo;

import org.junit.Test;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() throws Exception {
        assertEquals(4, 2 + 2);
    }

    @Test
    public void path() {
       String path = "file:///0/0we02/233/423.apk";
        String tmp = path.substring(0, path.lastIndexOf("."));
        System.out.println(tmp);
    }
}