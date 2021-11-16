/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_ATOMIC_INCLUDED
#define ETL_ATOMIC_INCLUDED

#include "platform.h"

#if ETL_CPP11_SUPPORTED && ETL_USING_STL
  #include "atomic/atomic_std.h"
  #define ETL_HAS_ATOMIC 1
#elif defined(ETL_COMPILER_ARM5)
  #include "atomic/atomic_arm.h"
  #define ETL_HAS_ATOMIC 1
#elif defined(ETL_COMPILER_ARM6)
  #include "atomic/atomic_arm.h"
  #define ETL_HAS_ATOMIC 1
#elif defined(ETL_COMPILER_GCC) || (defined(ETL_COMPILER_CLANG) && defined(__ARM_ARCH))
  #include "atomic/atomic_gcc_sync.h"
  #define ETL_HAS_ATOMIC 1
#else
  #define ETL_HAS_ATOMIC 0
#endif

#endif
