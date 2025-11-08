/*
 * Copyright 2025 DroidOCR Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tenshi18.droidocr

/**
 * Configuration for OCR language models
 */
data class LanguageConfig(
    val recParamPath: String,
    val recBinPath: String,
    val dictPath: String
) {
    companion object {
        val SLAVIC = LanguageConfig(
            recParamPath = "eslav_ppocrv5_rec.ncnn.param",
            recBinPath = "eslav_ppocrv5_rec.ncnn.bin",
            dictPath = "ppocrv5_eslav_dict.txt"
        )
    }
}

/**
 * Supported OCR languages
 */
enum class OcrLanguage(
    val displayName: String,
    val config: LanguageConfig
) {
    SLAVIC("Slavic (RU/BY/UA)", LanguageConfig.SLAVIC);

    companion object {
        fun fromString(value: String?): OcrLanguage {
            return try {
                valueOf(value ?: "SLAVIC")
            } catch (e: IllegalArgumentException) {
                SLAVIC
            }
        }
    }
}

