//
// Created by zhangli on 2021/7/20.
//

#include <unistd.h>
#include "TriangleRender.h"
#include "android_log.h"

auto gVertexShader =
        "attribute vec4 vPosition;\n"
        "void main() {\n"
        "  gl_Position = vPosition;\n"
        "}\n";

auto gFragmentShader =
        "precision mediump float;\n"
        "void main() {\n"
        "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "}\n";

void TriangleRender::onInit() {
    program = createProgram(gVertexShader, gFragmentShader);
    positionLoc = glGetAttribLocation(program, "vPosition");
}

void TriangleRender::onSizeChange(int width, int height) {
    glViewport(0, 0, width, height);
}

void TriangleRender::onDraw() {
    LOGD("onDraw");
    static float grey;
    usleep(200000);
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(program);
    checkGlError("glUseProgram");

    const GLfloat gTriangleVertices[] = {0.0f, 0.5f, -0.5f, -0.5f,
                                         0.5f, -0.5f};
    glEnableVertexAttribArray(positionLoc);
    checkGlError("glEnableVertexAttribArray");
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    checkGlError("glVertexAttribPointer");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(positionLoc);
    checkGlError("glDrawArrays");
}

void TriangleRender::onDestroy() {
    LOGD("onDestroy");
}