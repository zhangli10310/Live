package com.zl.mediacodec

import android.graphics.SurfaceTexture
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.os.Bundle
import android.util.Log
import android.view.Surface
import androidx.appcompat.app.AppCompatActivity
import com.zl.glnative.GLRenderer
import com.zl.glnative.GLTextureListener
import com.zl.glnative.getCurrentThreadInfo
import com.zl.mediacodec.databinding.ActivityMp4Binding
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
                            Log.i(TAG, "OnFrameAvailable: ${getCurrentThreadInfo()}")
//                        it.updateTexImage()
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
        binding.mp4TextureView.surfaceTextureListener = GLTextureListener(render)

//        binding.mp4TextureView.surfaceTextureListener =
//            object : TextureView.SurfaceTextureListener {
//                override fun onSurfaceTextureAvailable(
//                    surface: SurfaceTexture,
//                    width: Int,
//                    height: Int
//                ) {
//                    Log.i(TAG, "onSurfaceTextureAvailable: ")
//                    thread {
//                        decode(surface, width, height)
//                    }
//                }
//
//                override fun onSurfaceTextureSizeChanged(
//                    surface: SurfaceTexture,
//                    width: Int,
//                    height: Int
//                ) {
//                    Log.i(TAG, "onSurfaceTextureSizeChanged: ")
//                }
//
//                override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
//                    Log.i(TAG, "onSurfaceTextureDestroyed: ")
//                    return true
//                }
//
//                override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {
//                    Log.d(TAG, "onSurfaceTextureUpdated: ")
//                }
//
//            }
    }

    private fun decode(surface: SurfaceTexture) {
        val extractor = MediaExtractor()
        val fd = assets.openFd("test.mp4")
        extractor.setDataSource(fd)

        var videoIndex = -1
        var mime: String? = null
        var format: MediaFormat? = null
        for (i in 0 until extractor.trackCount) {
            format = extractor.getTrackFormat(i)
            mime = format.getString(MediaFormat.KEY_MIME) ?: continue
            if (mime.startsWith("video")) {
                videoIndex = i
                break
            }
        }
        if (videoIndex < 0 || mime == null || format == null) {
            return
        }
        val width = format.getInteger(MediaFormat.KEY_WIDTH)
        val height = format.getInteger(MediaFormat.KEY_HEIGHT)
        render.setTextureSize(width, height)
        extractor.selectTrack(videoIndex)
        val decoder = MediaCodec.createDecoderByType(mime)
        decoder.configure(format, Surface(surface), null, 0)
        decoder.start()
        val info = MediaCodec.BufferInfo()
        var videoEOS = false
        while (!exit) {
            if (!videoEOS) {
                videoEOS = putBufferToCoder(extractor, decoder)
            }
            val outputBufferIndex = decoder.dequeueOutputBuffer(info, -1)
            when (outputBufferIndex) {
                MediaCodec.INFO_OUTPUT_FORMAT_CHANGED -> {
                    Log.d(TAG, "decode: format change")
                }
                MediaCodec.INFO_TRY_AGAIN_LATER -> {
                    Log.d(TAG, "decode: try again")
                }
                MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED -> {
                    Log.d(TAG, "decode: output buffer change")
                }
                else -> {
                    Thread.sleep(50)
                    decoder.releaseOutputBuffer(outputBufferIndex, true)
                }
            }
            if ((info.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                Log.i(TAG, "decode: break")
                break
            }
        }
        decoder.stop()
        decoder.release()
        extractor.release()

    }

    private fun putBufferToCoder(
        extractor: MediaExtractor,
        decoder: MediaCodec
    ): Boolean {
        var isMediaEOS = false
        val inputBufferIndex = decoder.dequeueInputBuffer(-1)
        Log.d(TAG, "putBufferToCoder: $inputBufferIndex")
        if (inputBufferIndex >= 0) {
            val inputBuffer = decoder.getInputBuffer(inputBufferIndex) ?: return false
            val sampleSize = extractor.readSampleData(inputBuffer, 0)
            if (sampleSize < 0) {
                decoder.queueInputBuffer(
                    inputBufferIndex,
                    0,
                    0,
                    0,
                    MediaCodec.BUFFER_FLAG_END_OF_STREAM
                )
                isMediaEOS = true
                Log.v(TAG, "media eos")
            } else {
                decoder.queueInputBuffer(inputBufferIndex, 0, sampleSize, extractor.sampleTime, 0)
                extractor.advance()
            }
        }
        return isMediaEOS
    }

    override fun onDestroy() {
        super.onDestroy()
        exit = true
        surface?.release()
    }
}