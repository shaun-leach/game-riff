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
      InitReflType();
   }

private:
   int32 basei32test;
   float32 basef32test;
   int16 basei16test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, BaseClass);
    REFL_ADD_CLASS_ALIAS(BaseClass, OldBaseClass);
    REFL_MEMBER(BaseClass, basei32test);
    REFL_MEMBER(BaseClass, basef32test);
    REFL_MEMBER(BaseClass, basei16test);
REFL_IMPL_CLASS_END(BaseClass);

class BaseBaseClass2 : public ReflClass {
public:
   REFL_DEFINE_CLASS(BaseBaseClass2);
   BaseBaseClass2() :
      basebase2i32test(400),
      basebase2f32test(-127.0f),
      basebase2u16test(17)
   {
      InitReflType();
   }

private:
   int32 basebase2i32test;
   float32 basebase2f32test;
   uint16 basebase2u16test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, BaseBaseClass2);
    REFL_MEMBER(BaseBaseClass2, basebase2i32test);
    REFL_MEMBER(BaseBaseClass2, basebase2f32test);
    REFL_MEMBER(BaseBaseClass2, basebase2u16test);
REFL_IMPL_CLASS_END(BaseBaseClass2);

class BaseClass2 : public BaseBaseClass2 {
public:
   REFL_DEFINE_CLASS(BaseClass2);
   BaseClass2() :
      base2i32test(44998877),
      base2f32test(596.0f),
      base2u16test(4000)
   {
      InitReflType();
   }

private:
   int32 base2i32test;
   float32 base2f32test;
   uint16 base2u16test;
};

REFL_IMPL_CLASS_BEGIN(BaseBaseClass2, BaseClass2);
    REFL_ADD_PARENT(BaseClass2, BaseBaseClass2);
    REFL_MEMBER(BaseClass2, base2i32test);
    REFL_MEMBER(BaseClass2, base2f32test);
    REFL_MEMBER(BaseClass2, base2u16test);
REFL_IMPL_CLASS_END(BaseClass2);

namespace SubNameSpace {

class BaseSubClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(BaseSubClass);
    BaseSubClass() :
        basesubitest(445566),
        basesubftest(44.5566f)
    {
       InitReflType();
    }

private:
    int32 basesubitest;
    float basesubftest;
};

REFL_IMPL_CLASS_BEGIN_NAMESPACE(ReflClass, SubNameSpace, BaseSubClass);
    REFL_MEMBER(BaseSubClass, basesubitest);
    REFL_MEMBER(BaseSubClass, basesubftest);
REFL_IMPL_CLASS_END(BaseSubClass);

class SubClass : public BaseSubClass {
public:
    REFL_DEFINE_CLASS(SubClass);
    SubClass() :
        subitest(102),
        subftest(457.345f)
    {
       InitReflType();
    }

private:
    int32 subitest;
    float subftest;
};

REFL_IMPL_CLASS_BEGIN_NAMESPACE(BaseSubClass, SubNameSpace, SubClass);
    REFL_ADD_PARENT_NAMESPACE(SubClass, SubNameSpace, BaseSubClass);
    REFL_MEMBER(SubClass, subitest);
    REFL_MEMBER(SubClass, subftest);
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
       InitReflType();
    }

    virtual ~Dummy() {

    }

    static void Convert(
       ReflClass   * inst, 
       ReflHash      name, 
       ReflHash      oldType, 
       void        * data
    ) {
       Dummy * dummy = ReflCast<Dummy>(inst);
       if (dummy != NULL) {
          if (name == ReflHash(L"btest") && oldType == ReflHash(L"int32")) {
             int32 * idata = reinterpret_cast<int32 *>(data);
             
             dummy->btest = *idata > 0 ? true : false;
          }
       }
    }

    static void ConvertITest(
       ReflClass   * inst, 
       ReflHash      name, 
       ReflHash      oldType, 
       void        * data
    ) {
       Dummy * dummy = ReflCast<Dummy>(inst);
       if (dummy != NULL) {
          if (name == ReflHash(L"itest") && oldType == ReflHash(L"float32")) {
             float32 * fdata = reinterpret_cast<float32 *>(data);

             dummy->i32test = (int32) ceilf(*fdata);
          }
       }
    }

    static void ConvertClassTest(
       ReflClass   * inst, 
       ReflHash      name, 
       ReflHash      oldType, 
       void        * data
    ) {
       Dummy * dummy = ReflCast<Dummy>(inst);
       if (dummy != NULL) {
          if (name == ReflHash(L"basesubitest") && oldType == ReflHash(L"int32")) {
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
    REFL_MEMBER(Dummy, i32test);
    REFL_ADD_MEMBER_ALIAS_W_CONVERSION(i32test, itest, ConvertITest);
    REFL_MEMBER(Dummy, f32test);
    REFL_MEMBER(Dummy, i8test);
    REFL_MEMBER(Dummy, class_alias_test);
    REFL_ADD_MEMBER_ALIAS_W_CONVERSION(class_alias_test, mcTest, ConvertClassTest);
    REFL_MEMBER(Dummy, btest);
    REFL_ADD_MEMBER_CONVERSION(btest, Convert);
    REFL_MEMBER_ENUM(Dummy, etest);
        REFL_ENUM_VALUE(etest, INTERNAL_VALUE1);
        REFL_ENUM_VALUE(etest, INTERNAL_VALUE2);
        REFL_ENUM_ALIAS(etest, INTERNAL_VALUE2, INTERNAL_VALUE3);
    REFL_MEMBER(Dummy, pertest);
    REFL_MEMBER(Dummy, angtest);
    REFL_ADD_MEMBER_ALIAS(angtest, atest);
    REFL_MEMBER(Dummy, ctest);
REFL_IMPL_CLASS_END(Dummy);

class SimpleCastBaseClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleCastBaseClass);
    SimpleCastBaseClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleCastBaseClass);
    REFL_MEMBER(SimpleCastBaseClass, baseUint32Test);
    REFL_MEMBER(SimpleCastBaseClass, baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleCastBaseClass);

class SimpleCastBaseClass2 : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleCastBaseClass2);
    SimpleCastBaseClass2() :
        base2Uint32Test(0),
        base2Float32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      base2Uint32Test;
    float32     base2Float32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleCastBaseClass2);
    REFL_MEMBER(SimpleCastBaseClass2, base2Uint32Test);
    REFL_MEMBER(SimpleCastBaseClass2, base2Float32Test);
