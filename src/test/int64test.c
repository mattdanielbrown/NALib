
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.



#include "../NALib/NABase.h"
#include "../NALib/NADateTime.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int64 randInt64();
uint64 randUInt64();
double getTime();
double printAndSwapTime(const char* title, double starttime);
void printInt64(int64 i);
void printUInt64(uint64 ui);
void testEqualInt64(int64 i, uint32 goalhi, uint32 goallo);
void testEqualUInt64(uint64 ui, uint32 goalhi, uint32 goallo);
void testEqualBool(NABool b, NABool goalb);



#define TESTSIZE 1000000



void testMaking(){
  int64 i1 = naMakeInt64WithLo(-0x12345678);
  int64 i2 = naMakeInt64WithDouble(-123456789012345.);
  uint64 ui1 = naMakeUInt64WithLiteralLo(0x12345678);
  uint64 ui2 = naMakeUInt64WithLo(0x12345678);
  uint64 ui3 = naMakeUInt64WithDouble(123456789012345.);
  uint64 ui4 = naMakeUInt64(0x12345678, 0x56473829);
  testEqualInt64(i1, 0xffffffff, 0xedcba988);
  testEqualInt64(i2, 0xffff8fb7, 0x79f22087);
  testEqualUInt64(ui1, 0x00000000, 0x12345678);
  testEqualUInt64(ui2, 0x00000000, 0x12345678);
  testEqualUInt64(ui3, 0x00007048 , 0x860ddf79);
  testEqualUInt64(ui4, 0x12345678 , 0x56473829);
}



void timeMaking(){
  double t;
  int i;
  int64 i64;
  uint64 ui64;
  
  t = getTime();
  for(i=0; i<TESTSIZE; i++){i64 = naMakeInt64WithLo(-rand());}
  t = printAndSwapTime("naMakeInt64WithLo", t);
  for(i=0; i<TESTSIZE; i++){i64 = naMakeInt64WithDouble(-(double)rand() * (double)rand());}
  t = printAndSwapTime("naMakeInt64WithDouble", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naMakeUInt64WithLo((uint32)rand());}
  t = printAndSwapTime("naMakeUInt64WithLo", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naMakeUInt64WithDouble((double)rand() * (double)rand());}
  t = printAndSwapTime("naMakeUInt64WithDouble", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naMakeUInt64((uint32)rand(), (uint32)rand());}
  t = printAndSwapTime("naMakeUInt64", t);
}



void testBinary(){
  int64 i = naMakeInt64WithDouble(-123456789012345.);
  int64 term = naCastUInt64ToInt64(naMakeUInt64(0x55555555, 0x55555555));
  uint64 ui = naMakeUInt64(0x12345678, 0x56473829);
  uint64 uterm = naMakeUInt64(0x55555555, 0x55555555);

  int64 i1 = naNotInt64(i);
  int64 i2 = naOrInt64(i, term);
  int64 i3 = naAndInt64(i, term);
  int64 i4 = naXorInt64(i, term);
  int64 i5 = naShlInt64(i, 5);
  int64 i6 = naShrInt64(i, 5);

  uint64 ui1 = naNotUInt64(ui);
  uint64 ui2 = naOrUInt64(ui, uterm);
  uint64 ui3 = naAndUInt64(ui, uterm);
  uint64 ui4 = naXorUInt64(ui, uterm);
  uint64 ui5 = naShlUInt64(ui, 5);
  uint64 ui6 = naShrUInt64(ui, 5);

  testEqualInt64(i1, 0x00007048 , 0x860ddf78);
  testEqualInt64(i2, 0xffffdff7 , 0x7df775d7);
  testEqualInt64(i3, 0x55550515 , 0x51500005);
  testEqualInt64(i4, 0xaaaadae2 , 0x2ca775d2);
  testEqualInt64(i5, 0xfff1f6ef , 0x3e4410e0);
  testEqualInt64(i6, 0xfffffc7d , 0xbbcf9104);

  testEqualUInt64(ui1, 0xedcba987 , 0xa9b8c7d6);
  testEqualUInt64(ui2, 0x5775577d , 0x57577d7d);
  testEqualUInt64(ui3, 0x10145450 , 0x54451001);
  testEqualUInt64(ui4, 0x4761032d , 0x03126d7c);
  testEqualUInt64(ui5, 0x468acf0a , 0xc8e70520);
  testEqualUInt64(ui6, 0x0091a2b3 , 0xc2b239c1);
}



