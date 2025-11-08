// Copyright 2025 DroidOCR Contributors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <jni.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ppocrv5_full.h"

#define TAG "DroidOCR_JNI"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

static PPOCRv5* g_ppocrv5 = nullptr;

static std::vector<std::string> load_dict_from_asset(AAssetManager* mgr, const char* filename) {
    std::vector<std::string> dict;
    
    AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("Failed to open asset: %s", filename);
        return dict;
    }
    
    size_t size = AAsset_getLength(asset);
    char* buffer = new char[size + 1];
    AAsset_read(asset, buffer, size);
    buffer[size] = '\0';
    AAsset_close(asset);
    
    std::istringstream iss(buffer);
    std::string line;
    while (std::getline(iss, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (!line.empty()) {
            dict.push_back(line);
        }
    }
    
    delete[] buffer;
    
    return dict;
}

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
    if (g_ppocrv5 != nullptr) {
        delete g_ppocrv5;
        g_ppocrv5 = nullptr;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_tenshi18_droidocr_PPOCRv5Rec_loadModel(
    JNIEnv* env,
    jobject thiz,
    jobject asset_manager,
    jstring det_param_path,
    jstring det_bin_path,
    jstring rec_param_path,
    jstring rec_bin_path,
    jstring dict_path,
    jboolean use_gpu
) {
    if (g_ppocrv5 == nullptr) {
        g_ppocrv5 = new PPOCRv5();
    }
    
    AAssetManager* mgr = AAssetManager_fromJava(env, asset_manager);
    if (!mgr) {
        LOGE("Failed to get AssetManager");
        return JNI_FALSE;
    }
    
    const char* det_param_str = env->GetStringUTFChars(det_param_path, nullptr);
    const char* det_bin_str = env->GetStringUTFChars(det_bin_path, nullptr);
    const char* rec_param_str = env->GetStringUTFChars(rec_param_path, nullptr);
    const char* rec_bin_str = env->GetStringUTFChars(rec_bin_path, nullptr);
    const char* dict_path_str = env->GetStringUTFChars(dict_path, nullptr);
    
    std::vector<std::string> dict = load_dict_from_asset(mgr, dict_path_str);
    if (dict.empty()) {
        LOGE("Failed to load dictionary");
        env->ReleaseStringUTFChars(det_param_path, det_param_str);
        env->ReleaseStringUTFChars(det_bin_path, det_bin_str);
        env->ReleaseStringUTFChars(rec_param_path, rec_param_str);
        env->ReleaseStringUTFChars(rec_bin_path, rec_bin_str);
        env->ReleaseStringUTFChars(dict_path, dict_path_str);
        return JNI_FALSE;
    }
    
    int ret = g_ppocrv5->load(
        mgr,
        det_param_str,
        det_bin_str,
        rec_param_str,
        rec_bin_str,
        true,
        use_gpu
    );
    
    g_ppocrv5->set_dictionary(dict);
    
    env->ReleaseStringUTFChars(det_param_path, det_param_str);
    env->ReleaseStringUTFChars(det_bin_path, det_bin_str);
    env->ReleaseStringUTFChars(rec_param_path, rec_param_str);
    env->ReleaseStringUTFChars(rec_bin_path, rec_bin_str);
    env->ReleaseStringUTFChars(dict_path, dict_path_str);
    
    if (ret != 0) {
        LOGE("Failed to load models");
        return JNI_FALSE;
    }
    
    g_ppocrv5->set_target_size(1024);
    
    return JNI_TRUE;
}

JNIEXPORT jstring JNICALL
Java_com_tenshi18_droidocr_PPOCRv5Rec_detectAndRecognize(
    JNIEnv* env,
    jobject thiz,
    jobject bitmap
) {
    if (g_ppocrv5 == nullptr) {
        LOGE("Model not loaded");
        return env->NewStringUTF("");
    }
    
    AndroidBitmapInfo info;
    int ret = AndroidBitmap_getInfo(env, bitmap, &info);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGE("AndroidBitmap_getInfo failed: %d", ret);
        return env->NewStringUTF("");
    }
    
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format not RGBA_8888");
        return env->NewStringUTF("");
    }
    
    void* pixels;
    ret = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGE("AndroidBitmap_lockPixels failed: %d", ret);
        return env->NewStringUTF("");
    }
    
    cv::Mat rgba(info.height, info.width, CV_8UC4, pixels);
    cv::Mat rgb;
    cv::cvtColor(rgba, rgb, cv::COLOR_RGBA2RGB);
    
    std::vector<Object> objects;
    g_ppocrv5->detect_and_recognize(rgb, objects);
    
    AndroidBitmap_unlockPixels(env, bitmap);
    
    std::string result;
    for (size_t i = 0; i < objects.size(); i++) {
        const Object& obj = objects[i];
        
        std::string line;
        for (size_t j = 0; j < obj.text.size(); j++) {
            const Character& ch = obj.text[j];
            const std::string& char_str = g_ppocrv5->get_char(ch.id);
            
            if (char_str.empty()) {
                if (!line.empty() && line.back() != ' ') {
                    line += " ";
                }
                continue;
            }
            
            line += char_str;
        }
        
        if (!line.empty()) {
            result += line;
            if (i + 1 < objects.size()) {
                result += "\n";
            }
        }
    }
    
    return env->NewStringUTF(result.c_str());
}

