//
// Created by zhangli on 2021/7/20.
//

#ifndef LIVE_TRIANGLERENDER_H
#define LIVE_TRIANGLERENDER_H

#include "BaseRender.h"

class TriangleRender: public BaseRender {
private:
    GLint program;
    GLint positionLoc;
protected:
    void onInit() final;
    void onSizeChange(int width, int height) final;
    void onDraw() final;
    void onDestroy() final;
};

#endif //LIVE_TRIANGLERENDER_H
