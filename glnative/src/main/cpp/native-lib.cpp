#include <jni.h>
#include <TriangleRender.h>
#include "android_log.h"

BaseRender *getRender(JNIEnv *env, jobject thiz) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fid = env->GetFieldID(clazz, "renderPtr", "J");
    jlong p = env->GetLongField(thiz, fid);
    return (BaseRender *) p;
}

void setRender(JNIEnv *env, jobject thiz, const BaseRender *render) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fid = env->GetFieldID(clazz, "renderPtr", "J");
    env->SetLongField(thiz, fid, (jlong) render);
}

void destroyRender(JNIEnv *env, jobject thiz) {
    auto render = getRender(env, thiz);
    if (render != nullptr) {
        delete render;
        render = nullptr;
        setRender(env, thiz, render);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_init(JNIEnv *env, jobject thiz) {
    LOGD("jni init");
    auto *render = new TriangleRender();
    LOGI("set render %ld", (long)render);
    setRender(env, thiz, render);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_changeSurface(JNIEnv *env, jobject thiz, jobject surface) {
    LOGD("jni changeSurface");
    auto render = getRender(env, thiz);
    render->changeSurface(env, surface);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_resize(JNIEnv *env, jobject thiz, jint width, jint height) {
    LOGD("jni reset");
    auto render = getRender(env, thiz);
    LOGI("get render %ld", render);
    LOGI("get render reset:%d,%d", width, height);
    render->reset(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_stop(JNIEnv *env, jobject thiz) {
    auto render = getRender(env, thiz);
    if (render != nullptr) {
        LOGD("jni stop");
        render->changeSurface(env, nullptr);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_finalize(JNIEnv *env, jobject thiz) {
    LOGI("finalize");
    destroyRender(env, thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_destroy(JNIEnv *env, jobject thiz) {
    LOGI("destroy");
    destroyRender(env, thiz);
}
