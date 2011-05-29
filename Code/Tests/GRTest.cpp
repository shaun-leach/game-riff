/*
   GameRiff - Framework for creating various video game services
   Complicated test of various reflection bits
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
// pragma_directive_init_seg.cpp
#pragma warning(disable : 4075)

typedef void (__cdecl *PF)(void);
int cxpf = 0;   // number of destructors we need to call
PF pfx[200];    // pointers to destructors.

int myexit (PF pf) {
   pfx[cxpf++] = pf;
   return 0;
}

struct A {
   A() { puts("A()"); }
   ~A() { puts("~A()"); }
};

// ctor & dtor called by CRT startup code 
// because this is before the pragma init_seg
A aaaa; 

// The order here is important.
// Section names must be 8 characters or less.
// The sections with the same name before the $
// are merged into one section. The order that
// they are merged is determined by sorting
// the characters after the $.
// InitSegStart and InitSegEnd are used to set
// boundaries so we can find the real functions
// that we need to call for initialization.

#pragma section(".mine$a", read)
__declspec(allocate(".mine$a")) const PF InitSegStart = (PF)1;

#pragma section(".mine$z",read)
__declspec(allocate(".mine$z")) const PF InitSegEnd = (PF)1;

// The comparison for 0 is important.
// For now, each section is 256 bytes. When they
// are merged, they are padded with zeros. You
// can't depend on the section being 256 bytes, but
// you can depend on it being padded with zeros.

void InitializeObjects () {
   const PF *x = &InitSegStart;
   for (++x ; x < &InitSegEnd ; ++x)
      if (*x) 
         (*x)();
}

void DestroyObjects () {
   while (cxpf>0) {
      --cxpf;
      (pfx[cxpf])();
   }
}

// by default, goes into a read only section
/*#pragma init_seg(".mine$m", myexit)

A bbbb; 
A cccc;*/

class BaseClass : public ReflClass {
public:
   REFL_DEFINE_CLASS(BaseClass);
   BaseClass() :
      basei32test(392),
      basef32test(4.0f),
      basei16test(1600)
   {
   }

private:
   int32 basei32test;
   float32 basef32test;
   int16 basei16test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, BaseClass);
    REFL_ADD_CLASS_ALIAS(BaseClass, OldBaseClass);
    REFL_MEMBER(BaseClass, basei32test, int32);
    REFL_MEMBER(BaseClass, basef32test, float32);
    REFL_MEMBER(BaseClass, basei16test, int16);
REFL_IMPL_CLASS_END(BaseClass);

class BaseBaseClass2 : public ReflClass {
public:
   REFL_DEFINE_CLASS(BaseBaseClass2);
   BaseBaseClass2() :
      basebase2i32test(400),
      basebase2f32test(-127.0f),
      basebase2u16test(17)
   {
   }

private:
   int32 basebase2i32test;
   float32 basebase2f32test;
   uint16 basebase2u16test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, BaseBaseClass2);
    REFL_MEMBER(BaseBaseClass2, basebase2i32test, int32);
    REFL_MEMBER(BaseBaseClass2, basebase2f32test, float32);
    REFL_MEMBER(BaseBaseClass2, basebase2u16test, uint16);
REFL_IMPL_CLASS_END(BaseBaseClass2);

class BaseClass2 : public BaseBaseClass2 {
public:
   REFL_DEFINE_CLASS(BaseClass2);
   BaseClass2() :
      base2i32test(44998877),
      base2f32test(596.0f),
      base2u16test(4000)
   {
   }

private:
   int32 base2i32test;
   float32 base2f32test;
   uint16 base2u16test;
};

REFL_IMPL_CLASS_BEGIN(BaseBaseClass2, BaseClass2);
    REFL_ADD_PARENT(BaseClass2, BaseBaseClass2);
    REFL_MEMBER(BaseClass2, base2i32test, int32);
    REFL_MEMBER(BaseClass2, base2f32test, float32);
    REFL_MEMBER(BaseClass2, base2u16test, uint16);
REFL_IMPL_CLASS_END(BaseClass2);

namespace SubNameSpace {

class BaseSubClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(BaseSubClass);
    BaseSubClass() :
        basesubitest(445566),
        basesubftest(44.5566f)
    {
    }

private:
    int32 basesubitest;
    float basesubftest;
};

REFL_IMPL_CLASS_BEGIN_NAMESPACE(ReflClass, SubNameSpace, BaseSubClass);
    REFL_MEMBER(BaseSubClass, basesubitest, int32);
    REFL_MEMBER(BaseSubClass, basesubftest, float32);
REFL_IMPL_CLASS_END(BaseSubClass);

