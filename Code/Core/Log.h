/*
   GameRiff - Framework for creating various video game services
   Interface for logging
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

enum ELogPriority {
    LOG_PRIORITY_INFO,
    LOG_PRIORITY_WARN,
    LOG_PRIORITY_ERROR
};

void LogInit();
void LogFlushAndCloseAll();
void LogClose();

class LogModuleId {
public:

private:
    const char    * m_idName;
    uint64          m_id;
};

#define LOG_DECLARE_MODULE(module)                                          \
    
#define LOG_DEFINE_MODULE(module)                                           \
    static chargr * s_logModuleId = TOWSTR(module)


#define LOG(pri, format, ...)                                               \
    Log(pri, s_logModuleId, __FILE__, __LINE__, format, __VA_ARGS__)                          

#define LOGF(pri, file, line, format, ...)                                  \
    Log(pri, s_logModuleId, file, line, format, __VA_ARGS__)                          

#define LOGV(pri, file, line, format, vargs)                                \
    LogV(pri, s_logModuleId, file, line, format, vargs)                          

void Log(ELogPriority pri, const chargr  * moduleID, const chargr  * file, unsigned lineNum, const chargr  * format, ...);
void Log(ELogPriority pri, const chargr  * moduleID, const chargr  * file, unsigned lineNum, const charsys * format, ...);
void Log(ELogPriority pri, const chargr  * moduleID, const charsys * file, unsigned lineNum, const chargr  * format, ...);
void Log(ELogPriority pri, const chargr  * moduleID, const charsys * file, unsigned lineNum, const charsys * format, ...);
void LogV(ELogPriority pri, const chargr  * moduleID, const chargr  * file, unsigned lineNum, const chargr  * format, va_list vargs);
void LogV(ELogPriority pri, const chargr  * moduleID, const chargr  * file, unsigned lineNum, const charsys * format, va_list vargs);
void LogV(ELogPriority pri, const chargr  * moduleID, const charsys * file, unsigned lineNum, const chargr  * format, va_list vargs);
void LogV(ELogPriority pri, const chargr  * moduleID, const charsys * file, unsigned lineNum, const charsys * format, va_list vargs);


