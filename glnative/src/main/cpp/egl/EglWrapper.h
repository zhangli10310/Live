//
// Created by zhangli on 2021/7/19.
//

#ifndef LIVE_EGLWRAPPER_H
#define LIVE_EGLWRAPPER_H

#include "EGL/eglplatform.h"
#include "android/native_window.h"

#include "EGL/egl.h"

class EglWrapper {
private:
    ANativeWindow* nativeWindow;
    EGLDisplay display;
    EGLSurface eglSurface;
    EGLContext eglContext;
    bool hasInit = false;

    void initFailed() {
        hasInit = false;
    }

    void initSuccess() {
        hasInit = true;
    }
public:
    EGLint surfaceWidth, surfaceHeight;
    void init(ANativeWindow* window);
    void swapBuffers();
    void destroy();

    bool isInit() {
        return hasInit;
    }
};


#endif //LIVE_EGLWRAPPER_H
