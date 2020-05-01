
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.


// This file contains configurations of your installation of NALib. There are
// some few options you can compile NALib with. These options are implemented
// as preprocessor macros such that you can define them with prefix headers
// when you need them in your special project.
//
// Note that preprocessor flags with values usually follow this scheme:
// PREPROCESSOR_MACRO=value
// Without spaces at the equal sign.
//
// NAConfiguration.h is included at the beginning of the NABase.h file.
// Do not include it anywhere else.
//
// Note that there is no possibility to platform-independently provide a
// clean configuration structure without the need to configure either the
// NALib, installation or your compiler in one or another way. The solution
// with this file allows you to share NALib with a default implementation
// but which can be altered project-wise by as simple compiler prefix macro
// definition.



// ////////////////////////////////
// NAInt bit width
// ////////////////////////////////

// Usually, an NAInt has the same bit width as an address in the current
// configuration, meaning 32 bits on a 32 bit system and 64 bits on a 64 bit
// system. But there may be instances, where an NAInt shall have a specific
// size.
//
// Choose from the following values:
// 0   NAInt has bit width defined by NA_SYSTEM_ADDRESS_BITS
// 32  NAInt has a 32 bit width.
// 64  NAInt has a 64 bit width.
//
// Default is 0

#ifndef NA_PREFERRED_NAINT_BITS
  #define NA_PREFERRED_NAINT_BITS 0
#endif


// ////////////////////////////////
// OpenGL support
// ////////////////////////////////

// If the following macro is set to 1, NALib compiles with OpenGL support.
//
// If so, you MUST link your program to the appropriate library:
// Windows: WinAPI (always available) and opengl32.lib
// Macintosh: Cocoa
//
// The #include directives can be found on top of the NAUI.h file
//
// Default is 0

#ifndef NA_CONFIG_COMPILE_OPENGL
  #define NA_CONFIG_COMPILE_OPENGL 0
#endif



// ////////////////////////////////
// GUI application support
// ////////////////////////////////

// If the following macro is set to 1, NALib compiles with GUI support.
//
// If so, you MUST link your program to the appropriate library:
// Windows: WinAPI (always available) and ComCtl32.lib, see next macro.
// Macintosh: Cocoa
//
// Default is 0

#ifndef NA_CONFIG_COMPILE_GUI
  #define NA_CONFIG_COMPILE_GUI 0
#endif



// On windows, GUI programming is done with WINAPI. This would by default use
// the version 5 of common controls which looks very outdated. As usually,
// a newer look is preferred, the default in NALib is to load version 6.
// Unfortunately, this requires you to link your binary to ComCtl32.lib.
// Set this macro to 0 if you want the old look or not link to the library.
//
// If your code does not compile for windows or NA_CONFIG_COMPILE_GUI is 0,
// this macro has no effect.
//
// Default is 1

#ifndef NA_CONFIG_USE_WINDOWS_COMMON_CONTROLS_6
  #define NA_CONFIG_USE_WINDOWS_COMMON_CONTROLS_6 1
#endif



// ////////////////////////////////
// Aligned memory allocation
// ////////////////////////////////

// Usually, aligned memory can be created in unix like systems using several
// methods. See the definitions of the following macros in NAMemory.h file
// for more information.
//
// - NA_MEMALIGN_USE_CUSTOM
// - NA_MEMALIGN_USE_ALIGNED_ALLOC (needs full C11 support)
// - NA_MEMALIGN_USE_POSIX
//
// Default is POSIX

#ifndef NA_MEMALIGN
  #define NA_MEMALIGN NA_MEMALIGN_USE_POSIX
#endif



// ////////////////////////////////
// Runtime memory pools
// ////////////////////////////////

// Define the size of a core memory pool part. This size is used for the
// NALib runtime system, more precisely allocation with naNew, as well as
// for garbage collection.
//
// If you set this macro to 0, one memory page size will be used.
//
// Turns out, on most systems, one pagesize is far too small to result in
// good speed improvements for naNew. A large enough custom bytesize can
// result in up to 2 times faster allocation and deallocation.
//
// Default is (1 << 16)

#ifndef NA_POOLPART_BYTESIZE
  #define NA_POOLPART_BYTESIZE (1 << 16)
#endif

