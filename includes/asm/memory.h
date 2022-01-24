#ifndef __MEMORY__
#define __MEMORY__
#define memcpy(dest,src,n) ({ \
    void* _res=dest;\
    __asm__("cld;rep;movsb" \
            ::"D" ((long)dest),"S" ((long)src),"c" ((long)n):); \
_res;\
})
#endif
