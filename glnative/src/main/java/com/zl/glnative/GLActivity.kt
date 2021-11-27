package com.zl.glnative

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.PixelFormat
import android.graphics.SurfaceTexture
import android.opengl.GLSurfaceView
import android.os.BatteryManager
import android.os.Build
import android.os.Bundle
import android.os.PowerManager
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.TextureView
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.zl.glnative.databinding.ActivityGlBinding
import com.zl.glnative.system.GLSurfaceRender
import java.io.BufferedReader
import java.io.DataInputStream
import java.io.IOException
import java.io.InputStreamReader
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.concurrent.thread


/**
 *  Created by zhangli04 at 2021/7/12 3:36 下午
 */
class GLActivity : AppCompatActivity() {

    private val TAG = "GLActivity"
    private lateinit var binding: ActivityGlBinding

    val render = GLRenderer();

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityGlBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.surfaceView.setOnClickListener {
            Log.i(TAG, "onCreate: ")
        }

        binding.surfaceView.apply {
            setZOrderOnTop(true)
            holder.setFormat(PixelFormat.TRANSPARENT)
            holder.addCallback(GLSurfaceCallback(render.apply {
                init()
            }))
        }

        binding.glView.apply {
            setEGLContextClientVersion(2)
            setEGLConfigChooser(8, 8, 8, 8, 16, 0)
            holder.setFormat(PixelFormat.TRANSPARENT)
            setZOrderOnTop(true)
//            renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY
            setRenderer(GLSurfaceRender())
        }

    }

    override fun onDestroy() {
        super.onDestroy()
        render.destroy()
    }

}