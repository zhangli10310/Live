//
// Created by zhangli on 2021/7/20.
//

#ifndef LIVE_BASERENDER_H
#define LIVE_BASERENDER_H

#include "GLES2/gl2.h"
#include "EglHelper.h"
#include <thread>
#include "BlockQueue.h"
#include <jni.h>

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

static GLuint loadShader(GLenum shaderType, const char *pSource) {
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

static GLuint createProgram(const char *pVertexSource, const char *pFragmentSource) {
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

enum ExecMsg {
    INVALID,
    RENDER,
    CHANGE_SURFACE,
    RESIZE_SURFACE,
    FINISH,
};

class BaseRender {

private:
    ANativeWindow *nwin;
    EglHelper *eglHelper;

    BlockQueue<ExecMsg> *queue;
    std::atomic<bool> threadExit;

    int currentWidth;
    int currentHeight;

    bool renderWhenDirty = false;

    void run();

protected:

    virtual void onInit() = 0;

    virtual void onSizeChange(int width, int height) = 0;

    virtual void onDraw() = 0;

    virtual void onDestroy() = 0;

public:

    GLuint* textureId; // 绑定的纹理id,如果用到纹理的话

    BaseRender();

    void setRenderWhenDirty(bool value);

    void changeSurface(JNIEnv *env, jobject surface);

    void reset(int width, int height);

    void draw();

    void destroy();

    ~BaseRender();
};

#endif //LIVE_BASERENDER_H
