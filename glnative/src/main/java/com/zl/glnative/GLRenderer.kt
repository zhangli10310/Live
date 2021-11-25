package com.zl.glnative

import android.util.Log
import android.view.Surface

/**
 *  Created by zhangli04 at 2021/7/12 1:58 下午
 */
class GLRenderer {

    private val TAG = "GLRenderer"

    // called by native code
    var renderPtr: Long = 0

    var listener: NativeCallback? = null

    init {
        System.loadLibrary("native-lib")
    }

    fun callFromNative(textureId: Int) {
        Log.i(TAG, "callFromNative: $textureId, ${getCurrentThreadInfo()}")
        listener?.onTextureIdGenerate(textureId)
    }

    fun callFromNativeThread() {
        listener?.callFromNativeThread()
    }

    external fun init()

    external fun changeSurface(surface: Surface?)

    external fun resize(width: Int, height: Int)

    external fun setTextureSize(width: Int, height: Int)

    external fun draw()

    /**
     *  调用native函数，native再回调Java方法
     *  使调用的Java方法和native在同一个线程
     *  如果直接在native生成纹理就不用这么麻烦了
     */
    external fun callNativeThread()

    external fun stop()

    external fun destroy()

    protected external fun finalize()

    interface NativeCallback {
        fun onTextureIdGenerate(textureId: Int)

        fun callFromNativeThread()
    }
}