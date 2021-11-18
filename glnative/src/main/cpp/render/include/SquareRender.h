//
// Created by zl on 2021/11/17.
//

#ifndef LIVE_SQUARERENDER_H
#define LIVE_SQUARERENDER_H

#include "BaseRender.h"

class Square : public BaseRender {

private:

    string gVertexShader =
            "uniform mat4 uMVPMatrix;\n"
            "attribute vec4 vPosition;\n"
            "void main() {\n"
            "  gl_Position = vPosition;\n"
            "}\n";

    string gFragmentShader =
            "precision mediump float;\n"
            "void main() {\n"
            "  gl_FragColor = vec4(0.8, 0.5, 0.2, 1.0);\n"
            "}\n";

    GLint program;
    GLint positionLoc;
    GLint vPMatrixHandle;
protected:
    void onInit() {
        program = createProgram(gVertexShader.c_str(), gFragmentShader.c_str());
        positionLoc = glGetAttribLocation(program, "vPosition");
        vPMatrixHandle = glGetUniformLocation(program, "uMVPMatrix");
    }

    void onSizeChange(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void onDraw() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        checkGlError("glClearColor");

        glUseProgram(program);

        const GLfloat gTriangleVertices[] = {0.0f, 0.5f, -0.5f, -0.5f, 0.0f, -0.5f,
                                             0.5f, -0.5f};
        glEnableVertexAttribArray(positionLoc);
        checkGlError("glEnableVertexAttribArray");
        glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
        checkGlError("glVertexAttribPointer");
        glDrawArrays(GL_TRIANGLES, 0, 4);
        glDisableVertexAttribArray(positionLoc);
        checkGlError("glDrawArrays");
    }

    void onDestroy() {

    }
};

#endif //LIVE_SQUARERENDER_H
