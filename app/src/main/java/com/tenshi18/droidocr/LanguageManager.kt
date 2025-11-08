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

import android.content.Context
import android.content.SharedPreferences
import androidx.core.content.edit

class LanguageManager(private val context: Context) {
    
    private val prefs: SharedPreferences = context.getSharedPreferences(
        PREFS_NAME, 
        Context.MODE_PRIVATE
    )
    
    fun getCurrentLanguage(): OcrLanguage {
        val languageName = prefs.getString(KEY_LANGUAGE, null)
        return OcrLanguage.fromString(languageName)
    }
    
    fun setLanguage(language: OcrLanguage) {
        prefs.edit {
            putString(KEY_LANGUAGE, language.name)
        }
    }
    
    fun getCurrentLanguageConfig(): LanguageConfig {
        return getCurrentLanguage().config
    }
    
    companion object {
        private const val PREFS_NAME = "droidocr_prefs"
        private const val KEY_LANGUAGE = "selected_language"
    }
}

