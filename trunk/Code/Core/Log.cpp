/*
   GameRiff - Framework for creating various video game services
   Implementation of logging services
   Copyright (C) 2011, Shaun Leach.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
  
       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.
  
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Pch.h"

//////////////////////////////////////////////////////
//
// Internal constants
//

static chargr * s_logPriorities[] = {
    L"Info",
    L"Warning",
    L"Error"
};

static const unsigned s_formatLength = 1024;
static chargr s_localFormat[s_formatLength];

static const unsigned s_bufferLength = 2048;
static chargr s_stringBuffer[s_bufferLength];

static IRawFilePtr s_errorLog    = NULL;
static IRawFilePtr s_warnLog     = NULL;
static IRawFilePtr s_infoLog     = NULL;

static byte s_utf16BOM[2] = {
    0xFF,
    0xFE
};

//////////////////////////////////////////////////////
//
// Internal Functions
//

void LogInternal(ELogPriority pri, const chargr * format, va_list args) {
    StrPrintfV(s_stringBuffer, s_bufferLength, s_localFormat, args);

    if (pri == LOG_PRIORITY_ERROR) {
        wprintf(s_stringBuffer);
        s_errorLog->Write(s_stringBuffer, StrLen(s_stringBuffer, s_bufferLength));
    }
    else if (pri == LOG_PRIORITY_WARN) {
        s_warnLog->Write(s_stringBuffer, StrLen(s_stringBuffer, s_bufferLength));
    }
    else if (pri == LOG_PRIORITY_INFO) {
        s_infoLog->Write(s_stringBuffer, StrLen(s_stringBuffer, s_bufferLength));
    }
}

//////////////////////////////////////////////////////
//
// External Functions
//

//====================================================
void LogInit() {
    EFileResult result;
    s_errorLog = FileOpenRaw(L"errors.log", &result);
    ASSERTMSGGR(s_errorLog != NULL, "Failed to open log file: %s", L"errors.log");
    s_errorLog->Write(s_utf16BOM, sizeof(s_utf16BOM));

    s_warnLog = FileOpenRaw(L"warnings.log", &result);
    ASSERTMSGGR(s_warnLog != NULL, "Failed to open log file: %s", L"warnings.log");
    s_errorLog->Write(s_utf16BOM, sizeof(s_utf16BOM));

    s_infoLog = FileOpenRaw(L"info.log", &result);
    ASSERTMSGGR(s_infoLog != NULL, "Failed to open log file: %s", L"info.log");
    s_errorLog->Write(s_utf16BOM, sizeof(s_utf16BOM));
}

//====================================================
void LogClose() {
    LogFlushAndCloseAll();
}

//====================================================
void LogFlushAndCloseAll() {
    s_errorLog->Flush();
    s_errorLog->Close();
    s_errorLog = NULL;

    s_warnLog->Flush();
    s_warnLog->Close();
    s_warnLog = NULL;

    s_infoLog->Flush();
    s_infoLog->Close();
    s_infoLog = NULL;
}

//====================================================
void Log(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const charsys * file, 
    unsigned        lineNum, 
    const chargr  * format, 
    ...
) {
    va_list args;
    va_start(args, format);
    LogV(pri, moduleID, file, lineNum, format, args);
}

//====================================================
void Log(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const charsys * file, 
    unsigned        lineNum, 
    const charsys * format, 
    ...
) {
    va_list args;
    va_start(args, format);
    LogV(pri, moduleID, file, lineNum, format, args);
}

//====================================================
void Log(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const chargr  * file, 
    unsigned        lineNum, 
    const chargr  * format, 
    ...
) {
    va_list args;
    va_start(args, format);
    LogV(pri, moduleID, file, lineNum, format, args);
}

//====================================================
void Log(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const chargr  * file, 
    unsigned        lineNum, 
    const charsys * format, 
    ...
) {
    va_list args;
    va_start(args, format);
    LogV(pri, moduleID, file, lineNum, format, args);
}

//====================================================
void LogV(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const chargr  * file, 
    unsigned        lineNum, 
    const chargr  * format, 
    va_list         vargs
) {
    StrPrintf(s_localFormat, s_formatLength, L">:<%s>:<%s>:<%s>:<%d>:<%s\n", s_logPriorities[pri], moduleID, file, lineNum, format);
    LogInternal(pri, s_localFormat, vargs);
}

//====================================================
void LogV(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const chargr  * file, 
    unsigned        lineNum, 
    const charsys * format, 
    va_list         vargs
) {
    StrPrintf(s_localFormat, s_formatLength, L">:<%s>:<%s>:<%s>:<%d>:<%S\n", s_logPriorities[pri], moduleID, file, lineNum, format);
    LogInternal(pri, s_localFormat, vargs);
}

//====================================================
void LogV(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const charsys * file, 
    unsigned        lineNum, 
    const chargr  * format, 
    va_list         vargs
) {
    StrPrintf(s_localFormat, s_formatLength, L">:<%s>:<%s>:<%S>:<%d>:<%s\n", s_logPriorities[pri], moduleID, file, lineNum, format);
    LogInternal(pri, s_localFormat, vargs);
}

//====================================================
void LogV(
    ELogPriority    pri, 
    const chargr  * moduleID, 
    const charsys * file, 
    unsigned        lineNum, 
    const charsys * format, 
    va_list         vargs
) {
    StrPrintf(s_localFormat, s_formatLength, L">:<%s>:<%s>:<%S>:<%d>:<%S\n", s_logPriorities[pri], moduleID, file, lineNum, format);
    LogInternal(pri, s_localFormat, vargs);
}

