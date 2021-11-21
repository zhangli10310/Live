package com.zl.glnative

import android.util.Log
import android.view.SurfaceHolder

/**
 * Created by zl on 2021/11/21.
 **/
class GLSurfaceCallback(private val render: GLRenderer): SurfaceHolder.Callback {

    private val TAG = "GLSurfaceCallback"

    override fun surfaceCreated(holder: SurfaceHolder) {
        Log.i(TAG, "surfaceCreated: ${holder.surface}")
        render.changeSurface(holder.surface)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        Log.i(TAG, "surfaceChanged: ${holder.surface}, $width, $height")
        render.resize(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        Log.i(TAG, "surfaceDestroyed: ${holder.surface}")
        render.changeSurface(null)
    }
}