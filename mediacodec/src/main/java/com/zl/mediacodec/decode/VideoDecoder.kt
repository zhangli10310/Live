package com.zl.mediacodec.decode

import android.graphics.SurfaceTexture
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.os.Handler
import android.os.HandlerThread
import android.util.Log
import android.view.Surface
import com.zl.mediacodec.decode.datasource.DecoderDataSource

/**
 * Created by zl on 2021/11/28.
 **/
class VideoDecoder(
    private val decoderDataSource: DecoderDataSource,
    private val surface: SurfaceTexture,
    private val onParseVideoSize: OnParseVideoSize?,
) {

    private val TAG = "VideoDecoder"

    private var videoIndex = -1
    private var mediaFormat: MediaFormat? = null
    private var mime: String? = null
    private val extractor by lazy { MediaExtractor() }
    private var mediacodec: MediaCodec? = null

    private var handler: Handler
    val ht = HandlerThread("tag")
    init {
        ht.start()
        handler = Handler(ht.looper)
    }

    fun init() {
        decoderDataSource.setDataSource(extractor)
        ensureMediaType()
        val type = mime ?: return
        mediacodec = MediaCodec.createDecoderByType(type).apply {
            setCallback(object : MediaCodec.Callback() {
                private var startTimeStamp = 0L
                private var startTimeUs = 0L

                override fun onInputBufferAvailable(codec: MediaCodec, index: Int) {
                    val inputBuffer = codec.getInputBuffer(index)
                    inputBuffer ?: return
                    val readCount = extractor.readSampleData(inputBuffer, 0)
                    if (readCount < 0) {
                        codec.queueInputBuffer(index, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM)
                        return
                    }

                    val currentTime = extractor.sampleTime // 当前时间戳
                    val currentFlag = extractor.sampleFlags // 当前帧的标志位
                    extractor.advance()

                    codec.queueInputBuffer(index, 0, readCount, currentTime, currentFlag)
                }

                override fun onOutputBufferAvailable(
                    codec: MediaCodec,
                    index: Int,
                    info: MediaCodec.BufferInfo
                ) {
                    val timeUs = info.presentationTimeUs
                    if (timeUs == 0L) {
                        startTimeStamp = System.nanoTime()
                    }
                    val clientTimeGap = (System.nanoTime() - startTimeStamp)/1000
                    val sampleTimeGap = timeUs - startTimeUs
                    Log.i(TAG, "${Thread.currentThread()},onOutputBufferAvailable: $sampleTimeGap, $clientTimeGap")
                    if (sampleTimeGap > clientTimeGap) {
                        Thread.sleep((sampleTimeGap - clientTimeGap) / 1000)
                    }
                    codec.releaseOutputBuffer(index, true)
                }

                override fun onError(codec: MediaCodec, e: MediaCodec.CodecException) {
                    codec.reset()
                }

                override fun onOutputFormatChanged(codec: MediaCodec, format: MediaFormat) {

                }

            }, handler)
            configure(mediaFormat, Surface(surface), null, 0)
        }
    }

    private fun ensureMediaType() {
        val numTracks = extractor.trackCount
        for (i in 0 until numTracks) {
            val format = extractor.getTrackFormat(i)
            val mime = format.getString(MediaFormat.KEY_MIME)
            Log.i(TAG, "ensureMediaType: $mime")
            if (interestedIn(mime)) {
                videoIndex = i
                this.mime = mime
                this.mediaFormat = format
                val width = format.getInteger(MediaFormat.KEY_WIDTH)
                val height = format.getInteger(MediaFormat.KEY_HEIGHT)
                onParseVideoSize?.onSize(width, height)
                extractor.selectTrack(videoIndex)
                break
            }
        }
    }

    private fun interestedIn(mime: String?): Boolean {
        return mime?.startsWith("video") == true
    }

    fun start() {
        mediacodec?.start()
    }

    fun stop() {
        mediacodec?.stop()
    }

    fun release() {
        mediacodec?.release()
        mediacodec = null
        extractor.release()
        surface.release()
        ht.quit()
    }

    fun interface OnParseVideoSize {
        fun onSize(width: Int, height: Int)
    }
}