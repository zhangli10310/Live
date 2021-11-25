//
// Created by zl on 2021/11/22.
//

#include "TextureRender.h"
#include "GLES2/gl2ext.h"

auto textureVertexShader =
        "attribute vec4 av_Position;\n"
        "attribute vec2 af_Position;\n"
        "varying vec2 v_texPosition;\n"
        "void main() {\n"
        "    v_texPosition = af_Position;\n"
        "    gl_Position = av_Position;\n"
        "}\n";

auto textureFragmentShader =
        "#extension GL_OES_EGL_image_external : require\n"
        "precision mediump float;\n"
        "varying vec2 v_texPosition;\n"
        "uniform samplerExternalOES sTexture;\n"
        "void main() {\n"
        "    vec4 rgb = texture2D(sTexture, v_texPosition);"
        "    gl_FragColor = vec4(rgb.r, rgb.g, rgb.b, 0.3);\n"
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
    program = createProgram(textureVertexShader, textureFragmentShader);
    avPositionLoc = glGetAttribLocation(program, "av_Position");
    afPositionLoc = glGetAttribLocation(program, "af_Position");
    textureLoc = glGetUniformLocation(program, "sTexture");

    LOGI("before GenTextures, textureId: %d", textureId);
    glGenTextures(1, &textureId);

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textureGenerate(textureId);
}

void TextureRender::renderMediacodec() const {
//    surfaceTexture.updateTexImage();
    glUseProgram(program);

    glEnableVertexAttribArray(avPositionLoc);

    GLfloat *vData = vertexData;
    if (textureHeight > 0 && textureWidth > 0) {
        if ((float) textureHeight / (float) currentHeight >
            (float) textureWidth * 1.0f / (float) currentWidth) { // 按高适配
            float width = (float) textureWidth * (float) currentHeight / (float) textureHeight / (float) currentWidth;
            vData = new GLfloat[]{
                    -width, -1.0f,
                    width, -1.0f,
                    -width, 1.0f,
                    width, 1.0f
            };
        } else {
            float height = (float) textureHeight * (float) currentWidth / (float) textureWidth /
                           (float) currentHeight;
            vData = new GLfloat[]{
                    -1.0f, -height,
                    1.0f, -height,
                    -1.0f, height,
                    1.0f, height
            };
        }
    }

    glVertexAttribPointer(avPositionLoc, 2, GL_FLOAT, false, 8, vData);
    glEnableVertexAttribArray(afPositionLoc);
    glVertexAttribPointer(afPositionLoc, 2, GL_FLOAT, false, 8, textureData);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId);
    glUniform1i(textureLoc, 0);

    glEnable(GL_BLEND);
    // SRC的颜色已经在片段着色器中乘过一次MASK的透明度，故srcAlpha为GL_ONE
    glBlendFuncSeparate(
            GL_SRC_ALPHA,
            GL_ONE_MINUS_SRC_ALPHA,
            GL_ONE,
            GL_ONE_MINUS_SRC_ALPHA
    );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_BLEND);
}

void TextureRender::onSizeChange(int width, int height) {
    glViewport(0, 0, width, height);
}

void TextureRender::onDraw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // renderMediacodec
    renderMediacodec();
}

void TextureRender::onDestroy() {

}