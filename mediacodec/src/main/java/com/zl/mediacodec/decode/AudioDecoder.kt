package com.zl.mediacodec.decode

import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.util.Log
import com.zl.mediacodec.decode.datasource.DecoderDataSource

/**
 * Created by zl on 2021/11/28.
 **/
class AudioDecoder(
    private val decoderDataSource: DecoderDataSource,
    private val decodeListener: DecodeListener
) {
    private val TAG = "AudioDecoder"

    private var audioIndex = -1
    private var mediaFormat: MediaFormat? = null
    private var mime: String? = null
    private val extractor by lazy { MediaExtractor() }
    private var mediacodec: MediaCodec? = null

    fun init() {
        decoderDataSource.setDataSource(extractor)
        ensureMediaType()
        val type = mime ?: return
        mediacodec = MediaCodec.createDecoderByType(type).apply {
            setCallback(object : MediaCodec.Callback() {
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
                    val outBuffer = codec.getOutputBuffer(index) ?: return
                    val chunk = ByteArray(info.size)
                    outBuffer.get(chunk)
                    outBuffer.clear()
                    decodeListener.onCapturePCM(chunk, info.size)
                    Log.i(TAG, "onOutputBufferAvailable: ${info.presentationTimeUs}")
                    codec.releaseOutputBuffer(index, true)
                }

                override fun onError(codec: MediaCodec, e: MediaCodec.CodecException) {
                    codec.reset()
                }

                override fun onOutputFormatChanged(codec: MediaCodec, format: MediaFormat) {

                }

            })
            configure(mediaFormat, null, null, 0)
        }
    }

    private fun ensureMediaType() {
        val numTracks = extractor.trackCount
        for (i in 0 until numTracks) {
            val format = extractor.getTrackFormat(i)
            val mime = format.getString(MediaFormat.KEY_MIME)
            if (interestedIn(mime)) {
                audioIndex = i
                this.mime = mime
                this.mediaFormat = format
                extractor.selectTrack(audioIndex)
                decodeListener.onParseAudioInfo(mediaFormat?.getInteger(MediaFormat.KEY_SAMPLE_RATE),
                    mediaFormat?.getInteger(MediaFormat.KEY_CHANNEL_COUNT))
                break
            }
        }
    }

    private fun interestedIn(mime: String?): Boolean {
        return mime?.startsWith("audio") == true
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
    }

    interface DecodeListener {
        fun onCapturePCM(chunk: ByteArray, size: Int)

        fun onParseAudioInfo(sampleRate: Int?, channelCount: Int?)
    }
}