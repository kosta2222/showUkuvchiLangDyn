#include "vm.h"
#include "map.h"
#ifdef __cplusplus
extern "C"
{
#endif

  /**
  Замечания. В тексте N целые числа, R - вещественные
   */
  /**
   * Работаем с данными элементами из ObjectHeap.cpp
   */
  extern Variable* m_objectMap[];
  extern struct map * vTable;

  /** вызвать пользовательскую функцию
   \param [in] argc количество аргументов
   \param [in] массив целых аргументов
   \return значение
   */
  //Variable
  //call_user (int funcid, int argc, float *argv)
  //{
  //  Variable ret;
  //  int i;
  //
  //  if (funcid == 0)
  //    {
  //      printf ("Called user function 0 => stop.\n");
  //      return ret;
  //    }
  //  if (funcid == 1)
  //    {
  //      ret = cos (argv[-1]);
  //    }
  //  if (funcid == 2)
  //    {
  //      ret = sin (argv[-1]);
  //    }
  //  printf ("Called user function %d with %d args:", funcid, argc);
  //  for (i = 0; i < argc; i++)
  //    {
  //      printf (" %f", argv[i]);
  //
  //    }
  //  printf ("\n");
  //  return ret;
  //
  //}

  /**
  Отпечатка инструкции
   */
  typedef struct
  {
    /** имя инструкции*/
    char name[20];
    /** количество аргументов*/
    u1 nargs;
  } VM_INSTRUCTION;

  /**Массив данных о каждой инструкции */

  static VM_INSTRUCTION vm_instructions[] = {
    { "noop", 0},
    { "iadd", 0},
    { "isub", 0},
    { "imul", 0},
    {"idiv", 0},
    {"irem", 0},
    {"ipow", 0},
    { "ilt", 0},
    { "ieq", 0},
    { "br", 1},
    { "brt", 1},
    { "brf", 1},
    { "iconst", 1},
    { "load", 1},
    { "gload", 1},
    { "store", 1},
    { "gstore", 1},
    { "print", 0},
    { "pop", 0},
    { "call", 2},
    { "ret", 0},
    {"store_result", 1},
    {"load_result", 0},
    {"invoke_in_vm", 0},
    {"createstring", 0},
    {"newarray", 0},
    {"fastore", 0},
    {"faload", 0},
    {"dup", 0},
    {"astore", 0},
    {"aload", 0},
    {"invoke", 0},
    { "stop", 0}
  };
  /**Инициализация контекста */
  static void vm_context_init (Context *ctx, u4 ip, u1 nlocals);

  /** инициализируем виртуальную машину */
  void
  vm_init (VM *vm, char* entryFuncName, u1 nglobals)
  {
    u1 * code = (u1*) mapGet (entryFuncName, vTable);

    u4 code_size = sizeof (code);
    vm->code = code;
    vm->code_size = code_size;
    vm->globals = (Variable*) calloc (nglobals, sizeof (Variable));
    vm->nglobals = nglobals;
  }

  /** освободить память из под виртуальной машины */
  void
  vm_free (VM *vm)
  {
    free (vm->globals);
    free (vm);
  }

  /** создать виртуальную машину */
  VM *
  vm_create (char* entryFuncName, u1 nglobals)
  {
    VM *vm = (VM*) calloc (1, sizeof (VM));
    vm_init (vm, entryFuncName, nglobals);
    return vm;
  }

  /**
  Выполняе байт-коды
  \param[in] vm  экземпляр Vm
  \param[in] startip с какого байта выполнять опкоды
  \param[in] trace если трассировка?
  \param[in] numberFromLocalsAsPar использовать ли число из локальных переменных
  \return число из локальных переменных
   */
  void
  vm_exec (VM *vm, bool trace)
  {


    u4 ip;
    u4 sp;
    u4 callsp;

    f4 a = 0;
    f4 b = 0;

    ip = 0;
    sp = -1;
    callsp = -1;

    i2 addr;
    u1 n_locNum;
    u1 offset;



    u4 opcode = vm->code[ip];
    while (opcode != STOP)
      {
        if (trace)
          {
            vm_print_instr (vm->code, ip);

          }

        ip++; // переходим на следующую инструкцию(!) или аргумент(!)
        switch (opcode)
          {
          case NOOP:
            break;
          case FADD:
            b = vm->stack[sp--].floatValue;
            a = vm->stack[sp--].floatValue;
            vm->stack[++sp].floatValue = a + b;
            break;
          case FSUB:
            b = vm->stack[sp--].floatValue;
            a = vm->stack[sp--].floatValue;
            vm->stack[++sp].floatValue = a - b;
            break;
          case FMUL:
            b = vm->stack[sp--].floatValue;
            a = vm->stack[sp--].floatValue;
            vm->stack[++sp].floatValue = a * b;
            break;
          case FDIV:
            b = vm->stack[sp--].floatValue;
            a = vm->stack[sp--].floatValue;
            vm->stack[++sp].floatValue = a / b;
            break;
            // \todo должна быть поддержка N в компиляторе
            //        case IADD:
            //          b = vm->stack[sp--].intValue;
            //          a = vm->stack[sp--].intValue;
            //          vm->stack[++sp].floatValue = a + b;
            //          break;
            //        case ISUB:
            //          b = vm->stack[sp--].intValue;
            //          a = vm->stack[sp--].intValue;
            //          vm->stack[++sp].intValue = a - b;
            //          break;
            //        case IMUL:
            //          b = vm->stack[sp--].intValue;
            //          a = vm->stack[sp--].intValue;
            //          vm->stack[++sp].intValue = a * b;
            //          break;
            //        case IDIV:
            //          b = vm->stack[sp--].intValue;
            //          a = vm->stack[sp--].intValue;
            //          vm->stack[++sp].intValue = a / b;
            //          break;

            // конец должна быть поддержка
          case IREM:// получить остаток от деления
            b = vm->stack[sp--].floatValue;
            a = vm->stack[sp--].floatValue;
            vm->stack[++sp].floatValue = (int) a % (int) b;
            break;

          case FPOW:// возведение в степень
            b = vm->stack[sp--].floatValue;
            a = vm->stack[sp--].floatValue;
            vm->stack[++sp].floatValue = pow (a, b);
            break;
          case DUP:// дублирование вершины стека
            vm->stack[sp + 1] = vm->stack[sp];
            sp += 1;
            break;
            // создать массив по длине
          case NEWARRAY:
            {
              vm->stack[sp + 1].object = createNewArray (1, (int) vm->stack[sp].floatValue);
              sp += 1;
              break;
            }
          case FALOAD: // загрузить значение с массива на стек
            {
              // arrayref в стеке - обеспечивается текстом программы getObject!
              Object heapKey = vm->stack[sp - 2].object;
              // берем нужный индекс со стека
              u4 index = (u4) (vm->stack[sp].floatValue);
              //  записываем элемент в стек - значение массива из кучи
              vm->stack[(u4) (sp - 1)] = m_objectMap[(u4) heapKey.heapPtr][index];
              sp -= 1;
              break;
            }
          case FASTORE: // загрузить значение со стека в массив
            {
              // arrayref в стеке - обеспечивается текстом программы getObject!
              Object heapKey = vm->stack[sp - 2].object;
              // берем нужный индекс со стека
              u4 index = (u4) (vm->stack[sp].floatValue);
              // работаем с кучей - записываем элемент из стека в массив
              m_objectMap[heapKey.heapPtr][(u4) index] = vm->stack[sp];
              sp -= 3;
              break;

            }
          case FEQ:
            {
              b = vm->stack[sp--].floatValue;
              a = vm->stack[sp--].floatValue;
              vm->stack[++sp].floatValue = (a < b) ? true : false;
              break;
            }

          case BR:
            {
              ip = geti2 (&vm->code[ip]);
              break;
            }
          case BRT:
            {
              addr = geti2 (&vm->code[ip]);
              if ((u1) vm->stack[sp--].floatValue == true) ip = addr;
              break;
            }
          case BRF:
            {
              addr = geti2 (&vm->code[ip]);
              if ((u1) vm->stack[sp--].floatValue == false) ip = addr;
              break;
            case FCONST:

              vm->stack[++sp].floatValue = *((float*) &vm->code[ip]);

              ip += 3;
              ip++;
              break;

            case LOAD:
              offset = vm->code[ip++];
              vm->stack[++sp] = vm->call_stack[callsp].locals[offset];
              break;
            case GLOAD:
              addr = vm->code[ip++];
              vm->stack[++sp] = vm->globals[addr];
              break;
            case STORE:
              offset = vm->code[ip++];
              vm->call_stack[callsp].locals[offset] = vm->stack[sp--];
              break;
            case GSTORE:
              addr = vm->code[ip++];
              vm->globals[addr] = vm->stack[sp--];
              break;
              // Отпечать R из локальых переменных
            case PRINT:
              {
                int numberFromLocals = vm->code[ip++];
                Variable value = vm->call_stack[callsp].locals[numberFromLocals];
                printf ("print float Value: %f\n", value.floatValue);


                break;
              }
            case POP:
              --sp;
              break;
              //        case 25 ... 25 + 15:
              //          {
              //            int argc = (int) vm->stack[sp--];
              //            float argv[argc];
              //            for (int i = 0; i < argc; i++)
              //              {
              //                argv[i] = vm->stack[sp--];
              //
              //              }
              //            a = call_user (opcode - 25, argc, argv);
              //            if (argc != 0)
              //              {
              //                vm->registrThatRetFunc = a;
              //
              //              }



              break;

            case INVOKE:
              {
                u1 strArgLen = *((u1*) (&vm->code[ip]));
                printf ("strArg len:%d\n", strArgLen);
                u1* strArgName = (u1*) malloc (strArgLen);
                ip++;
                for (int i = 0; i < strArgLen; i++)
                  {
                    strArgName[i] = vm->code[ip + i];

                  }
                printf ("%s\n", strArgName);
                if (strcmp ((char*) strArgName, "TestClass_medthot") == 0)
                  {
                    printf ("yes\n");

                  }
                printf ("strArgName len: %d Testclass_methot len: %d\n", strlen ((char*) strArgName), strlen ((char*) "TestClass_medthot"));

                ip += strArgLen;
                break;
              }

            case CALL:
              {

                addr = geti2 (&vm->code[ip++]);
                u1 nargs = vm->code[ip++];
                u1 nFirstarg = sp - nargs + 1;
                ++callsp;

                vm_context_init (&vm->call_stack[callsp], ip, 26);

                for (int i = 0; i < nargs; i++)
                  {
                    vm->call_stack[callsp].locals[i] = vm->stack[nFirstarg + i];
                  }
                sp -= nargs;
                ip = addr;
                break;
              }
            case RET:
              {
                ip = vm->call_stack[callsp].returnip;
                callsp--;
                break;
              }
            case STORE_RESULT:
              {
                n_locNum = vm->code[ip++];
                vm->registrThatRetFunc = vm->call_stack[callsp].locals[n_locNum];
                break;
              }
            case LOAD_RESULT:
              {
                vm->stack[++sp] = vm->registrThatRetFunc;
                break;
              }


            default:
              {
                printf ("invalid opcode: %d at ip=%d\n", opcode, (ip - 1));
                exit (1);
              }

            }






          }

        if (trace) vm_print_stack (vm->stack, 3);
        opcode = vm->code[ip];


      }
    if (trace)
      {
        vm_print_data (vm->globals, vm->nglobals);
      }
    printf ("Heap:\n");
    dumpHeap ();
  }

  static void
  vm_context_init (Context *ctx, u4 ip, u1 nlocals)
  {
    if (nlocals > DEFAULT_NUM_LOCALS)
      {
        fprintf (stderr, "too many locals requested: %d\n", nlocals);
      }
    ctx->returnip = ip;
  }

  void
  vm_print_instr (unsigned char *code, u4 ip)
  {

    u4 opcode = code[ip];
    VM_INSTRUCTION *inst = &vm_instructions[opcode];
    switch (inst->nargs)
      {
      case 0:
        printf ("%04d:  %-20s", ip, inst->name);
        break;
      case 1:
        if (opcode == FCONST)
          {

            printf ("%04d: %-10s %f", ip, "iconst", *((float*) &(code[ip + 1])));
          }
        else
          {
            printf ("%04d:  %-10s%-10d", ip, inst->name, code[ip + 1]);
          }


        break;
      case 2:
        printf ("%04d:  %-10s%d,%10d", ip, inst->name, code[ip + 1], code[ip + 2]);
        break;
      case 3:
        printf ("%04d:  %-10s%d,%d,%-6d", ip, inst->name, code[ip + 1], code[ip + 2], code[ip + 3]);

        break;
      }
  }

  void
  vm_print_stack (Variable *stack, u1 count)
  {

    printf ("stack=[");
    for (int i = 0; i < count; i++)
      {

        printf (" %f", stack[i].floatValue);
      }
    printf (" ]\n");




  }

  void
  vm_print_data (Variable *globals, u1 count)
  {
    printf ("Data memory:\n");
    for (int i = 0; i < count; i++)
      {

        printf ("%04d: %f\n", i, globals[i]);
      }
  }

  void
  vm_print_locals (float *locals, int count)
  {
    printf ("Locals memory:\n");
    for (int i = 0; i < count; i++)
      {

        printf ("%04d: %f\n", i, locals[i]);
      }
  }

  int
  main ()
  {
    FILE * ptrFile = fopen ("code.bin", "rb");

    if (ptrFile == NULL)
      {
        fputs ("Ошибка файла", stderr);
        exit (1);
      }

    // определяем размер файла
    fseek (ptrFile, 0, SEEK_END); // устанавливаем позицию в конец файла
    long lSize = ftell (ptrFile); // получаем размер в байтах
    rewind (ptrFile); // устанавливаем указатель в конец файла

    u1 * opcodeCharBuffer = (u1*) malloc (sizeof (u1) * lSize); // выделить память для хранения содержимого файла
    if (opcodeCharBuffer == NULL)
      {
        fputs ("Ошибка памяти", stderr);
        exit (2);
      }

    size_t result = fread (opcodeCharBuffer, 1, lSize, ptrFile); // считываем файл в буфер
    if (result != lSize)
      {
        fputs ("Ошибка чтения", stderr);
        exit (3);
      }


    // завершение работы
    fclose (ptrFile);


    if (!parseByteCodeToMap (opcodeCharBuffer))
      {
        fputs ("Не разобрал файл в виртуальную таблицу", stderr);
      }


    VM *vm = vm_create ("main", 0);
    vm_exec (vm, true);
    vm_free (vm);
    free (opcodeCharBuffer);
    mapClose (vTable);

    return 0;
  }
#ifdef __cplusplus
}
#endif