// Define if memory pools shall be cleaned up aggressively.
//
// Memory pools do organize themselves by allocating and deallocating large
// chunks of memory. Upon startup, there is no memory allocated whatsoever.
// This should of course be the same in the end when the runtime system
// stops. Nontheless, it might be very helpful to always keep one chunk per
// type in memory to prevent allocating and deallocating the same (first)
// chunk over and over again.
//
// With this flag, the programmer defines, if that last chunk shall be freed
// aggressively, meaning as soon as no more memory is needed for that type,
// all related pools get deallocated. If set to 0, the last chunk stays in
// memory up until the runtime system stops.
//
// This flag is important for systems with very small memory as well as for
// performance testing: When clenaing up aggressively, the runtime can be
// considerably slower. Otherwise, one big chunk of memory always stays
// ready resulting in very high performance, especially for small testing
// cases. But this might not reveal potential performance losses when using
// the tested functionality in real life.
//
// Default is 0

#ifndef NA_MEMORY_POOL_AGGRESSIVE_CLEANUP
  #define NA_MEMORY_POOL_AGGRESSIVE_CLEANUP 0
#endif

// Defines when the temp garbage collection starts collecting automatically.
//
// With this macro, you can define, if and when the garbage collection should
// perform an automatic collection of temporarily allocated memory. If you
// set this value to 0, no automatic collection will be performed ever. You
// need to call naCollectGarbage manually.
//
// If this macro is set to a non-zero value, it will fire during a call to
// naMallocTmp but only when the sum of all previously temporary allocated
// bytes had become higher than this number in the previous call to
// naMallocTmp. If so, the garbage will first be collected and then the new
// bytes will be allocated and returned.
//
// The default value is 1000000 (1 Million) when no GUI is in use and 0 when
// a GUI is in use. When a GUI is in use, the event loop of NAApplication
// calls naCollectGarbage automatically before executing any event.

#ifndef NA_GARBAGE_TMP_AUTOCOLLECT_LIMIT
  #if NA_CONFIG_COMPILE_GUI == 0
    #define NA_GARBAGE_TMP_AUTOCOLLECT_LIMIT 1000000
  #else
    #define NA_GARBAGE_TMP_AUTOCOLLECT_LIMIT 0
  #endif
#endif



// ////////////////////////////////
// Buffers
// ////////////////////////////////

// Define the size of a buffer part used for NABuffer:
//
// With the following macro, you can define, what the default byte size of
// one buffer part shall be. The value 0 denotes that the memory page size
// will be used. Custom values typically are powers of 2, for example 4096.
//
// The default is 0

#ifndef NA_BUFFER_PART_BYTESIZE
  #define NA_BUFFER_PART_BYTESIZE 0
#endif



// String caching
//
// When set to 1, the NAString structure always tries to create a cached
// version of its content. This is really only useful when debugging as the
// cached version will be stored within the NAString structure. With NDEBUG
// defined though, nothing will be stored and it is just a waste of time.
// Nontheless as caching will take a considerable amount of time even in
// debugging, you may choose to set NA_STRING_ALWAYS_CACHE to 0 even for
// debugging right here.
//
// The default is that strings will always be cached when running in debug
// mode but will not be cached if NDEBUG is defined.

#ifndef NA_STRING_ALWAYS_CACHE
  #ifndef NDEBUG
    #define NA_STRING_ALWAYS_CACHE 1
  #else
    #define NA_STRING_ALWAYS_CACHE 0
  #endif
#endif



// ////////////////////////////////
// Mutexes on Windows
// ////////////////////////////////

// There are two ways to implement mutexes on windows: "Critical section" or
// "Mutex". Both implementations work but some performence issues have been
// observed with the "Mutex" implementation.
//
// Set this macro to 1 if you want to use the "critical section" or set it to
// 0 if you want to use "Mutex".
//
// Default is 1

#ifndef NA_WINDOWS_MUTEX_USE_CRITICAL_SECTION
  #define NA_WINDOWS_MUTEX_USE_CRITICAL_SECTION 1
#endif



// ////////////////////////////////
// Mathematical, chemical and physical constants
// ////////////////////////////////

// The constants in NAMathConstants.h are constants from physics, chemistry,
// mathematics, all mixed together. These constants are directly from NIST:
// http://physics.nist.gov/constants
//
// With the following macro, you define, what year of the CODATA you would
// like to use. Possible values are 2006, 2010, 2014 and 2018.
//
// By default, NALib uses the latest setting.

#ifndef NA_NIST_CODATA_YEAR
  #define NA_NIST_CODATA_YEAR 2018
#endif



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
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.