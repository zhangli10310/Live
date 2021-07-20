//
// Created by zhangli on 2021/7/20.
//
#include <unistd.h>
#include "BaseRender.h"
#include "GLES2/gl2.h"
#include "android_log.h"
#include "malloc.h"


void BaseRender::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

void BaseRender::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

GLuint BaseRender::loadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = glCreateShader(shaderType);
    checkGlError("glCreateShader");
    LOGI("shader: %d", shader);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint BaseRender::createProgram(const char *pVertexSource, const char *pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

BaseRender::BaseRender() {
    eglWrapper = NULL;
    threadRun = true;
//    int ret = pthread_create(&thread, NULL, run, NULL);
//    if (ret != 0) {
//        LOGE("create thread error: %d", ret);
//    }
}

void BaseRender::init(ANativeWindow *window) {
    LOGI("render init");
    if (eglWrapper == NULL) {
        eglWrapper = new EglWrapper();
    }
    eglWrapper->init(window);
    printGLString("GL_RENDERER", GL_RENDERER);
    printGLString("GL_VENDOR", GL_VENDOR);
    printGLString("GL_VERSION", GL_VERSION);
    printGLString("GL_EXTENSIONS", GL_EXTENSIONS);
    onInit();
    onDraw();
    eglWrapper->swapBuffers();
}

void BaseRender::reset(int width, int height) {
    LOGI("render reset size");
    onSizeChange(width, height);
}

void BaseRender::destroy() {
    LOGI("render destroy");
    if (eglWrapper != NULL) {
        eglWrapper->destroy();
        eglWrapper = NULL;
    }
    onDestroy();
    threadRun = false;
}