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
    eglWrapper = nullptr;
    queue = new BlockQueue<function<void()>>();
    threadRun.store(true);
    threadExit.store(false);

    auto lambda_fun = [&]() -> void {
        while (threadRun) {
            this->run();
        }
        threadExit.store(true);
    };
    std::thread t(lambda_fun);
    t.detach();
}

BaseRender::~BaseRender() {
    LOGI("release render");
    threadRun.store(false);
    queue->put([] {}); // push 一个空消息解除阻塞
    while (!threadExit && !queue->isQuit());
    delete queue;
    LOGI("render released");
}

void BaseRender::run() {
    LOGD("try take message");
    function<void()> func = queue->take();
    if (func != nullptr) {
        func();
    }
}


void BaseRender::init(ANativeWindow *window) {

    function<void()> lambda = [=] {
        if (this->eglWrapper == nullptr) {
            this->eglWrapper = new EglWrapper();
        }
        this->eglWrapper->init(window);
        printGLString("GL_RENDERER", GL_RENDERER);
        printGLString("GL_VENDOR", GL_VENDOR);
        printGLString("GL_VERSION", GL_VERSION);
        printGLString("GL_EXTENSIONS", GL_EXTENSIONS);
        if (this->eglWrapper->isInit()) {
            onInit();
            reset(eglWrapper->surfaceWidth, eglWrapper->surfaceHeight);
        }
        onDraw();
        eglWrapper->swapBuffers();
    };
    queue->put(lambda);
}

void BaseRender::reset(int width, int height) {
    if (width == eglWrapper->surfaceWidth && height == eglWrapper->surfaceHeight) {
        return;
    }
    LOGI("render reset size");
    function<void()> lambda = [&] {
        LOGI("onSizeChange lambda width=%d,height=%d", width, height);
        onSizeChange(width, height);
    };
    queue->put(lambda);
}

void BaseRender::draw() {
    LOGI("render reset size");
    function<void()> lambda = [=] {
        onDraw();
        EGLint ret = eglWrapper->swapBuffers();
        switch (ret) {
            case EGL_SUCCESS:
                break;
            case EGL_CONTEXT_LOST:
                break;
            default:
                break;
        }
        queue->put(lambda);
    };
    queue->put(lambda);
}

void BaseRender::destroy() {
    LOGI("render destroy");
    function<void()> lambda = [&] {
        LOGI("lambda destroy");
        if (eglWrapper != nullptr) {
            eglWrapper->destroy();
            delete eglWrapper;
            eglWrapper = nullptr;
        }
        onDestroy();
        queue->quit();
    };
//    queue->put(lambda);
}