class SubClass : public BaseSubClass {
public:
    REFL_DEFINE_CLASS(SubClass);
    SubClass() :
        subitest(102),
        subftest(457.345f)
    {
    }

private:
    int32 subitest;
    float subftest;
};

REFL_IMPL_CLASS_BEGIN_NAMESPACE(BaseSubClass, SubNameSpace, SubClass);
    REFL_ADD_PARENT_NAMESPACE(SubClass, SubNameSpace, BaseSubClass);
    REFL_MEMBER(SubClass, subitest, int32);
    REFL_MEMBER(SubClass, subftest, float32);
REFL_IMPL_CLASS_END(SubClass);
}

class Dummy : public BaseClass, public BaseClass2 {
public:
    REFL_DEFINE_CLASS(Dummy);
    Dummy() :
        i32test(4),
        f32test(10.9f),
        i8test('b'),
        class_alias_test(0),
        btest(true),
        etest(INTERNAL_VALUE1),
        pertest(1.0f),
        angtest(2.57f)
    {
    }

    static void Convert(
       ReflClass   * inst, 
       Hash64        name, 
       Hash64        oldType, 
       void        * data
    ) {
       Dummy * dummy = dynamic_cast<Dummy *>(inst);
       if (dummy != NULL) {
          if (name == Hash64(L"btest") && oldType == ReflTypeInt32) {
             int32 * idata = reinterpret_cast<int32 *>(data);
             
             dummy->btest = *idata > 0 ? true : false;
          }
       }
    }

    static void ConvertITest(
       ReflClass   * inst, 
       Hash64        name, 
       Hash64        oldType, 
       void        * data
    ) {
       Dummy * dummy = dynamic_cast<Dummy *>(inst);
       if (dummy != NULL) {
          if (name == Hash64(L"itest") && oldType == Hash64(L"float32")) {
             float32 * fdata = reinterpret_cast<float32 *>(data);

             dummy->i32test = (int32) ceilf(*fdata);
          }
       }
    }

    static void ConvertClassTest(
       ReflClass   * inst, 
       Hash64        name, 
       Hash64        oldType, 
       void        * data
    ) {
       Dummy * dummy = dynamic_cast<Dummy *>(inst);
       if (dummy != NULL) {
          if (name == Hash64(L"basesubitest") && oldType == ReflTypeInt32) {
             int32 * idata = reinterpret_cast<int32 *>(data);

             dummy->class_alias_test = (*idata);
          }
       }
    }
private:
    enum EInternal {
        INTERNAL_VALUE1,
        INTERNAL_VALUE2
    };

    int32       i32test;
    float32     f32test;
    int8        i8test;
    int32       class_alias_test;
    bool        btest;
    EInternal   etest;
    float       pertest;
    float       angtest;
    SubNameSpace::SubClass    ctest;
};

REFL_IMPL_CLASS_BEGIN(BaseClass, Dummy);
    REFL_ADD_PARENT(Dummy, BaseClass);
    REFL_ADD_PARENT(Dummy, BaseClass2);
    REFL_MEMBER(Dummy, i32test, int32);
    REFL_ADD_MEMBER_ALIAS_W_CONVERSION(i32test, itest, ConvertITest);
    REFL_MEMBER(Dummy, f32test, float32);
    REFL_MEMBER(Dummy, i8test, int8);
    REFL_MEMBER(Dummy, class_alias_test, int32);
    REFL_ADD_MEMBER_ALIAS_W_CONVERSION(class_alias_test, mcTest, ConvertClassTest);
    REFL_MEMBER(Dummy, btest, bool);
    REFL_ADD_MEMBER_CONVERSION(btest, Convert);
    REFL_MEMBER(Dummy, etest, enum);
        REFL_ENUM_VALUE(etest, INTERNAL_VALUE1);
        REFL_ENUM_VALUE(etest, INTERNAL_VALUE2);
        REFL_ENUM_ALIAS(etest, INTERNAL_VALUE2, INTERNAL_VALUE3);
    REFL_MEMBER(Dummy, pertest, percentage);
    REFL_MEMBER(Dummy, angtest, angle);
    REFL_ADD_MEMBER_ALIAS(angtest, atest);
    REFL_MEMBER(Dummy, ctest, SubNameSpace::SubClass);
REFL_IMPL_CLASS_END(Dummy);

int main(int argc,  char * argv[]) {
   InitializeObjects();
/*    
    IStructuredTextStream * testStream = StreamCreateXML(L"test.xml");

    Dummy testClass;

    testClass.Serialize(testStream);

    testStream->Save();
//*/
    
//*
    IStructuredTextStream * testStream = StreamOpenXML(L"test.xml");

    Dummy testClass;
    //testClass.Deserialize(testStream);
    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    Dummy * dummy = dynamic_cast<Dummy *>(inst);
//*/
    DestroyObjects();
}