REFL_IMPL_CLASS_END(SimpleCastBaseClass2);

class SimpleCastBaseClass3 : public SimpleCastBaseClass2 {
public:
    REFL_DEFINE_CLASS(SimpleCastBaseClass3);
    SimpleCastBaseClass3() :
        base3Uint32Test(0),
        base3Float32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      base3Uint32Test;
    float32     base3Float32Test;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass2, SimpleCastBaseClass3);
    REFL_ADD_PARENT(SimpleCastBaseClass3, SimpleCastBaseClass2);
    REFL_MEMBER(SimpleCastBaseClass3, base3Uint32Test);
    REFL_MEMBER(SimpleCastBaseClass3, base3Float32Test);
REFL_IMPL_CLASS_END(SimpleCastBaseClass3);

class ComplexInheritanceCastClass : public SimpleCastBaseClass, public SimpleCastBaseClass3 {
public:
    REFL_DEFINE_CLASS(ComplexInheritanceCastClass);
    ComplexInheritanceCastClass() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass, ComplexInheritanceCastClass);
    REFL_ADD_PARENT(ComplexInheritanceCastClass, SimpleCastBaseClass);
    REFL_ADD_PARENT(ComplexInheritanceCastClass, SimpleCastBaseClass3);
    REFL_MEMBER(ComplexInheritanceCastClass, derivedBoolTest);
    REFL_MEMBER(ComplexInheritanceCastClass, derivedInt16Test);
    REFL_MEMBER(ComplexInheritanceCastClass, derivedInt16Test2);
REFL_IMPL_CLASS_END(ComplexInheritanceCastClass);

//====================================================
void TestCastingFromMultipleBases() {

    ComplexInheritanceCastClass testCasting;
    testCasting.baseUint32Test      = 320000;
    testCasting.baseFloat32Test     = 32.32f;
    testCasting.base2Uint32Test     = 310000;
    testCasting.base2Float32Test    = 31.31f;
    testCasting.base3Uint32Test     = 300000;
    testCasting.base3Float32Test    = 30.30f;
    testCasting.derivedBoolTest     = true;
    testCasting.derivedInt16Test    = 1600;
    testCasting.derivedInt16Test2   = 1601;

    IStructuredTextStream * testStream = StreamCreateXML(L"testComplexMultipleInheritanceCasting.xml");
    bool result = ReflLibrary::Serialize(testStream, static_cast<SimpleCastBaseClass *>(&testCasting));
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testComplexMultipleInheritanceCasting.xml");

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ComplexInheritanceCastClass * loadTypes = ReflCast<ComplexInheritanceCastClass>(inst);

    Dummy * castNull = ReflCast<Dummy>(inst);

    SimpleCastBaseClass  * base1    = ReflCast<SimpleCastBaseClass>(inst);
    SimpleCastBaseClass2 * base2    = ReflCast<SimpleCastBaseClass2>(inst);
    SimpleCastBaseClass3 * base3    = ReflCast<SimpleCastBaseClass3>(inst);

    //ReflClass            * instD    = ReflCast<ReflClass>(loadTypes);
    ReflClass            * inst1    = ReflCast<ReflClass>(base1);
    ReflClass            * inst2    = ReflCast<ReflClass>(base2);
    ReflClass            * inst3    = ReflCast<ReflClass>(base3);

    ComplexInheritanceCastClass * derived  = ReflCast<ComplexInheritanceCastClass>(base1);
    ComplexInheritanceCastClass * derived2 = ReflCast<ComplexInheritanceCastClass>(base2);
    ComplexInheritanceCastClass * derived4 = ReflCast<ComplexInheritanceCastClass>(base3);

    SimpleCastBaseClass  * base12 = ReflCast<SimpleCastBaseClass>(base2);
    SimpleCastBaseClass  * base13 = ReflCast<SimpleCastBaseClass>(base3);
    SimpleCastBaseClass2 * base21 = ReflCast<SimpleCastBaseClass2>(base1);
    SimpleCastBaseClass2 * base23 = ReflCast<SimpleCastBaseClass2>(base3);
    SimpleCastBaseClass3 * base31 = ReflCast<SimpleCastBaseClass3>(base1);
    SimpleCastBaseClass3 * base32 = ReflCast<SimpleCastBaseClass3>(base2);

    delete loadTypes;
    loadTypes = NULL;
}

class VirtualBaseClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(VirtualBaseClass);
    VirtualBaseClass() :
        basevUint32Test(0),
        basevFloat32Test(0.0f)
    {
        InitReflType();
    }

    virtual ~VirtualBaseClass() {
    }
//private:
    uint32      basevUint32Test;
    float32     basevFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, VirtualBaseClass);
    REFL_MEMBER(VirtualBaseClass, basevUint32Test);
    REFL_MEMBER(VirtualBaseClass, basevFloat32Test);
REFL_IMPL_CLASS_END(VirtualBaseClass);

class CastWithVirtualsInBase : public VirtualBaseClass {
public:
    REFL_DEFINE_CLASS(CastWithVirtualsInBase);
    CastWithVirtualsInBase() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(VirtualBaseClass, CastWithVirtualsInBase);
    REFL_ADD_PARENT(CastWithVirtualsInBase, VirtualBaseClass);
    REFL_MEMBER(CastWithVirtualsInBase, derivedBoolTest);
    REFL_MEMBER(CastWithVirtualsInBase, derivedInt16Test);
    REFL_MEMBER(CastWithVirtualsInBase, derivedInt16Test2);
REFL_IMPL_CLASS_END(CastWithVirtualsInBase);

//====================================================
void TestCastingWVirtualsInBase() {
    CastWithVirtualsInBase testCasting;
    testCasting.basevUint32Test     = 3200000;
    testCasting.basevFloat32Test    = 32.32f;
    testCasting.derivedBoolTest     = true;
    testCasting.derivedInt16Test    = 17000;
    testCasting.derivedInt16Test2   = 17001;

    IStructuredTextStream * testStream = StreamCreateXML(L"testCastingWVirtualBase.xml");
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testCastingWVirtualBase.xml");

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    CastWithVirtualsInBase * loadTypes = ReflCast<CastWithVirtualsInBase>(inst);

    CastWithVirtualsInBase * self = ReflCast<CastWithVirtualsInBase>(loadTypes);

    VirtualBaseClass * base = ReflCast<VirtualBaseClass>(inst);

    delete loadTypes;
    loadTypes = NULL;
}

class NonreflectedClass {
public:
    NonreflectedClass() :
        nfUint32Test(0),
        nfFloat32Test(0.0f)
    {
    }

