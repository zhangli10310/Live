package com.zl.glnative

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
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

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityGlBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val renderer = GLRenderer()
        binding.surfaceView.setOnClickListener {
            Log.i(TAG, "onCreate: ")

        }

        binding.surfaceView.holder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceCreated(holder: SurfaceHolder) {
                Log.i(TAG, "surfaceCreated: ")
                renderer.init(holder.surface)
            }

            override fun surfaceChanged(
                holder: SurfaceHolder,
                format: Int,
                width: Int,
                height: Int
            ) {
                Log.i(TAG, "surfaceChanged: $width,$height")
                renderer.reset(width, height)
            }

            override fun surfaceDestroyed(holder: SurfaceHolder) {
                Log.i(TAG, "surfaceDestroyed: ")
                renderer.stop()
            }

        })

//        val textureRender = GLRenderer()
//        binding.textureView.surfaceTextureListener = object : TextureView.SurfaceTextureListener {
//
//            private var thisSurface: SurfaceTexture? = null
//
//            override fun onSurfaceTextureAvailable(
//                surface: SurfaceTexture,
//                width: Int,
//                height: Int
//            ) {
//                Log.i(TAG, "onSurfaceTextureAvailable: $surface")
//                thisSurface = surface
//                textureRender.init(Surface(surface))
//            }
//
//            override fun onSurfaceTextureSizeChanged(
//                surface: SurfaceTexture,
//                width: Int,
//                height: Int
//            ) {
//                Log.i(TAG, "onSurfaceTextureSizeChanged: ")
//                textureRender.reset(width, height)
//            }
//
//            override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
//                Log.i(TAG, "onSurfaceTextureDestroyed: ")
//                textureRender.stop()
//                return true
//            }
//
//            override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {
//                Log.i(TAG, "onSurfaceTextureUpdated: $surface")
//                if (surface != thisSurface) {
//                    thisSurface = surface
//                    textureRender.init(Surface(surface))
//                }
//            }
//
//        }

    }

}