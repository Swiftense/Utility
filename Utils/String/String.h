#ifndef STRING_H /* just little different to bs tree */
#define STRING_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <malloc.h>
#include <stdlib.h>

#include "../null.h"
    ;
    typedef struct _str XString;

    ;
    struct _str
    {
        char *_str;
        int _size;
    };

#define xstrcreate()                   \
    {                                  \
        ._str = malloc(24), ._size = 0 \
    }

#define xstrcreate()                   \
    {                                  \
        ._str = malloc(24), ._size = 0 \
    }

#define xstrcreateft(t) ({           \
    XString str = xstrcreate();      \
    xstrappends((XString *)&str, t); \
    str;                             \
})

#define xstrappend(str, ch)                                                 \
    {                                                                       \
        if ((str._size) == malloc_usable_size(str._str))                    \
            str._str = realloc(str._str, malloc_usable_size(str._str) * 2); \
        *(str._str + (str._size++)) = ch;                                   \
    }

#define xstrappendToBuff(str, ch)                                              \
    {                                                                          \
        if ((str->_size) == malloc_usable_size(str->_str))                     \
            str->_str = realloc(str->_str, malloc_usable_size(str->_str) * 2); \
        *(str->_str + (str->_size++)) = ch;                                    \
    }

#define xstrserialize(str) ({xstrappend(str, '\00'); str._str; })

#define xstrgrow(str)                                \
    if ((str._size) == malloc_usable_size(str._str)) \
        str._str = realloc(str._str, malloc_usable_size(str._str) * 2);

    /*__attribute__((__nonnull__(1, 2)))*/ extern void xstrappends(XString *str, char *stra);
    /*__attribute__((__nonnull__(1)))*/ extern void xstrappendc(XString *str, char stra);

    extern void xstrappendi(XString *str, int i);
    extern void xstrappendl(XString *str, long long l);

    extern void xstrappendui(XString *str, unsigned int i);
    extern void xstrappendul(XString *str, unsigned long long l);

    extern void xstrappendf(XString *str, float f);
    extern void xstrappendfp(XString *str, float f, int precision);
    extern void xstrappendd(XString *str, double d);
    extern void xstrappenddp(XString *str, double f, int precision);

    extern void xstrappendmemaddr(XString *str, unsigned long long addr);
    /// @brief supports %d  %u  %lld %llu  %lF  %s  %c  %p
    extern void xstrappendformat(XString *str, char *format, ...);

    extern char *strcreaterandomlowercase(int lengtha, int lengthb);
    extern char *strcreaterandom(int lengtha, int lengthb);
    extern int strstartswith(char *str, char *start);
    extern int strend(char *str);
    extern unsigned long long strhash(char *str);
    extern int strendswith(char *str, char *end);
    extern signed int strequalsmo(char *str, char opts, ...);
    extern int strequals(char *str1, char *str2);
    extern char *strremoveAdStart(char *str, char *start);
    /// @brief concats <str1> and <str2>
    /// @return result, allocated using malloc
    extern char *strccat(char *str1, char *str2);
    __attribute__((__nonnull__(1, 3))) extern char *strccatph(char *str1, char ph, char *str2);
    /// @brief returns a string splited before <i> strsubbf("hello.world", "."); will return "hello"
    /// @return result, allocated using malloc
    extern char *strsubbf(char *str1, char *i);
    extern char *strremovecAdStart(char *str, char ch);
    /// @brief searches for character sequence <i> in <str1>
    /// returns substring abfter the searched sequence or null if there was no occurance
    extern char *strsearch(char *str1, char *i);
    extern char *strsearchbf(char *str1, char *i);
#ifdef __cplusplus
}
#endif
#endif