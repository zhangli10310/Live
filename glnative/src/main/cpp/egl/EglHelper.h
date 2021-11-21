//
// Created by zhangli on 2021/7/19.
//

#ifndef LIVE_EGLHELPER_H
#define LIVE_EGLHELPER_H

#include "EGL/eglplatform.h"
#include "EGL/egl.h"

class EglHelper {
private:
    EGLDisplay eglDisplay;
    EGLConfig eglConfig;
    EGLSurface eglSurface;
    EGLContext eglContext;
    bool initSuccess;

    void destroySurface();

public:
    EGLint surfaceWidth, surfaceHeight;

    void start();

    bool createEglSurface(ANativeWindow* surface);

    EGLint swapBuffers();

    void destroy();

};


#endif //LIVE_EGLHELPER_H
