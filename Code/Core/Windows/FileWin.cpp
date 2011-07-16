/*
   GameRiff - Framework for creating various video game services
   Windows implementation of files
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
// Internal implementation
//

class RawFile : public IRawFile {
public:
    RawFile();

    EFileResult Open(const chargr * filename);

    virtual void Close();
    virtual EFileResult Flush();
    virtual EFileResult Write(const chargr * string, unsigned len);
    virtual EFileResult Write(const byte * buffer, unsigned len);

private:

    EFileResult ConvertSysError();

private:
    FILE * m_file;
};

//====================================================
RawFile::RawFile() :
    m_file(NULL)
{
}

//====================================================
void RawFile::Close() {
    if (m_file != NULL) 
        fclose(m_file);
    m_file = NULL;
}

//====================================================
EFileResult RawFile::ConvertSysError() {
    return FILE_RESULT_DOESNT_EXIST;
}

//====================================================
EFileResult RawFile::Flush() {
    EFileResult result = FILE_RESULT_OK;
    int fileResult = fflush(m_file);
    if (fileResult != 0) 
        result = ConvertSysError();
    return result;
}

//====================================================
EFileResult RawFile::Open(const chargr * filename) {
    EFileResult result = FILE_RESULT_OK;

    charsys utf8Filename[256];
    StrConvertToUtf8(filename, utf8Filename, 256);
    fopen_s(&m_file, utf8Filename, "w");
    if (m_file == NULL) 
        result = ConvertSysError();

    return result;
}

//====================================================
EFileResult RawFile::Write(const chargr * string, unsigned len) {
    EFileResult result = FILE_RESULT_DOESNT_EXIST;
    if (m_file != NULL) {
        size_t bytesToWrite = len * sizeof(chargr);
        size_t bytesWritten = fwrite(string, 1, bytesToWrite, m_file);
        if (bytesWritten != bytesToWrite) 
            result = ConvertSysError();
        else
            result = FILE_RESULT_OK;
    }
    return result;
}

//====================================================
EFileResult RawFile::Write(const byte * buffer, unsigned len) {
    EFileResult result = FILE_RESULT_DOESNT_EXIST;
    if (m_file != NULL) {
        size_t bytesToWrite = len;
        size_t bytesWritten = fwrite(buffer, 1, bytesToWrite, m_file);
        if (bytesWritten != bytesToWrite) 
            result = ConvertSysError();
        else
            result = FILE_RESULT_OK;
    }
    return result;
}

//////////////////////////////////////////////////////
//
// External Functions
//

IRawFilePtr FileOpenRaw(const chargr * filename, EFileResult * result) {

    RawFile * file = new(MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_FILEIO)) RawFile;

    *result = file->Open(filename);
    if (*result != FILE_RESULT_OK) {
        delete file;
        file = NULL;
    }

    return IRawFilePtr(file);
}

