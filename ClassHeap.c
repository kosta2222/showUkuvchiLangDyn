/**
 * Место где я собираюсь хранить 'определение' класса (пока только функции т.е это vtable)
 */

#include "map.h"
#include "types.h"
#include "vm.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"
#ifdef __cplusplus
extern "C"
{
#endif


  struct map * vTable;
  static u2 method_count;
  static u1 * p;

  bool
  parseMethods ()
  {
    vTable=mapNew();
    int i;
    for (i = 0; i < method_count; i++)
      {
        // длина имени функции
        u1 strLen = *((u1*) p);
        p += 1;
        // определяем имя функции
        char* strFuncName = (char*) malloc (strLen + 1);
        strcpy (strFuncName, p);
        p += strLen;
        // длина байт-кода
        u2 len = getu2 (p);
        p += 2;
        // выделение памяти под байт-код
        u1 * byteCodeForMap = (u1*) malloc (len);
        memcpy (byteCodeForMap, p, len);

        p += len;

        mapAdd(strFuncName,byteCodeForMap,vTable);



      }
    return true;
  }

  bool
  parseByteCodeToMap (u1 * bytecode)
  {

    p = bytecode;
    method_count = getu2 (p);
    p += 2;
    printf ("method count:%d\n", method_count);
    if (method_count > 0)
      {
        parseMethods ();

      }


    return true;

  }


#ifdef __cplusplus
}
#endif