JNIEXPORT jobjectArray JNICALL
Java_com_tenshi18_droidocr_PPOCRv5Rec_detectAndRecognizeWithBoxes(
    JNIEnv* env,
    jobject thiz,
    jobject bitmap
) {
    if (g_ppocrv5 == nullptr) {
        LOGE("Model not loaded");
        return env->NewObjectArray(0, env->FindClass("com/tenshi18/droidocr/TextRegion"), nullptr);
    }
    
    AndroidBitmapInfo info;
    int ret = AndroidBitmap_getInfo(env, bitmap, &info);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGE("AndroidBitmap_getInfo failed: %d", ret);
        return env->NewObjectArray(0, env->FindClass("com/tenshi18/droidocr/TextRegion"), nullptr);
    }
    
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format not RGBA_8888");
        return env->NewObjectArray(0, env->FindClass("com/tenshi18/droidocr/TextRegion"), nullptr);
    }
    
    void* pixels;
    ret = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGE("AndroidBitmap_lockPixels failed: %d", ret);
        return env->NewObjectArray(0, env->FindClass("com/tenshi18/droidocr/TextRegion"), nullptr);
    }
    
    cv::Mat rgba(info.height, info.width, CV_8UC4, pixels);
    cv::Mat rgb;
    cv::cvtColor(rgba, rgb, cv::COLOR_RGBA2RGB);
    
    std::vector<Object> objects;
    g_ppocrv5->detect_and_recognize(rgb, objects);
    
    AndroidBitmap_unlockPixels(env, bitmap);
    
    jclass textRegionClass = env->FindClass("com/tenshi18/droidocr/TextRegion");
    jclass pointFClass = env->FindClass("android/graphics/PointF");
    
    jmethodID textRegionConstructor = env->GetMethodID(
        textRegionClass, "<init>", 
        "(Ljava/lang/String;[Landroid/graphics/PointF;F)V"
    );
    
    jmethodID pointFConstructor = env->GetMethodID(pointFClass, "<init>", "(FF)V");
    
    struct TextRegionData {
        std::string text;
        Object obj;
        float center_y;
        float center_x;
    };
    
    std::vector<TextRegionData> regions_data;
    
    for (size_t i = 0; i < objects.size(); i++) {
        const Object& obj = objects[i];
        
        std::string text;
        for (size_t j = 0; j < obj.text.size(); j++) {
            const Character& ch = obj.text[j];
            const std::string& char_str = g_ppocrv5->get_char(ch.id);
            
            if (char_str.empty()) {
                if (!text.empty() && text.back() != ' ') {
                    text += " ";
                }
                continue;
            }
            
            text += char_str;
        }
        
        TextRegionData data;
        data.text = text;
        data.obj = obj;
        data.center_y = obj.rrect.center.y;
        data.center_x = obj.rrect.center.x;
        regions_data.push_back(data);
    }
    
    float avg_height = 0.0f;
    for (const auto& data : regions_data) {
        avg_height += data.obj.rrect.size.height;
    }
    if (!regions_data.empty()) {
        avg_height /= regions_data.size();
    }
    const float line_threshold = avg_height * 0.6f;
    
    std::sort(regions_data.begin(), regions_data.end(), 
        [](const TextRegionData& a, const TextRegionData& b) {
            return a.center_y < b.center_y;
        });
    
    std::vector<std::vector<size_t>> lines;
    for (size_t i = 0; i < regions_data.size(); i++) {
        bool found_line = false;
        for (auto& line : lines) {
            float line_y = regions_data[line[0]].center_y;
            if (std::abs(regions_data[i].center_y - line_y) < line_threshold) {
                line.push_back(i);
                found_line = true;
                break;
            }
        }
        if (!found_line) {
            lines.push_back({i});
        }
    }
    
    std::vector<TextRegionData> sorted_regions;
    for (auto& line : lines) {
        std::sort(line.begin(), line.end(),
            [&regions_data](size_t a, size_t b) {
                return regions_data[a].center_x < regions_data[b].center_x;
            });
        
        std::vector<bool> merged(line.size(), false);
        std::vector<TextRegionData> line_regions;
        
        for (size_t i = 0; i < line.size(); i++) {
            if (merged[i]) continue;
            
            size_t idx = line[i];
            bool is_single_dot = (regions_data[idx].text.length() == 1 && regions_data[idx].text[0] == '.');
            
            if (is_single_dot && i > 0 && !merged[i-1]) {
                size_t prev_idx = line[i-1];
                bool prev_is_dot = (regions_data[prev_idx].text.length() == 1 && regions_data[prev_idx].text[0] == '.');
                
                if (!prev_is_dot) {
                    float x1 = regions_data[prev_idx].center_x + regions_data[prev_idx].obj.rrect.size.width / 2;
                    float x2 = regions_data[idx].center_x;
                    
                    if (x2 > x1) {
                        line_regions.back().text += ".";
                        merged[i] = true;
                        continue;
                    }
                }
            }
            
            line_regions.push_back(regions_data[idx]);
        }
        
        for (const auto& region : line_regions) {
            sorted_regions.push_back(region);
        }
    }
    
    regions_data = sorted_regions;
    
    jobjectArray resultArray = env->NewObjectArray(regions_data.size(), textRegionClass, nullptr);
    
    for (size_t i = 0; i < regions_data.size(); i++) {
        const TextRegionData& data = regions_data[i];
        const Object& obj = data.obj;
        std::string text = data.text;
        
        cv::Point2f corners[4];
        obj.rrect.points(corners);
        
        jobjectArray cornersArray = env->NewObjectArray(4, pointFClass, nullptr);
        for (int j = 0; j < 4; j++) {
            jobject point = env->NewObject(pointFClass, pointFConstructor, 
                                          corners[j].x, corners[j].y);
            env->SetObjectArrayElement(cornersArray, j, point);
            env->DeleteLocalRef(point);
        }
        
        jstring jtext = env->NewStringUTF(text.c_str());
        jobject textRegion = env->NewObject(textRegionClass, textRegionConstructor,
                                           jtext, cornersArray, obj.prob);
        
        env->SetObjectArrayElement(resultArray, i, textRegion);
        
        env->DeleteLocalRef(jtext);
        env->DeleteLocalRef(cornersArray);
        env->DeleteLocalRef(textRegion);
    }
    
    env->DeleteLocalRef(textRegionClass);
    env->DeleteLocalRef(pointFClass);
    
    return resultArray;
}

