/*
   GameRiff - Framework for creating various video game services
   Implementation of xml streamed i/o service
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

namespace NSXMLStream {

//////////////////////////////////////////////////////
//
// Constants
//

#define XML_MEM_FLAGS (MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_XML))

//////////////////////////////////////////////////////
//
// Internal
//

class XMLTextStream : public IStructuredTextStream {
public:
    XMLTextStream();
    XMLTextStream(const chargr * fileName);
    ~XMLTextStream();

    EStreamError Open(const chargr * fileName);
    EStreamError Save();
    void Close();

    EStreamError WriteNode(const chargr * name);
    EStreamError WriteNodeValue(const chargr * value);
    EStreamError EndNode();
    EStreamError WriteNodeAttribute(const chargr * name, const chargr * value);

    EStreamError ReadNodeName(chargr * name, unsigned len);
    EStreamError ReadNextNode();
    EStreamError ReadParentNode();
    EStreamError ReadNodeValue(chargr * value, unsigned len) const;
    EStreamError ReadNodeAttribute(const chargr * name, chargr * value, unsigned len) const;
    EStreamError ReadChildNode();

private:

    EStreamError DecodeTiXmlError();

private:
    TiXmlNode     * m_currentNode;
    TiXmlDocument * m_document;
};

//====================================================
XMLTextStream::XMLTextStream() :
    m_document(NULL),
    m_currentNode(NULL)
{

}

//====================================================
XMLTextStream::XMLTextStream(const chargr * fileName) :
    m_document(NULL),
    m_currentNode(NULL)
{
    charsys * sysfile = StrCreateUtf8(fileName, XML_MEM_FLAGS);
    m_document = new(XML_MEM_FLAGS) TiXmlDocument(sysfile);
    TiXmlDeclaration * decl = new(XML_MEM_FLAGS) TiXmlDeclaration("1.0", "", "");
    m_document->LinkEndChild(decl);
}

//====================================================
XMLTextStream::~XMLTextStream() {
    if (m_document != NULL) {
        delete m_document;
        m_document = NULL;
    }

    m_currentNode = NULL;
}

//====================================================
EStreamError XMLTextStream::Open(const chargr *fileName) {
    EStreamError result = STREAM_ERROR_OK;

    m_document = new(XML_MEM_FLAGS) TiXmlDocument();

    charsys * sysfile = StrCreateUtf8(fileName, XML_MEM_FLAGS);
    bool tiResult = m_document->LoadFile(sysfile, TIXML_ENCODING_UTF8);
    if (!tiResult) {
        delete m_document;
        m_document = NULL;
        result = DecodeTiXmlError();
    }

    m_currentNode = m_document->FirstChild();
    while(m_currentNode != NULL && m_currentNode->Type() == TiXmlNode::DECLARATION) {
        m_currentNode = m_currentNode->NextSibling();
    }

    return result;
}

//====================================================
EStreamError XMLTextStream::Save() {
    EStreamError result = STREAM_ERROR_OK;
    if (m_document != NULL) {
        if (!m_document->SaveFile())
            result = DecodeTiXmlError();
    }
    else
        result = STREAM_ERROR_FILEDOESNTEXIST;

    return result;
}

//====================================================
void XMLTextStream::Close() {
    if (m_document != NULL) {
        delete m_document;
        m_document = NULL;
    }
    m_currentNode = NULL;
}

//====================================================
EStreamError XMLTextStream::WriteNode(const chargr * name) {
    EStreamError result = STREAM_ERROR_OK;

    if (m_document == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    charsys * sysname = StrCreateUtf8(name, XML_MEM_FLAGS);
    TiXmlElement * node = new(XML_MEM_FLAGS) TiXmlElement(sysname);

    if (m_currentNode == NULL) 
        m_currentNode = m_document->LinkEndChild(node);
    else 
        m_currentNode = m_currentNode->LinkEndChild(node);

    if (m_currentNode == NULL)
        result = STREAM_ERROR_BADDATA;

    return result;
}

//====================================================
EStreamError XMLTextStream::WriteNodeValue(const chargr * value) {
    EStreamError result = STREAM_ERROR_OK;

    if (m_document == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    if (m_currentNode != NULL) {
        charsys * sysvalue = StrCreateUtf8(value, XML_MEM_FLAGS);
        TiXmlText * nodeValue = new(XML_MEM_FLAGS) TiXmlText(sysvalue);
        if (m_currentNode->LinkEndChild(nodeValue) == NULL)
            result = DecodeTiXmlError();
    }
    else
        result = STREAM_ERROR_BADDATA;

    return result;
}

//====================================================
EStreamError XMLTextStream::EndNode() {
    // We're creating a new node, so attach it to the parent
    m_currentNode = m_currentNode->Parent();

    return DecodeTiXmlError();
}

//====================================================
EStreamError XMLTextStream::WriteNodeAttribute(const chargr * name, const chargr * value) {
    EStreamError result = STREAM_ERROR_FILENOTOPENED;
    if (m_currentNode != NULL) {

        TiXmlElement * element = m_currentNode->ToElement();
        if (element != NULL) {
            charsys * sysname = StrCreateUtf8(name, XML_MEM_FLAGS);
            charsys * sysvalue = StrCreateUtf8(value, XML_MEM_FLAGS);

            element->SetAttribute(sysname, sysvalue);
            result = DecodeTiXmlError();
        }
        else
            result = STREAM_ERROR_BADDATA;
    }
    return result;
}

//====================================================
EStreamError XMLTextStream::ReadNodeName(chargr * name, unsigned len) {
    if (m_document == NULL || m_currentNode == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    StrUtf8ConvertToCharGr(m_currentNode->Value(), name, len);

    return STREAM_ERROR_OK;
}

//====================================================
EStreamError XMLTextStream::ReadNextNode() {
    if (m_document == NULL || m_currentNode == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    TiXmlNode * node = m_currentNode->NextSibling();

    if (node != NULL) 
        m_currentNode = node;
    else 
        return STREAM_ERROR_NODEDOESNTEXIST;

    return STREAM_ERROR_OK;
}

//====================================================
EStreamError XMLTextStream::ReadParentNode() {
    if (m_document == NULL || m_currentNode == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    TiXmlNode * node = m_currentNode->Parent();

    if (node != NULL) 
        m_currentNode = node;
    else 
        return STREAM_ERROR_NODEDOESNTEXIST;

    return STREAM_ERROR_OK;
}

//====================================================
EStreamError XMLTextStream::ReadChildNode() {
    if (m_document == NULL || m_currentNode == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    TiXmlNode * child = m_currentNode->FirstChild();

    if (child != NULL) 
        m_currentNode = child;
    else
        return STREAM_ERROR_NODEDOESNTEXIST;

    return STREAM_ERROR_OK;
}

//====================================================
EStreamError XMLTextStream::ReadNodeValue(chargr * value, unsigned len) const {
    if (m_document == NULL || m_currentNode == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    TiXmlNode * child = m_currentNode->FirstChild();
    ASSERTMSGGR(child->Type() == TiXmlNode::TEXT, "Node must be of type text");
    TiXmlText * text = child->ToText();

    StrUtf8ConvertToCharGr(text->Value(), value, len);

    return STREAM_ERROR_OK;
}

//====================================================
EStreamError XMLTextStream::ReadNodeAttribute(const chargr * name, chargr * value, unsigned len) const {
    if (m_document == NULL || m_currentNode == NULL) 
        return STREAM_ERROR_FILENOTOPENED;

    ASSERTMSGGR(m_currentNode->Type() == TiXmlNode::ELEMENT, "Node must be of type text");

    TiXmlElement * node = m_currentNode->ToElement();

    StrStackConverter sysName(name);
    const char * attr = node->Attribute(sysName);
    if (attr != NULL) 
        StrUtf8ConvertToCharGr(attr, value, len);
    else
        value[0] = L'\0';

    return STREAM_ERROR_OK;
}

//====================================================
EStreamError XMLTextStream::DecodeTiXmlError() {
    EStreamError result = STREAM_ERROR_FILENOTOPENED;

    if (m_document != NULL) {
        switch(m_document->ErrorId()) {
        case TiXmlBase::TIXML_ERROR_OPENING_FILE:
            result = STREAM_ERROR_FILENOTFOUND;
            break;
        case TiXmlBase::TIXML_NO_ERROR:
            result = STREAM_ERROR_OK;
            break;
        default:
            ASSERTMSGGR(false, "Handle this error code");
        }    
    }

    return result;
}

} // namespace NSXMLStream

//////////////////////////////////////////////////////
//
// External functions
//

//====================================================
IStructuredTextStream * StreamOpenXML(const chargr * fileName) {

    NSXMLStream::XMLTextStream * stream = new(XML_MEM_FLAGS) NSXMLStream::XMLTextStream();
    EStreamError result = stream->Open(fileName);

    if (result != STREAM_ERROR_OK) {
        delete stream;
        stream = NULL;
    }

    return stream;
}

//====================================================
IStructuredTextStream * StreamCreateXML(const chargr * fileName) {

    NSXMLStream::XMLTextStream * stream = new(XML_MEM_FLAGS) NSXMLStream::XMLTextStream(fileName);

    return stream;
}


