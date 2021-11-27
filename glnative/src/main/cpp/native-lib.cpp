#include <jni.h>
#include <TriangleRender.h>
#include <TextureRender.h>
#include "android_log.h"

JavaVM *jvm;
jobject obj;

//extern "C"
//JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
//    JNIEnv *env = nullptr;
//    //获取JNIEnv
//    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
//        return -1;
//    }
//    LOGI("jni onload");
//    jvm = vm;
//
//    //返回jni 的版本
//    return JNI_VERSION_1_6;
//}

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

void textureGenerate(GLuint texName) {
    LOGI("thread id: %d, texture id %d", std::this_thread::get_id(), texName);
    JNIEnv* env;
    jvm->AttachCurrentThread(&env, nullptr);
    jclass clazz = env->GetObjectClass(obj);
    if (clazz == nullptr) {
        LOGE("get java class error");
    }
    jmethodID method = env->GetMethodID(clazz, "callFromNative", "(I)V");
    if (method == nullptr) {
        LOGE("get method id error");
    }
    env->CallVoidMethod(obj, method, (jint)texName);
}

void callback() {
    LOGI("callback form thread id: %d", std::this_thread::get_id());
    JNIEnv* env;
    jvm->AttachCurrentThread(&env, nullptr);
    jclass clazz = env->GetObjectClass(obj);
    if (clazz == nullptr) {
        LOGE("get java class error");
    }
    jmethodID method = env->GetMethodID(clazz, "callFromNativeThread", "()V");
    if (method == nullptr) {
        LOGE("get method id error");
    }
    env->CallVoidMethod(obj, method);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_init(JNIEnv *env, jobject thiz) {

    env->GetJavaVM(&jvm);
    obj = env->NewGlobalRef(thiz);
    LOGI("jni init,thread id:%d", std::this_thread::get_id());
    auto *render = new TextureRender();
    render->setRenderWhenDirty(true);
//    auto *render = new TriangleRender();
    LOGI("set render %ld", (long) render);
    setRender(env, thiz, render);

    render->textureGenerate = textureGenerate;
    render->callback = callback;
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

extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_draw(JNIEnv *env, jobject thiz) {
    auto render = getRender(env, thiz);
    if (render != nullptr) {
        LOGD("jni draw");
        render->draw();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_callNativeThread(JNIEnv *env, jobject thiz) {
    auto render = getRender(env, thiz);
    if (render != nullptr) {
        LOGD("jni callNativeThread");
        render->callFromNativeThread();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zl_glnative_GLRenderer_setTextureSize(JNIEnv *env, jobject thiz, jint width, jint height) {
    auto render = getRender(env, thiz);
    if (render != nullptr) {
        LOGD("jni setTextureSize");
        render->setTextureSize(width, height);
    }
}