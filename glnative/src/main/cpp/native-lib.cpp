#include <jni.h>
#include "TriangleRender.h"
#include "android/native_window_jni.h"
#include "android_log.h"

TriangleRender *getRender(JNIEnv *env, jobject thiz) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fid = env->GetFieldID(clazz, "renderPtr", "J");
    jlong p = env->GetLongField(thiz, fid);
    return (TriangleRender *) p;
}

void setRender(JNIEnv *env, jobject thiz, const TriangleRender *render) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fid = env->GetFieldID(clazz, "renderPtr", "J");
    env->SetLongField(thiz, fid, (jlong) render);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_init(JNIEnv *env, jobject thiz, jobject surface) {
    LOGD("jni init");
    ANativeWindow *nwin = ANativeWindow_fromSurface(env, surface);
    auto *render = new TriangleRender();
    LOGI("set render %ld", render);
    setRender(env, thiz, render);
    render->init(nwin);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_reset(JNIEnv *env, jobject thiz, jint width, jint height) {
    LOGD("jni reset");
    TriangleRender* render = getRender(env, thiz);
    LOGI("get render %d", render);
    LOGI("get render reset:%d,%d", width, height);
    render->reset(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_stop(JNIEnv *env, jobject thiz) {
    LOGD("jni stop");
    TriangleRender* render = getRender(env, thiz);
    render->destroy();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_finalize(JNIEnv *env, jobject thiz) {
    TriangleRender* render = getRender(env, thiz);
    if (render != nullptr) {
        delete render;
        render = nullptr;
        setRender(env, thiz, render);
    }
}
