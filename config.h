/* config.h.in.  Generated from configure.in by autoheader.  */

/* Set to the canonical name of the target machine */
#define BUILDHOST "Xbox"

/* Define this if you want SDL sound instead of native drivers */
#define BUILD_SDLQUAKE2

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define if you have the dlopen function. */
#undef HAVE_DLOPEN

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define this if you want joystick support to be built */
//#define HAVE_JOYSTICK

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H

/* Define this if you have GL/glext.h */
#undef HAVE_OPENGL_GLEXT

/* Define if you have POSIX threads libraries and header files. */
#undef HAVE_PTHREAD

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H

/* Define this if C symbols are prefixed with an underscore */
#undef HAVE_SYM_PREFIX_UNDERSCORE

/* Define to 1 if you have the <sys/soundcard.h> header file. */
#undef HAVE_SYS_SOUNDCARD_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define if va_copy is available */
#undef HAVE_VA_COPY

/* Define this if you have the xf86dga extension and its Xxf86dga library. */
#undef HAVE_XF86_DGA

/* Define this if you have the xf86vmode extension and its Xxf86vm library. */
#undef HAVE_XF86_VIDMODE

/* Define if __va_copy is available */
#undef HAVE__VA_COPY

/* Name of package */
#define PACKAGE "Quake2x"

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.00"

/* Define this to the path containing the game data (\${prefix}/share/quake2/)
   */
#define PKGDATADIR "D:"

/* Define this to the path containing the dynamic modules
   (\${exec-prefix}/lib/quake2/) */
#define PKGLIBDIR "D:"

/* Define to the necessary symbol if this constant uses a non-standard name on
   your system. */
#undef PTHREAD_CREATE_JOINABLE

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#undef TIME_WITH_SYS_TIME

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
#undef TM_IN_SYS_TIME

/* Define this if you want to use assembler optimised code */
#undef USE_ASM

/* Define if va_list is an array */
#undef VA_LIST_IS_ARRAY

/* Version number of package */
#define VERSION "1.00"

/* Define to 1 if the X Window System is missing or not being used. */
#undef X_DISPLAY_MISSING

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `int' if <sys/types.h> doesn't define. */
#undef gid_t

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
#define inline __inline

/* Define to `unsigned' if <sys/types.h> does not define. */
#undef size_t

/* Define to `int' if <sys/types.h> doesn't define. */
#define uid_t int
