/*
   GameRiff - Framework for creating various video game services
   Interface to stream i/o service
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

enum EStreamError {
    STREAM_ERROR_OK,
    STREAM_ERROR_EOF,
    STREAM_ERROR_FILENOTFOUND,
    STREAM_ERROR_FILENOTOPENED,
    STREAM_ERROR_BADDATA,
    STREAM_ERROR_FILEDOESNTEXIST,
    STREAM_ERROR_NODEDOESNTEXIST,
};

class IRawStream {
public:
    virtual EStreamError Open(const chargr * fileName) = 0;

    virtual EStreamError ReadBytes(void * bytes, unsigned * bytesRead) = 0;
    virtual EStreamError WriteBytes(void * bytes, unsigned * bytesWritten) = 0;
};

class Stream {
public:
    Stream(IRawStream * rawStream);

};

class DataStream : public Stream {
public:
    DataStream(IRawStream * rawStream);

    template <typename T>
    EStreamError Read(T & value, unsigned * bytesRead);

    template <typename T>
    EStreamError Write(T value, unsigned * bytesWritten);

    EStreamError ReadBytes(void * bytes, unsigned * bytesRead);
    EStreamError WriteBytes(void * bytes, unsigned * bytesWritten);
};

class IStructuredTextStream : public RefCounted {
public:
    virtual ~IStructuredTextStream() { }

    virtual const chargr * GetName() const = 0;

    virtual EStreamError Save() = 0;

    virtual EStreamError WriteNode(const chargr * name) = 0;
    virtual EStreamError WriteNodeValue(const chargr * value) = 0;
    virtual EStreamError EndNode() = 0;
    virtual EStreamError WriteNodeAttribute(const chargr * name, const chargr * value) = 0;

    virtual EStreamError ReadNodeName(chargr * name, unsigned len) = 0;
    virtual EStreamError ReadNextNode() = 0;
    virtual EStreamError ReadParentNode() = 0;
    virtual EStreamError ReadNodeValue(chargr * value, unsigned len) const = 0;
    virtual EStreamError ReadNodeAttribute(
        const chargr  * name, 
        unsigned        nameLen,
        chargr        * value, 
        unsigned        len
    ) const = 0;
    virtual EStreamError ReadChildNode() = 0;
};

DECLARE_SMARTPTR(IStructuredTextStream);

IRawStream * StreamOpenFile(const chargr * fileName);
IRawStream * StreamOpenMemory(void * memory);
IStructuredTextStreamPtr StreamOpenXML(const chargr * fileName);
IStructuredTextStreamPtr StreamCreateXML(const chargr * fileName);

