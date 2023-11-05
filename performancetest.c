#include <stdio.h>
#include <math.h>

#include "Utils/StringIterator/StringIterator.h"
#include "Utils/debug.h"
#include "Utils/String/String.h"
#include <unistd.h>
timevar(total_parse);
timevar(dump);

void main()
{

    StringIterator *itr = itr_loadFromLargeFile("bigtest.json");

    timestart(total_parse);

    AbstractValue val;
    itr_getAbstract(itr, &val);

    timepush(total_parse);

    timestart(dump);
    itr_dump(&val, "out.json");
    timepush(dump);

    //itr_clearAbstract(&val);


    /*printf("Parsed Json:\n");
    vect_foreachBuff(obj, JsonValue, value)
    {
        itr_printObject(value);
    }*/
    printf("error: %s\n", itr_geterr());

    itr_closeLargeFile(itr);

    return;
    // char ch;
    // if(_itr_getchar(&itr, &ch) == 0)
    //     printf("error: %s\n", itr_geterr());
    // printf("%c\n", ch);
    itr_getAbstract(itr, &val);
    printf("type: %d\n", val.valueType);
    if (val.valueType > 33)
        printf("string: %s\n", val.valueData.string_val);

    printf("long: %lld\n", val.valueData.slong_val);
    printf("int: %d\n", val.valueData.sint_val);
    printf("double: %.2F\n", val.valueData.double_val);
    printf("char: %c\n", val.valueData.char_val);
}
