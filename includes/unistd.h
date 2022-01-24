#ifndef _UNISTD_H
#define _UNISTD_H
#define __NR_setup	0
#define __NR_exit   1
#define __NR_fork	2

#define syscall0(type,name) \
    type name(){\
        long __res;\
        __asm__ volatile("int $0x80" \
        :"=a"(__res) \
        :"0"(__NR_##name) \
        );\
        if (__res>=0) return (type) __res;\
        return -1;\
    }

#define syscall1(type,name,atype,a) \
    type name(atype a){\
        long __res;\
        __asm__ volatile("int $0x80" \
        :"=a"(__res) \
        :"0"(__NR_##name),"b"((long)(a)) \
        );\
        if (__res>=0) return (type) __res;\
        return -1;\
    }

#define syscall2(type,name,atype,a,btype,b) \
    type name(){\
        long __res;\
        __asm__ volatile("int $0x80" \
        :"=a"(__res) \
        :"0"(__NR_##name),"b"((long)(a)),"c"((long)(b)) \
        );\
        if (__res>=0) return (type) __res;\
        return -1;\
    }

#define syscall3(type,name,atype,a,btype,b,ctype,c) \
    type name(){\
        long __res;\
        __asm__ volatile("int $0x80" \
        :"=a"(__res) \
        :"0"(__NR_##name),"b"((long)(a)),"c"((long)(b)),"d"((long)(c)) \
        );\
        if (__res>=0) return (type) __res;\
        return -1;\
    }


#endif
