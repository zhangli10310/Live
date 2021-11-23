//
// Created by zl on 2021/11/22.
//

#include "TextureRender.h"
#include "GLES2/gl2ext.h"

auto gVertexShader =
        "attribute vec4 av_Position;\n"
        "attribute vec2 af_Position;\n"
        "varying vec2 v_texPosition;\n"
        "void main() {\n"
        "    v_texPosition = af_Position;\n"
        "    gl_Position = av_Position;\n"
        "}\n";

auto gFragmentShader =
        "#extension GL_OES_EGL_image_external : require\n"
        "precision mediump float;\n"
        "varying vec2 v_texPosition;\n"
        "uniform samplerExternalOES sTexture;\n"
        "void main() {\n"
        "    gl_FragColor=texture2D(sTexture, v_texPosition);\n"
        "}\n";

GLfloat vertexData[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
};

GLfloat textureData[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
};

void TextureRender::onInit() {
    program = createProgram(gVertexShader, gFragmentShader);
    avPositionLoc = glGetAttribLocation(program, "av_Position");
    afPositionLoc = glGetAttribLocation(program, "af_Position");
    textureLoc = glGetUniformLocation(program, "sTexture");

    glGenTextures(1, textureId);

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


}

void TextureRender::renderMediacodec() const {
//    surfaceTexture.updateTexImage();
    glUseProgram(program);

    glEnableVertexAttribArray(avPositionLoc);
    glVertexAttribPointer(avPositionLoc, 2, GL_FLOAT, false, 8, vertexData);
    glEnableVertexAttribArray(afPositionLoc);
    glVertexAttribPointer(afPositionLoc, 2, GL_FLOAT, false, 8, textureData);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, *textureId);
    glUniform1i(textureLoc, 0);
}

void TextureRender::onSizeChange(int width, int height) {
    glViewport(0, 0, width, height);
}

void TextureRender::onDraw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // renderMediacodec
    renderMediacodec();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void TextureRender::onDestroy() {

}