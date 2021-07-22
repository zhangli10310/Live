//
// Created by zhangli on 2021/7/20.
//

#ifndef LIVE_BASERENDER_H
#define LIVE_BASERENDER_H

#include "GLES2/gl2.h"
#include "EglWrapper.h"
#include <thread>
#include "BlockQueue.h"


class BaseRender {

private:
    EglWrapper *eglWrapper;

    BlockQueue<std::function<void()>> *queue;
    std::atomic<bool> threadRun;
    std::atomic<bool> threadExit;

    void run();

protected:
    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    GLuint loadShader(GLenum shaderType, const char *pSource);

    GLuint createProgram(const char *pVertexSource, const char *pFragmentSource);

    virtual void onInit() = 0;

    virtual void onSizeChange(int width, int height) = 0;

    virtual void onDraw() = 0;

    virtual void onDestroy() = 0;

public:
    BaseRender();

    void init(ANativeWindow *window);

    void reset(int width, int height);

    void draw();

    void destroy();

    ~BaseRender();
};

#endif //LIVE_BASERENDER_H
