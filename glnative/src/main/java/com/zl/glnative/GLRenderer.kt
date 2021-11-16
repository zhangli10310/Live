package com.zl.glnative

import android.view.Surface

/**
 *  Created by zhangli04 at 2021/7/12 1:58 下午
 */
class GLRenderer {

    // called by native code
    var renderPtr: Long = 0

    init {
        System.loadLibrary("native-lib")
    }

    external fun init(surface: Surface)

    external fun reset(width: Int, height: Int)

    external fun stop()

    protected external fun finalize()
}