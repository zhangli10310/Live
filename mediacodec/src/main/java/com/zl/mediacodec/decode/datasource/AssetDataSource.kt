package com.zl.mediacodec.decode.datasource

import android.content.Context
import android.media.MediaExtractor

/**
 * Created by zl on 2021/11/28.
 **/
class AssetDataSource(private val context: Context, val assetName: String): DecoderDataSource {
    override fun setDataSource(extractor: MediaExtractor) {
        val fd = context.assets.openFd(assetName)
        extractor.setDataSource(fd)
    }
}