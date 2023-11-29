#ifndef SQTREE_H /* just little different to bs tree */
#define SQTREE_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef char *qtr_K;
    typedef void *qtr_V;

    typedef NMap SQTree;
    typedef struct _sqn SQNode;

#include "String/String.h"
#include "NMap.h"

    struct _sqn
    {
        SNode *rightRoot;
        SNode *leftRoot;
        qtr_K key;
        qtr_V value;
    };

    struct _sqtr
    {
        SNode *rightRoot;
        SNode *leftRoot;
    };

    void sqtr_setEntry(NMap map, qtr_K key, qtr_V value);

    qtr_V sqtr_getValue(NMap map, qtr_K key);


#ifdef __cplusplus
}
#endif
#endif