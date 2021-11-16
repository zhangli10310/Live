//
// Created by zhangli on 2021/7/19.
//

#include "EglWrapper.h"
#include "android_log.h"

void EglWrapper::init(ANativeWindow *window) {
//    nativeWindow = window;
    LOGI("egl start init");
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("no display");
        initFailed();
        return;
    }
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        LOGE("egl init fail");
        initFailed();
        return;
    }

    EGLint config_attribs[] = {
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };

    int num_configs = 0;
    EGLConfig eglConfig;
    if (!eglChooseConfig(display, config_attribs, &eglConfig, 1, &num_configs)) {
        LOGE("egl choose config fail");
        initFailed();
        return;
    }
    eglSurface = eglCreateWindowSurface(display, eglConfig, window, nullptr);
    if (EGL_NO_SURFACE == eglSurface) {
        LOGE("create eglSurface fail");
        initFailed();
        return;
    }
    if (!eglQuerySurface(display, eglSurface, EGL_WIDTH, &surfaceWidth) ||
        !eglQuerySurface(display, eglSurface, EGL_HEIGHT, &surfaceHeight)) {
        LOGE("eglQuerySurface fail");
        initFailed();
        return;
    }
    LOGI("eglSurface width=%d, height=%d", surfaceWidth, surfaceHeight);
    EGLint context_attrib[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    eglContext = eglCreateContext(display, eglConfig, EGL_NO_CONTEXT, context_attrib);
    if (EGL_NO_CONTEXT == eglContext) {
        LOGE("create egl context fail");
        initFailed();
        return;
    }
    if (!eglMakeCurrent(display, eglSurface, eglSurface, eglContext)) {
        LOGE("eglMakeCurrent fail");
        initFailed();
        return;
    }
    LOGI("egl init success");
    initSuccess();
}

EGLint EglWrapper::swapBuffers() {
    if (!eglSwapBuffers(display, eglSurface)) {
        return eglGetError();
    }
    return EGL_SUCCESS;
}

void EglWrapper::destroy() {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(display, eglSurface);
    eglSurface = nullptr;
    eglDestroyContext(display, eglContext);
    eglContext = nullptr;
    eglTerminate(display);
    display = nullptr;
}
