package com.zl.glnative

/**
 * Created by zl on 2021/11/25.
 **/


fun getCurrentThreadInfo(): String {
    val thread = Thread.currentThread()
    return "thread name:${thread.name}, id:${thread.id}"
}