void timeBinary(){
  double t;
  int i;
  int64 i64;
  uint64 ui64;
  
  t = getTime();
  for(i=0; i<TESTSIZE; i++){i64 = naNotInt64(randInt64());}
  t = printAndSwapTime("naNotInt64", t);
  for(i=0; i<TESTSIZE; i++){i64 = naOrInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naOrInt64", t);
  for(i=0; i<TESTSIZE; i++){i64 = naAndInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naAndInt64", t);
  for(i=0; i<TESTSIZE; i++){i64 = naXorInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naXorInt64", t);
  for(i=0; i<TESTSIZE; i++){i64 = naShlInt64(randInt64(), (int)(32. * (double)rand() / (double)RAND_MAX));}
  t = printAndSwapTime("naShlInt64", t);
  for(i=0; i<TESTSIZE; i++){i64 = naShrInt64(randInt64(), (int)(32. * (double)rand() / (double)RAND_MAX));}
  t = printAndSwapTime("naShrInt64", t);

  for(i=0; i<TESTSIZE; i++){ui64 = naNotUInt64(randUInt64());}
  t = printAndSwapTime("naNotUInt64", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naOrUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naOrUInt64", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naAndUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naAndUInt64", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naXorUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naXorUInt64", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naShlUInt64(randUInt64(), (int)(32. * (double)rand() / (double)RAND_MAX));}
  t = printAndSwapTime("naShlUInt64", t);
  for(i=0; i<TESTSIZE; i++){ui64 = naShrUInt64(randUInt64(), (int)(32. * (double)rand() / (double)RAND_MAX));}
  t = printAndSwapTime("naShrUInt64", t);
}



void testComparison(){
  int64 i1 = naMakeInt64WithDouble(-123456789012345.);
  int64 i2 = naMakeInt64WithDouble(-123456.);
  uint64 ui1 = naMakeUInt64WithDouble(123456789012345.);
  uint64 ui2 = naMakeUInt64WithDouble(123456.);

  testEqualBool(naEqualInt64(i1, i1), NA_TRUE);
  testEqualBool(naGreaterInt64(i1, i2), NA_FALSE);
  testEqualBool(naGreaterInt64(i2, i1), NA_TRUE);
  testEqualBool(naGreaterInt64(i1, i1), NA_FALSE);
  testEqualBool(naGreaterEqualInt64(i1, i2), NA_FALSE);
  testEqualBool(naGreaterEqualInt64(i2, i1), NA_TRUE);
  testEqualBool(naGreaterEqualInt64(i1, i1), NA_TRUE);
  testEqualBool(naSmallerInt64(i1, i2), NA_TRUE);
  testEqualBool(naSmallerInt64(i2, i1), NA_FALSE);
  testEqualBool(naSmallerInt64(i1, i1), NA_FALSE);
  testEqualBool(naSmallerEqualInt64(i1, i2), NA_TRUE);
  testEqualBool(naSmallerEqualInt64(i2, i1), NA_FALSE);
  testEqualBool(naSmallerEqualInt64(i1, i1), NA_TRUE);

  testEqualBool(naEqualUInt64(ui1, ui1), NA_TRUE);
  testEqualBool(naGreaterUInt64(ui1, ui2), NA_TRUE);
  testEqualBool(naGreaterUInt64(ui2, ui1), NA_FALSE);
  testEqualBool(naGreaterUInt64(ui1, ui1), NA_FALSE);
  testEqualBool(naGreaterEqualUInt64(ui1, ui2), NA_TRUE);
  testEqualBool(naGreaterEqualUInt64(ui2, ui1), NA_FALSE);
  testEqualBool(naGreaterEqualUInt64(ui1, ui1), NA_TRUE);
  testEqualBool(naSmallerUInt64(ui1, ui2), NA_FALSE);
  testEqualBool(naSmallerUInt64(ui2, ui1), NA_TRUE);
  testEqualBool(naSmallerUInt64(ui1, ui1), NA_FALSE);
  testEqualBool(naSmallerEqualUInt64(ui1, ui2), NA_FALSE);
  testEqualBool(naSmallerEqualUInt64(ui2, ui1), NA_TRUE);
  testEqualBool(naSmallerEqualUInt64(ui1, ui1), NA_TRUE);
}



void testArithmetic(){
  int64 i01 = naMakeInt64WithDouble(-123456789012345.);
  int64 i02 = naMakeInt64WithDouble(987654321987.);
  int64 i03 = NA_ZERO_64;
  int64 i04 = naMakeInt64WithDouble(-123.);
  int64 i1, i2;
  uint64 ui01 = naMakeUInt64WithDouble(123456789012345.);
  uint64 ui02 = naMakeUInt64WithDouble(987654321987.);
  uint64 ui03 = NA_ZERO_64u;
  uint64 ui2;

  i1 = naNegInt64(i01);
  testEqualInt64(i1, 0x00007048, 0x860ddf79);
  i1 = naNegInt64(naNegInt64(i01));
  testEqualInt64(i1, 0xffff8fb7, 0x79f22087);
  naIncInt64(i01);
  testEqualInt64(i01, 0xffff8fb7, 0x79f22088);
  naDecInt64(i01);
  testEqualInt64(i01, 0xffff8fb7, 0x79f22087);
  naIncInt64(i03);
  testEqualInt64(i03, 0x00000000, 0x00000001);
  naDecInt64(i03);
  testEqualInt64(i03, 0x00000000, 0x00000000);
  naDecInt64(i03);
  testEqualInt64(i03, 0xffffffff, 0xffffffff);
  naIncInt64(i03);
  
  testEqualInt64(i03, 0x00000000, 0x00000000);
  i2 = naAddInt64(i01, i02);
  testEqualInt64(i2, 0xffff909d, 0x6ebb17ca);
  i2 = naSubInt64(i01, i02);
  testEqualInt64(i2, 0xffff8ed1, 0x85292944);
  i2 = naMulInt64(i01, i04);
  testEqualInt64(i2, 0x0035f2d8, 0x68aa5f23);
  i2 = naMulInt64(i02, i04);
  testEqualInt64(i2, 0xffff9183, 0x637132cf);
  i2 = naDivInt64(i01, i02);
  testEqualInt64(i2, 0xffffffff, 0xffffff84);
  i2 = naDivInt64(i01, i04);
  testEqualInt64(i2, 0x000000e9, 0xb2001cdf);
  i2 = naModInt64(i01, i02);
  testEqualInt64(i2, 0xffffff1a, 0x0b49e4fb);
  i2 = naModInt64(i01, i04);
  testEqualInt64(i2, 0xffffffff, 0xffffffac);
  
  naIncUInt64(ui01);
  testEqualUInt64(ui01, 0x00007048, 0x860ddf7a);
  naDecUInt64(ui01);
  testEqualUInt64(ui01, 0x00007048, 0x860ddf79);
  naIncUInt64(ui03);
  testEqualUInt64(ui03, 0x00000000, 0x00000001);
  naDecUInt64(ui03);
  testEqualUInt64(ui03, 0x00000000, 0x00000000);
  naDecUInt64(ui03);
  testEqualUInt64(ui03, 0xffffffff, 0xffffffff);
  naIncUInt64(ui03);
  testEqualUInt64(ui03, 0x00000000, 0x00000000);
  
  ui2 = naAddUInt64(ui01, ui02);
  testEqualUInt64(ui2, 0x0000712e, 0x7ad6d6bc);
  ui2 = naSubUInt64(ui01, ui02);
  testEqualUInt64(ui2, 0x00006f62, 0x9144e836);
  ui2 = naDivUInt64(ui01, ui02);
  testEqualUInt64(ui2, 0x00000000, 0x0000007c);
  ui2 = naModUInt64(ui01, ui02);
  testEqualUInt64(ui2, 0x000000e5, 0xf4b61b05);
  
}



void timeArithmetic(){
  double t;
  int i;
  int64 i64;
  uint64 ui64;
  
  t = getTime();
  for(i=0; i<TESTSIZE; i++){i64 = naNegInt64(randInt64());}
  t = printAndSwapTime("naNegInt64", t);
  for(i=0; i<TESTSIZE; i++){naIncInt64(i64);}
  t = printAndSwapTime("naIncInt64", t);
  for(i=0; i<TESTSIZE; i++){naDecInt64(i64);}
  t = printAndSwapTime("naDecInt64", t);
  for(i=0; i<TESTSIZE; i++){naAddInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naAddInt64", t);
  for(i=0; i<TESTSIZE; i++){naSubInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naSubInt64", t);
  for(i=0; i<TESTSIZE; i++){naMulInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naMulInt64", t);
  for(i=0; i<TESTSIZE; i++){naDivInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naDivInt64", t);
  for(i=0; i<TESTSIZE; i++){naModInt64(randInt64(), randInt64());}
  t = printAndSwapTime("naModInt64", t);

  for(i=0; i<TESTSIZE; i++){naIncUInt64(ui64);}
  t = printAndSwapTime("naIncUInt64", t);
  for(i=0; i<TESTSIZE; i++){naDecUInt64(ui64);}
  t = printAndSwapTime("naDecUInt64", t);
  for(i=0; i<TESTSIZE; i++){naAddUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naAddUInt64", t);
  for(i=0; i<TESTSIZE; i++){naSubUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naSubUInt64", t);
  for(i=0; i<TESTSIZE; i++){naMulUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naMulUInt64", t);
  for(i=0; i<TESTSIZE; i++){naDivUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naDivUInt64", t);
  for(i=0; i<TESTSIZE; i++){naModUInt64(randUInt64(), randUInt64());}
  t = printAndSwapTime("naModUInt64", t);

}



int64 randInt64(){
  return naCastUInt64ToInt64(naMakeUInt64((uint32)rand(), (uint32)rand()));
}
uint64 randUInt64(){
  return naMakeUInt64((uint32)rand(), (uint32)rand());
}
double getTime(){
  struct timeval curtime;
  NATimeZone curtimezone;
  gettimeofday(&curtime, &curtimezone);
  return curtime.tv_sec + curtime.tv_usec / 1000000.;
}
double printAndSwapTime(const char* title, double starttime){
  double t2 = getTime();
  printf("%s took %f s\n", title, t2 - starttime);
  return t2;
}
void printInt64(int64 i){
  printUInt64(naCastInt64ToUInt64(i));
}
void printUInt64(uint64 ui){
  #if !defined NA_TYPE_INT64
    printf("%08x %08x", ui.hi, ui.lo);
  #else
    printf("%016llx", ui);
  #endif
}
void testEqualInt64(int64 i, uint32 goalhi, uint32 goallo){
  testEqualUInt64(naCastInt64ToUInt64(i), goalhi, goallo);
}
void testEqualUInt64(uint64 ui, uint32 goalhi, uint32 goallo){
  #if !defined NA_TYPE_INT64
    if(ui.lo == goallo && ui.hi == goalhi){
      printf("Pass ");
    }else{
      printf("FAIL ");
    }
  #else
    if((ui & 0xffffffff) == goallo && (ui >> 32) == goalhi){
      printf("Pass ");
    }else{
      printf("FAIL ");
    }
  #endif
  printUInt64(ui);
  printf("\n");
}
void testEqualBool(NABool b, NABool goalb){
  if(b == goalb){
    printf("Pass\n");
  }else{
    printf("FAIL\n");
  }
}



int main(void){
//  testMaking();
//  testBinary();
//  testComparison();
//  testArithmetic();
  
//  timeMaking();
//  timeBinary();
  timeArithmetic();
  
  return 0;
}



// Copyright (c) NALib, Tobias Stamm
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the source-code inherently
// dependent on this software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.