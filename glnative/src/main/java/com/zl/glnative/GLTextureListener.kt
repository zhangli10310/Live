package com.zl.glnative

import android.graphics.SurfaceTexture
import android.util.Log
import android.view.Surface
import android.view.TextureView

/**
 * Created by zl on 2021/11/21.
 **/
class GLTextureListener(private val render: GLRenderer) : TextureView.SurfaceTextureListener {

    private val TAG = "GLTextureListener"

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        Log.i(TAG, "${getCurrentThreadInfo()},onSurfaceTextureAvailable: ")
        render.changeSurface(Surface(surface))
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {
        Log.i(TAG, "onSurfaceTextureSizeChanged: ")
        render.resize(width, height)
    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        Log.i(TAG, "onSurfaceTextureDestroyed: ")
        render.stop()
        return true
    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {
        Log.d(TAG, "onSurfaceTextureUpdated: ")
    }
}