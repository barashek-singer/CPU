#ifndef HZ_H_INCLUDED

#define HZ_H_INCLUDED

//короче каждая команда байт - [RR][OPCODE]
//                              /       \                                      ^-^     
//                             /         \                                     ^-^
//                          Mode        Operation Code 
//                            
//            Mode    
//                                                
// Register-bit     RAM-bit
// 0 - const        0 - not RAM 
// 1 - register     1 - RAM 

typedef uint8_t byte_t;

const byte_t REGISTER_BIT = (byte_t)(1 << 7);
const byte_t RAM_BIT      = (byte_t)(1 << 6);
const byte_t OPCODE_BITS  = (byte_t)((1 << 6) - 1);

enum cmd_t{
    UNKNOWN_CMD = -1, //111111 ???
//OPCODE:
    PUSH,  //000000
    POP,   //000001
    ADD,   //000010
    SUB,   //000011
    MUL,   //000100
    DIV,   //000101
    IN,    //000110
    OUT,   //000111 
    HLT,   //001000
    JMP,   //001001
    JA,    //001010
    JAE,   //001011
    JB,    //001100
    JBE,   //001101
    JE,    //001110
    JNE,   //001111
    MAX_NUM_CMD
};

enum cpu_err_t {
    CPU_OK            =  0,
    CPU_UNKNOWN_CMD   =  1,
    CPU_TOO_MUCH_ARGS =  2,
    CPU_SCANF         =  3,
    CPU_FOPEN         =  4,
    CPU_FCLOSE        =  5,
    CPU_UNEXPECTED_IP =  6,
    CPU_OOM           =  7,
    CPU_DIV_BY_ZERO   =  8,
    CPU_STACK         =  9,
    CPU_BAD_ARG       = 10,
    CPU_EMPTY_LABEL   = 11,
    CPU_UNKNOWN_ERROR
};

const size_t MAX_CMD_LEN = 5;

const char commands[MAX_NUM_CMD][MAX_CMD_LEN] = {"PUSH",
                                                 "POP",
                                                 "ADD",
                                                 "SUB",
                                                 "MUL",
                                                 "DIV",
                                                 "IN",
                                                 "OUT",
                                                 "HLT",
                                                 "JMP",
                                                 "JA",
                                                 "JAE",
                                                 "JB",
                                                 "JBE",
                                                 "JE",
                                                 "JNE",
                                                };

void CpuShowError(cpu_err_t err);

void CpuShowError(cpu_err_t err) {
    switch ((int)err) {
        case CPU_UNKNOWN_CMD:
            printf("CPU_UNKNOWN_CMD\n");
            break;
        case CPU_TOO_MUCH_ARGS:
            printf("CPU_TOO_MUCH_ARGS\n");
            break;
        case CPU_SCANF:
            printf("CPU_SCANF\n");
            break;
        case CPU_FOPEN:
            printf("CPU_FOPEN\n");
            break;
        case CPU_FCLOSE:
            printf("CPU_FCLOSE\n");
            break;
        case CPU_UNEXPECTED_IP:
            printf("CPU_UNEXPECTED_IP\n");
            break;
        case CPU_OOM:
            printf("CPU_OOM\n");
            break;
        case CPU_DIV_BY_ZERO:
            printf("CPU_DIV_BY_ZERO\n");
            break;
        case CPU_STACK:
            printf("CPU_STACK\n");
            break;
        case CPU_UNKNOWN_ERROR:
            printf("CPU_UNKNOWN_ERROR\n");
            break;
        case CPU_BAD_ARG:
            printf("CPU_BAD_ARG\n");
            break;
        default:
            printf("Ошибку добавь, дебил\n");
            break;
    }
}


#endif