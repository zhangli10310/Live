package com.zl.mediacodec.decode.datasource

import android.media.MediaExtractor

/**
 * Created by zl on 2021/11/28.
 **/
interface DecoderDataSource {

    fun setDataSource(extractor: MediaExtractor)
}