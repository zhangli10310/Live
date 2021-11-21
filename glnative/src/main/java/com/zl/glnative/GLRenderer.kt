package com.zl.glnative

import android.graphics.SurfaceTexture
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.TextureView

/**
 *  Created by zhangli04 at 2021/7/12 1:58 下午
 */
class GLRenderer {

    private val TAG = "GLRenderer"

    // called by native code
    var renderPtr: Long = 0

    init {
        System.loadLibrary("native-lib")
    }

    external fun init()

    external fun changeSurface(surface: Surface?)

    external fun resize(width: Int, height: Int)

    external fun stop()

    external fun destroy()

    protected external fun finalize()
}