JNIEXPORT void JNICALL
Java_com_tenshi18_droidocr_PPOCRv5Rec_release(
    JNIEnv* env,
    jobject thiz
) {
    if (g_ppocrv5 != nullptr) {
        delete g_ppocrv5;
        g_ppocrv5 = nullptr;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_tenshi18_droidocr_PPOCRv5Rec_switchLanguage(
    JNIEnv* env,
    jobject thiz,
    jobject asset_manager,
    jstring det_param_path,
    jstring det_bin_path,
    jstring rec_param_path,
    jstring rec_bin_path,
    jstring dict_path,
    jboolean use_gpu
) {
    if (g_ppocrv5 != nullptr) {
        delete g_ppocrv5;
        g_ppocrv5 = nullptr;
    }
    
    g_ppocrv5 = new PPOCRv5();
    
    AAssetManager* mgr = AAssetManager_fromJava(env, asset_manager);
    if (!mgr) {
        LOGE("Failed to get AssetManager");
        return JNI_FALSE;
    }
    
    const char* det_param_str = env->GetStringUTFChars(det_param_path, nullptr);
    const char* det_bin_str = env->GetStringUTFChars(det_bin_path, nullptr);
    const char* rec_param_str = env->GetStringUTFChars(rec_param_path, nullptr);
    const char* rec_bin_str = env->GetStringUTFChars(rec_bin_path, nullptr);
    const char* dict_path_str = env->GetStringUTFChars(dict_path, nullptr);
    
    std::vector<std::string> dict = load_dict_from_asset(mgr, dict_path_str);
    if (dict.empty()) {
        LOGE("Failed to load dictionary");
        env->ReleaseStringUTFChars(det_param_path, det_param_str);
        env->ReleaseStringUTFChars(det_bin_path, det_bin_str);
        env->ReleaseStringUTFChars(rec_param_path, rec_param_str);
        env->ReleaseStringUTFChars(rec_bin_path, rec_bin_str);
        env->ReleaseStringUTFChars(dict_path, dict_path_str);
        return JNI_FALSE;
    }
    
    int ret = g_ppocrv5->load(
        mgr,
        det_param_str,
        det_bin_str,
        rec_param_str,
        rec_bin_str,
        true,
        use_gpu
    );
    
    g_ppocrv5->set_dictionary(dict);
    
    env->ReleaseStringUTFChars(det_param_path, det_param_str);
    env->ReleaseStringUTFChars(det_bin_path, det_bin_str);
    env->ReleaseStringUTFChars(rec_param_path, rec_param_str);
    env->ReleaseStringUTFChars(rec_bin_path, rec_bin_str);
    env->ReleaseStringUTFChars(dict_path, dict_path_str);
    
    if (ret != 0) {
        LOGE("Failed to load models");
        return JNI_FALSE;
    }
    
    g_ppocrv5->set_target_size(1024);
    
    return JNI_TRUE;
}

} // extern "C"

