package com.zl.mediacodec

import android.graphics.PixelFormat
import android.graphics.SurfaceTexture
import android.media.*
import android.os.Bundle
import android.util.Log
import android.view.Surface
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.zl.glnative.GLRenderer
import com.zl.glnative.GLSurfaceCallback
import com.zl.glnative.GLTextureListener
import com.zl.glnative.getCurrentThreadInfo
import com.zl.mediacodec.databinding.ActivityMp4Binding
import com.zl.mediacodec.decode.AudioDecoder
import com.zl.mediacodec.decode.VideoDecoder
import com.zl.mediacodec.decode.datasource.AssetDataSource
import kotlin.concurrent.thread


/**
 * Created by zl on 2021/11/18.
 **/
class Mp4Activity : AppCompatActivity() {

    private val TAG = "Mp4Activity"

    private lateinit var binding: ActivityMp4Binding

    private lateinit var render: GLRenderer

    var exit = false
    var surface: SurfaceTexture? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMp4Binding.inflate(layoutInflater)
        setContentView(binding.root)

        render = GLRenderer().apply {
            init()
            listener = object : GLRenderer.NativeCallback {

                override fun onTextureIdGenerate(textureId: Int) {
                    Log.i(TAG, "onTextureIdGenerate: ${getCurrentThreadInfo()}")
                    surface = SurfaceTexture(textureId).apply {
                        setOnFrameAvailableListener {
                            Log.i(TAG, "OnFrameAvailable: ${getCurrentThreadInfo()}, width:")
                            render.callNativeThread()
                            render.draw()
                        }
                        thread {
                            decode(this)
                        }
                    }
                }

                override fun callFromNativeThread() {
                    Log.i(TAG, "callFromNativeThread: ${getCurrentThreadInfo()}")
                    surface?.updateTexImage()
                }

            }
        }
        val textureFlag = true
        if (textureFlag) {
            binding.mp4TextureView.visibility = View.VISIBLE
            binding.mp4SurfaceView.visibility = View.GONE
            binding.mp4TextureView.isOpaque = false
            binding.mp4TextureView.surfaceTextureListener = GLTextureListener(render)
        } else {
            binding.mp4TextureView.visibility = View.GONE
            binding.mp4SurfaceView.visibility = View.VISIBLE
            binding.mp4SurfaceView.setZOrderOnTop(true)
            binding.mp4SurfaceView.holder.setFormat(PixelFormat.TRANSPARENT)
            binding.mp4SurfaceView.holder.addCallback(GLSurfaceCallback(render))
        }

    }

    private var videoDecoder: VideoDecoder? = null
    private var audioDecoder: AudioDecoder? = null
    private var audioTrack: AudioTrack? = null
    private fun decode(surface: SurfaceTexture) {
        val dataSource = AssetDataSource(this, "video.mp4")
        videoDecoder = VideoDecoder(
            dataSource,
            surface
        ) { w, h ->
            render.setTextureSize(w, h)
        }.apply {
            init()
            start()
        }
        audioDecoder = AudioDecoder(dataSource, object : AudioDecoder.DecodeListener {
            override fun onCapturePCM(chunk: ByteArray, size: Int) {
                audioTrack?.play()
                audioTrack?.write(chunk, 0, size)
            }

            override fun onParseAudioInfo(sampleRate: Int?, channelCount: Int?) {
                val attr = AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_MEDIA)
                    .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC).build()
                val config = if (channelCount == 1) {
                    AudioFormat.CHANNEL_IN_MONO
                } else {
                    AudioFormat.CHANNEL_IN_STEREO
                }
                val format = AudioFormat.Builder()
                    .setSampleRate(sampleRate ?: AudioFormat.ENCODING_PCM_16BIT)
                    .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                    .setChannelMask(config)
                    .build()
                val bufferSize = AudioTrack.getMinBufferSize(format.sampleRate, config, AudioFormat.ENCODING_PCM_16BIT);
                audioTrack = AudioTrack(
                    attr,
                    format,
                    bufferSize,
                    AudioTrack.MODE_STREAM,
                    AudioManager.AUDIO_SESSION_ID_GENERATE
                )
            }

        }).apply {
            init()
            start()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        exit = true
        videoDecoder?.stop()
        videoDecoder?.release()
        audioDecoder?.stop()
        audioDecoder?.release()
        render.destroy()
    }
}