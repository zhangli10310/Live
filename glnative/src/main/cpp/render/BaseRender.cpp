//
// Created by zhangli on 2021/7/20.
//
#include <unistd.h>
#include <android/native_window_jni.h>
#include "BaseRender.h"
#include "GLES2/gl2.h"
#include "android_log.h"
#include "malloc.h"

BaseRender::BaseRender() {
    eglHelper = nullptr;
    queue = new BlockQueue<ExecMsg>(INVALID);
    threadExit.store(false);

    auto lambda_fun = [&]() -> void {
        if (this->eglHelper == nullptr) {
            this->eglHelper = new EglHelper();
        }
        eglHelper->start();
        while (!queue->isQuit()) {
            LOGD("thread run");
            this->run();
        }
        LOGI("thread exit");
        threadExit.store(true);
    };
    std::thread t(lambda_fun);
    t.detach();
}

BaseRender::~BaseRender() {
    LOGI("release render");
    destroy();
//    threadRun.store(false);
    while (!threadExit || !queue->isQuit());
    delete queue;
    LOGI("render released");
}

void BaseRender::run() {
    LOGD("try take message");
    ExecMsg msgId = queue->take();
    switch (msgId) {
        case INVALID:
            break;
        case CALL_BACK: {
            if (currentState != READY) {
                break;
            }
            callback();
        }
        case RENDER: {
            if (currentState != READY) {
                break;
            }
            onDraw();
            GLint ret = eglHelper->swapBuffers();
            // handle ret EGL_SUCCESS, EGL_CONTEXT_LOST
            LOGD("swapBuffers ret=%x", ret);
            if (!renderWhenDirty) {
                draw();
            }
            break;
        }
        case CHANGE_SURFACE: {
            bool initSuccess = eglHelper->createEglSurface(nwin);
            if (initSuccess) {
                LOGI("egl surface has init");
                //打印GL信息
                printGLString("GL_RENDERER", GL_RENDERER);
                printGLString("GL_VENDOR", GL_VENDOR);
                printGLString("GL_VERSION", GL_VERSION);
                printGLString("GL_EXTENSIONS", GL_EXTENSIONS);
                currentState = READY;
                onInit();
                reset(eglHelper->surfaceWidth, eglHelper->surfaceHeight);
            } else {
                currentState = NO_SURFACE;
                LOGI("clear queue because surface changed");
                queue->clear(RENDER);
            }
            break;
        }
        case RESIZE_SURFACE: {
            onSizeChange(currentWidth, currentHeight);
            if (!renderWhenDirty) {
                draw();
            }
            break;
        }
        case FINISH: {
            currentState = DESTROY;
            LOGI("render start finish");
            eglHelper->destroy();
            delete eglHelper;
            eglHelper = nullptr;
            queue->quit();
            LOGI("render finished");
            break;
        }
    }
}

void BaseRender::setRenderWhenDirty(bool value) {
    renderWhenDirty = value;
}

void BaseRender::changeSurface(JNIEnv *env, jobject surface) {
    if (nwin != nullptr) {
        ANativeWindow_release(nwin);
        nwin = nullptr;
    }
    if (surface != nullptr) {
        nwin = ANativeWindow_fromSurface(env, surface);
    }
    queue->put(CHANGE_SURFACE);
}

void BaseRender::reset(int width, int height) {
    if (width == currentWidth && currentHeight) {
        return;
    }
    currentWidth = width;
    currentHeight = height;
    LOGI("reset size width=%d,height=%d", width, height);
    queue->put(RESIZE_SURFACE);
}

void BaseRender::draw() {
    LOGD("draw()");
    queue->clear(RENDER);
    queue->put(RENDER);
}

void BaseRender::callFromNativeThread() {
    LOGD("callFromNativeThread()");
    queue->put(CALL_BACK);
}

void BaseRender::setTextureSize(int width, int height) {
    textureWidth = width;
    textureHeight = height;
}

void BaseRender::destroy() {
    LOGI("render destroy");
    queue->clear(RENDER);
    queue->put(FINISH);
}