    virtual ~NonreflectedClass() {
    }
//private:
    uint32      nfUint32Test;
    float32     nfFloat32Test;
};

class SimpleCastWithVirtualsNonreflectedBase : public NonreflectedClass, public SimpleCastBaseClass {
public:
    REFL_DEFINE_CLASS(SimpleCastWithVirtualsNonreflectedBase);
    SimpleCastWithVirtualsNonreflectedBase() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass, SimpleCastWithVirtualsNonreflectedBase);
    REFL_ADD_PARENT(SimpleCastWithVirtualsNonreflectedBase, SimpleCastBaseClass);
    REFL_MEMBER(SimpleCastWithVirtualsNonreflectedBase, derivedBoolTest);
    REFL_MEMBER(SimpleCastWithVirtualsNonreflectedBase, derivedInt16Test);
    REFL_MEMBER(SimpleCastWithVirtualsNonreflectedBase, derivedInt16Test2);
REFL_IMPL_CLASS_END(SimpleCastWithVirtualsNonreflectedBase);

//====================================================
void TestCastingWVirtualsNonreflectedBase() {
    SimpleCastWithVirtualsNonreflectedBase testCasting;
    testCasting.baseUint32Test      = 320000;
    testCasting.baseFloat32Test     = 32.32f;
    testCasting.derivedBoolTest     = true;
    testCasting.derivedInt16Test    = 18000;
    testCasting.derivedInt16Test2   = 18001;

    IStructuredTextStream * testStream = StreamCreateXML(L"testCastingWVirtualsNonreflectedBase.xml");
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testCastingWVirtualsNonreflectedBase.xml");

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    SimpleCastWithVirtualsNonreflectedBase * loadTypes = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(inst);

    SimpleCastWithVirtualsNonreflectedBase * self = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(loadTypes);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    SimpleCastWithVirtualsNonreflectedBase * derived1 = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(base);

    ReflClass * inst1 = ReflCast<ReflClass>(base);
    ReflClass * inst2 = ReflCast<ReflClass>(loadTypes);
    SimpleCastBaseClass * base2 = ReflCast<SimpleCastBaseClass>(loadTypes);

    delete loadTypes;
    loadTypes = NULL;
}

class CastWithVirtualsIn2ndBase : public SimpleCastBaseClass, public VirtualBaseClass {
public:
    REFL_DEFINE_CLASS(CastWithVirtualsIn2ndBase);
    CastWithVirtualsIn2ndBase() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass, CastWithVirtualsIn2ndBase);
    REFL_ADD_PARENT(CastWithVirtualsIn2ndBase, VirtualBaseClass);
    REFL_ADD_PARENT(CastWithVirtualsIn2ndBase, SimpleCastBaseClass);
    REFL_MEMBER(CastWithVirtualsIn2ndBase, derivedBoolTest);
    REFL_MEMBER(CastWithVirtualsIn2ndBase, derivedInt16Test);
    REFL_MEMBER(CastWithVirtualsIn2ndBase, derivedInt16Test2);
REFL_IMPL_CLASS_END(CastWithVirtualsIn2ndBase);

//====================================================
void TestCastingWVirtualsIn2ndBase() {
    CastWithVirtualsIn2ndBase testCasting;
    testCasting.baseUint32Test      = 330000;
    testCasting.baseFloat32Test     = 33.33f;
    testCasting.basevUint32Test     = 320000;
    testCasting.basevFloat32Test    = 32.32f;
    testCasting.derivedBoolTest     = true;
    testCasting.derivedInt16Test    = 19000;
    testCasting.derivedInt16Test2   = 19001;

    IStructuredTextStream * testStream = StreamCreateXML(L"testCastingWVirtual2ndBase.xml");
    bool result = ReflLibrary::Serialize(testStream, static_cast<VirtualBaseClass *>(&testCasting));
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testCastingWVirtual2ndBase.xml");

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    CastWithVirtualsIn2ndBase * actual = ReflCast<CastWithVirtualsIn2ndBase>(inst);

    //CastWithVirtualsIn2ndBase * self = ReflCast<CastWithVirtualsIn2ndBase>(actual);
    //EXPECT_EQ(actual, self);

    SimpleCastBaseClass * base1 = ReflCast<SimpleCastBaseClass>(inst);

    //VirtualBaseClass * base1FromActual = ReflCast<VirtualBaseClass>(actual);
    //EXPECT_EQ(base1, base1FromActual);

    VirtualBaseClass * base2 = ReflCast<VirtualBaseClass>(inst);

    //VirtualBaseClass * base2FromActual = ReflCast<VirtualBaseClass>(actual);
    //EXPECT_EQ(base2, base2FromActual);

    SimpleCastBaseClass * base12 = ReflCast<SimpleCastBaseClass>(base2);
    VirtualBaseClass    * base21 = ReflCast<VirtualBaseClass>(base1);

    delete actual;
    actual = NULL;
}

int main(int argc,  char * argv[]) {
   InitializeObjects();
/*    
    IStructuredTextStream * testStream = StreamCreateXML(L"test.xml");

    Dummy testClass;
 
    ReflLibrary::Serialize(testStream, &testClass);
    testClass.Serialize(testStream);

    testStream->Save();
//*/
    
   TestCastingWVirtualsIn2ndBase();    
   TestCastingWVirtualsNonreflectedBase();    
   TestCastingWVirtualsInBase();    
   TestCastingFromMultipleBases();
//*

    IStructuredTextStream * testStream = StreamOpenXML(L"test.xml");

    Dummy testClass;
    //testClass.Deserialize(testStream);
    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    Dummy * dummy             = ReflCast<Dummy>(inst);
    BaseBaseClass2 * basebase = ReflCast<BaseBaseClass2>(inst);
    BaseClass * base          = ReflCast<BaseClass>(inst);
    BaseClass2 * base2        = ReflCast<BaseClass2>(inst);
//*/
    DestroyObjects();
}
