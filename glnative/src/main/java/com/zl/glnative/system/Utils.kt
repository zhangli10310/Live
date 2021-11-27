package com.zl.glnative.system

import android.opengl.GLES20

/**
 * Created by zl on 2021/11/27.
 **/

fun loadShader(type: Int, shaderCode: String): Int {

    // create a vertex shader type (GLES20.GL_VERTEX_SHADER)
    // or a fragment shader type (GLES20.GL_FRAGMENT_SHADER)
    return GLES20.glCreateShader(type).also { shader ->

        // add the source code to the shader and compile it
        GLES20.glShaderSource(shader, shaderCode)
        GLES20.glCompileShader(shader)
    }
}

fun createProgram(vertexShaderCode: String, fragmentShaderCode: String): Int {
    val vertexShader: Int = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode)
    val fragmentShader: Int = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode)

    // create empty OpenGL ES Program
    return GLES20.glCreateProgram().also {

        // add the vertex shader to program
        GLES20.glAttachShader(it, vertexShader)

        // add the fragment shader to program
        GLES20.glAttachShader(it, fragmentShader)

        // creates OpenGL ES program executables
        GLES20.glLinkProgram(it)
    }
}