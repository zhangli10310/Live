//
// Created by zl on 2021/11/21.
//

#include "EglHelper.h"

#include "android_log.h"
#include "android/native_window.h"

void EglHelper::start() {
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == EGL_NO_DISPLAY) {
        LOGE("no display");
        return;
    }
    EGLint major, minor;
    if (!eglInitialize(eglDisplay, &major, &minor)) {
        LOGE("egl init fail");
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
    if (!eglChooseConfig(eglDisplay, config_attribs, &eglConfig, 1, &num_configs)) {
        LOGE("egl choose config fail");
        return;
    }

    EGLint context_attrib[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, context_attrib);
    if (eglContext == nullptr || EGL_NO_CONTEXT == eglContext) {
        LOGE("create egl context fail");
        return;
    }
    eglSurface = nullptr;
}

void EglHelper::destroySurface() {
    if (eglSurface != nullptr && eglSurface != EGL_NO_SURFACE) {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(eglDisplay, eglSurface);
        eglSurface = nullptr;
    }
}

bool EglHelper::createEglSurface(ANativeWindow *surface) {
    if (eglDisplay == nullptr) {
        LOGE("create surface error, eglDisplay is null");
        return false;
    }

    if (eglConfig == nullptr) {
        LOGE("create surface error, eglConfig is null");
        return false;
    }

    destroySurface();

    if (surface != nullptr) {
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, surface, nullptr);
    } else {
        eglSurface = nullptr;
    }
    if (eglSurface == nullptr || EGL_NO_SURFACE == eglSurface) {
        LOGW("create eglSurface fail");
        int error = eglGetError();
        if (error == EGL_BAD_NATIVE_WINDOW) {
            LOGW("createEglSurface returned EGL_BAD_NATIVE_WINDOW.");
        } else {
            LOGW("create eglSurface error=%x", error);
        }
        return false;
    }

    if (!eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &surfaceWidth) ||
        !eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &surfaceHeight)) {
        LOGE("eglQuerySurface fail");
        return false;
    }
    LOGI("eglSurface width=%d, height=%d", surfaceWidth, surfaceHeight);

    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        LOGE("eglMakeCurrent fail");
        return false;
    }
    LOGI("egl init success");
    return true;
}

EGLint EglHelper::swapBuffers() {
    if (!eglSwapBuffers(eglDisplay, eglSurface)) {
        return eglGetError();
    }
    return EGL_SUCCESS;
}

void EglHelper::destroy() {
    destroySurface();
    if (eglContext != nullptr) {
        eglDestroyContext(eglDisplay, eglContext);
        eglContext = nullptr;
    }
    if (eglDisplay != nullptr) {
        eglTerminate(eglDisplay);
        eglDisplay = nullptr;
    }
}