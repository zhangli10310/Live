#include <jni.h>
#include "TriangleRender.h"
#include "android/native_window_jni.h"
#include "android_log.h"

TriangleRender *render;

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_init(JNIEnv *env, jobject thiz, jobject surface) {
    ANativeWindow *nwin = ANativeWindow_fromSurface(env, surface);
    render = new TriangleRender();
    render->init(nwin);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_reset(JNIEnv *env, jobject thiz, jint width, jint height) {
    render->reset(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_stop(JNIEnv *env, jobject thiz) {
    render->destroy();
}
