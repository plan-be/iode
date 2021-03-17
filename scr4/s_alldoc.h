#ifndef _ALLOCDOC_

#define _ALLOCDOC_

#include "s_strs.h" // JMP 14/3/2017

#ifdef ALLOCDOCOFF

// Standard version (w/o alloc info)
#define SCR_malloc(abcdef__aze) SCR_malloc_orig(abcdef__aze)
#define SCR_realloc(ptr, elabcdef__, oldcount, newcount) SCR_realloc_orig((ptr), (elabcdef__), (oldcount), (newcount))
#define SW_nalloc(abcdef__aze) SW_nalloc_orig(abcdef__aze)
#define SW_nrealloc(abcdef__aze, oldcount, newcount) SW_nrealloc_orig((abcdef__aze), (oldcount), (newcount))

#else

// New version (default) with a file[line] for each (alloc / realloc)
#define SCR_malloc(abcdef__) SCR_malloc_doc((abcdef__), __FILE__, __LINE__)
#define SCR_realloc(ptr, sizeel__, oldcount, newcount) SCR_realloc_doc(ptr, sizeel__, oldcount, newcount, __FILE__, __LINE__)
#define SW_nalloc(abcdef__) SW_nalloc_doc((abcdef__), __FILE__, __LINE__)
#define SW_nrealloc(ptr, oldcount, newcount) SW_nrealloc_doc(ptr, oldcount, newcount, __FILE__, __LINE__)
#endif

#endif


