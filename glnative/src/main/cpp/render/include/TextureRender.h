//
// Created by zl on 2021/11/22.
//

#ifndef LIVE_TEXTURERENDER_H
#define LIVE_TEXTURERENDER_H

#include <BaseRender.h>

class TextureRender: public BaseRender {

private:
    GLint program;
    GLint avPositionLoc;
    GLint afPositionLoc;
    GLint textureLoc;

    void renderMediacodec() const;

public:

    void onInit() override;

    void onSizeChange(int width, int height) override;

    void onDraw() override;

    void onDestroy() override;
};

#endif //LIVE_TEXTURERENDER_H
