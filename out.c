# 1 "src/ncvm.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 418 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "src/ncvm.c" 2
# 1 "include/ncvm.h" 1
# 22 "include/ncvm.h"
enum __attribute__((__packed__)) OPCODE {

    NOP,
    STOP,
    RET,



    IMOV,
    LMOV,
    FMOV,
    DMOV,

    IRCLR,
    LRCLR,
    FRCLR,
    DRCLR,

    ISR,
    LSR,







    IRSI,
    ILSI,
    LRSI,
    LLSI,

    IRSA,
    ILSA,
    LRSA,
    LLSA,



    ISMLD,
    ISMST,


    LSMLD,
    LSMST,


    FSMLD,
    FSMST,


    DSMLD,
    DSMST,



    POPI,
    POPA,

    IPUSH,
    ISTLD,
    ISTST,

    LPUSH,
    LSTLD,
    LSTST,

    FPUSH,
    FSTLD,
    FSTST,

    DPUSH,
    DSTLD,
    DSTST,



    ALLOC,
    FREE,
    HELD,
    HEST,




    IADD,
    ISUB,
    IMULT,
    IDIV,
    IMOD,
    IINC,
    IDEC,


    LADD,
    LSUB,
    LMULT,
    LDIV,
    LMOD,
    LINC,
    LDEC,


    FADD,
    FSUB,
    FMULT,
    FDIV,
    FINC,
    FDEC,


    DADD,
    DSUB,
    DMULT,
    DDIV,
    DINC,
    DDEC,



    LTOI,
    FTOI,
    DTOI,
    ITOL,
    FTOL,
    DTOL,
    ITOF,
    LTOF,
    DTOF,
    ITOD,
    LTOD,
    FTOD,






    JMP,

    IJEZ,
    IJNZ,
    IJEQ,
    IJNQ,
    IJML,
    IJEL,
    IJMG,
    IJEG,


    LJEZ,
    LJNZ,
    LJEQ,
    LJNQ,
    LJML,
    LJEL,
    LJMG,
    LJEG,


    FJEZ,
    FJNZ,
    FJEQ,
    FJNQ,
    FJML,
    FJEL,
    FJMG,
    FJEG,


    DJEZ,
    DJNZ,
    DJEQ,
    DJNQ,
    DJML,
    DJEL,
    DJMG,
    DJEG,

    CALL,

    LIBCALL
};

enum __attribute__((__packed__)) REGISTER {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7
};


typedef struct {
    enum OPCODE opcode;
    unsigned char r1;
    unsigned char r2;
    unsigned char r3;
} Instruction;




typedef struct {
    unsigned char u32_reg_size;
    unsigned char u64_reg_size;
    unsigned char f32_reg_size;
    unsigned char f64_reg_size;
    unsigned long stack_size;
    unsigned long call_stack_size;
} ThreadSettings;
# 244 "include/ncvm.h"
typedef struct {
    Instruction* inst_p;
    unsigned long inst_count;
    unsigned char* static_mem_p;
    unsigned long static_mem_size;
    ThreadSettings main_thread_settings;
    void** lib_functions;
} ncvm;

typedef struct {
    ncvm* vm;
    const Instruction* current_instr_p;
    void* stack_p;
    void* call_stack_p;
    unsigned int* u32_registers;
    unsigned long long* u64_registers;
    float* f32_registers;
    double* f64_registers;
} ncvm_thread;


typedef struct {
    unsigned long static_mem_idx;
    unsigned long inst_idx;
} ByteCodeBlocksInfo;


typedef void (*ncvm_lib_function)(ncvm_thread* thread);
# 288 "include/ncvm.h"
__attribute__((visibility("default"))) ncvm ncvm_init(
    Instruction* inst_p,
    unsigned char* static_mem_p,
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void* lib_data_p
);







__attribute__((visibility("default"))) ncvm ncvm_loadBytecodeData(
    const unsigned char* data_p,
    const unsigned long data_size,
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void* lib_data_p
);
# 315 "include/ncvm.h"
__attribute__((visibility("default"))) ncvm ncvm_loadBytecodeStream(
    const unsigned char* (*get_next_n_bytes)(const unsigned long long n, void* const data_p),
    void* data_p,
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void* lib_data_p,
    int* ret_code
);





__attribute__((visibility("default"))) void ncvm_free(ncvm* vm);




__attribute__((visibility("default"))) unsigned char ncvm_execute(ncvm* vm);

__attribute__((visibility("default"))) ncvm_thread ncvm_create_thread(
    ncvm* vm,
    const Instruction* start_instr_p,
    unsigned char* ext_stack_p,
    unsigned long ext_stack_s,
    ThreadSettings settings,
    int* ret_code
);

__attribute__((visibility("default"))) void ncvm_thread_free(ncvm_thread* thread);
__attribute__((visibility("default"))) unsigned char ncvm_execute_thread_step(ncvm_thread* thread);
__attribute__((visibility("default"))) unsigned char ncvm_execute_thread(ncvm_thread* thread);





typedef struct {
    void** lib_handlers;
    unsigned long lib_handlers_count;
} ncvm_default_lib_loader;

__attribute__((visibility("default"))) ncvm_default_lib_loader ncvm_default_lib_loader_init(
    const char** lib_names,
    unsigned long lib_names_count
);
__attribute__((visibility("default"))) ncvm_lib_function ncvm_default_get_lib_function(
    const char* name,
    void* loader
);
__attribute__((visibility("default"))) void ncvm_default_lib_function_loader_free(
    ncvm_default_lib_loader* loader
);
# 2 "src/ncvm.c" 2
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 1 3 4
# 61 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/Availability.h" 1 3 4
# 172 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/Availability.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/AvailabilityVersions.h" 1 3 4
# 173 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/Availability.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/AvailabilityInternal.h" 1 3 4
# 125 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/AvailabilityInternal.h" 3 4
//FIXME: Workaround for rdar://116062344
# 176 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/AvailabilityInternal.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/AvailabilityInternalLegacy.h" 1 3 4
# 177 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/AvailabilityInternal.h" 2 3 4
# 174 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/Availability.h" 2 3 4
# 62 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/cdefs.h" 1 3 4
# 678 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/cdefs.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_symbol_aliasing.h" 1 3 4
# 679 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/cdefs.h" 2 3 4
# 744 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/cdefs.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_posix_availability.h" 1 3 4
# 745 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/cdefs.h" 2 3 4
# 63 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types.h" 1 3 4
# 27 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types.h" 1 3 4
# 33 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/_types.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/_types.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_types.h" 1 3 4
# 15 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_types.h" 3 4
typedef signed char __int8_t;



typedef unsigned char __uint8_t;
typedef short __int16_t;
typedef unsigned short __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef long long __int64_t;
typedef unsigned long long __uint64_t;

typedef long __darwin_intptr_t;
typedef unsigned int __darwin_natural_t;
# 48 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_types.h" 3 4
typedef int __darwin_ct_rune_t;





typedef union {
 char __mbstate8[128];
 long long _mbstateL;
} __mbstate_t;

typedef __mbstate_t __darwin_mbstate_t;


typedef long int __darwin_ptrdiff_t;







typedef long unsigned int __darwin_size_t;





typedef __builtin_va_list __darwin_va_list;





typedef int __darwin_wchar_t;




typedef __darwin_wchar_t __darwin_rune_t;


typedef int __darwin_wint_t;




typedef unsigned long __darwin_clock_t;
typedef __uint32_t __darwin_socklen_t;
typedef long __darwin_ssize_t;
typedef long __darwin_time_t;
# 35 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/_types.h" 2 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types.h" 2 3 4
# 55 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types.h" 3 4
typedef __int64_t __darwin_blkcnt_t;
typedef __int32_t __darwin_blksize_t;
typedef __int32_t __darwin_dev_t;
typedef unsigned int __darwin_fsblkcnt_t;
typedef unsigned int __darwin_fsfilcnt_t;
typedef __uint32_t __darwin_gid_t;
typedef __uint32_t __darwin_id_t;
typedef __uint64_t __darwin_ino64_t;

typedef __darwin_ino64_t __darwin_ino_t;



typedef __darwin_natural_t __darwin_mach_port_name_t;
typedef __darwin_mach_port_name_t __darwin_mach_port_t;
typedef __uint16_t __darwin_mode_t;
typedef __int64_t __darwin_off_t;
typedef __int32_t __darwin_pid_t;
typedef __uint32_t __darwin_sigset_t;
typedef __int32_t __darwin_suseconds_t;
typedef __uint32_t __darwin_uid_t;
typedef __uint32_t __darwin_useconds_t;
typedef unsigned char __darwin_uuid_t[16];
typedef char __darwin_uuid_string_t[37];

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_pthread/_pthread_types.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_pthread/_pthread_types.h" 3 4
// pthread opaque structures
# 57 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_pthread/_pthread_types.h" 3 4
struct __darwin_pthread_handler_rec {
 void (*__routine)(void *); // Routine to call
 void *__arg; // Argument to pass
 struct __darwin_pthread_handler_rec *__next;
};

struct _opaque_pthread_attr_t {
 long __sig;
 char __opaque[56];
};

struct _opaque_pthread_cond_t {
 long __sig;
 char __opaque[40];
};

struct _opaque_pthread_condattr_t {
 long __sig;
 char __opaque[8];
};

struct _opaque_pthread_mutex_t {
 long __sig;
 char __opaque[56];
};

struct _opaque_pthread_mutexattr_t {
 long __sig;
 char __opaque[8];
};

struct _opaque_pthread_once_t {
 long __sig;
 char __opaque[8];
};

struct _opaque_pthread_rwlock_t {
 long __sig;
 char __opaque[192];
};

struct _opaque_pthread_rwlockattr_t {
 long __sig;
 char __opaque[16];
};

struct _opaque_pthread_t {
 long __sig;
 struct __darwin_pthread_handler_rec *__cleanup_stack;
 char __opaque[8176];
};

typedef struct _opaque_pthread_attr_t __darwin_pthread_attr_t;
typedef struct _opaque_pthread_cond_t __darwin_pthread_cond_t;
typedef struct _opaque_pthread_condattr_t __darwin_pthread_condattr_t;
typedef unsigned long __darwin_pthread_key_t;
typedef struct _opaque_pthread_mutex_t __darwin_pthread_mutex_t;
typedef struct _opaque_pthread_mutexattr_t __darwin_pthread_mutexattr_t;
typedef struct _opaque_pthread_once_t __darwin_pthread_once_t;
typedef struct _opaque_pthread_rwlock_t __darwin_pthread_rwlock_t;
typedef struct _opaque_pthread_rwlockattr_t __darwin_pthread_rwlockattr_t;
typedef struct _opaque_pthread_t *__darwin_pthread_t;
# 81 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types.h" 2 3 4
# 28 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types.h" 2 3 4
# 40 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types.h" 3 4
typedef int __darwin_nl_item;
typedef int __darwin_wctrans_t;

typedef __uint32_t __darwin_wctype_t;
# 65 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 1 3 4
# 79 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 3 4
typedef enum {
 P_ALL,
 P_PID,
 P_PGID
} idtype_t;





# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_pid_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_pid_t.h" 3 4
typedef __darwin_pid_t pid_t;
# 90 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_id_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_id_t.h" 3 4
typedef __darwin_id_t id_t;
# 91 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 2 3 4
# 109 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 1 3 4
# 73 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/appleapiopts.h" 1 3 4
# 74 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4








# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/signal.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/signal.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/signal.h" 1 3 4
# 17 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/signal.h" 3 4
typedef int sig_atomic_t;
# 35 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/signal.h" 2 3 4
# 83 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4
# 146 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/_mcontext.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/_mcontext.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_mcontext.h" 1 3 4
# 36 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_mcontext.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/machine/_structs.h" 1 3 4
# 35 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/machine/_structs.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 1 3 4
# 37 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/types.h" 1 3 4
# 37 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/types.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 1 3 4
# 55 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int8_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int8_t.h" 3 4
typedef signed char int8_t;
# 56 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int16_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int16_t.h" 3 4
typedef short int16_t;
# 57 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int32_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int32_t.h" 3 4
typedef int int32_t;
# 58 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int64_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_int64_t.h" 3 4
typedef long long int64_t;
# 59 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int8_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int8_t.h" 3 4
typedef unsigned char u_int8_t;
# 61 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int16_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int16_t.h" 3 4
typedef unsigned short u_int16_t;
# 62 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int32_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int32_t.h" 3 4
typedef unsigned int u_int32_t;
# 63 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int64_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_u_int64_t.h" 3 4
typedef unsigned long long u_int64_t;
# 64 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4


typedef int64_t register_t;




# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_intptr_t.h" 1 3 4
# 30 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_intptr_t.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/types.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_intptr_t.h" 2 3 4

typedef __darwin_intptr_t intptr_t;
# 72 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_uintptr_t.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_uintptr_t.h" 3 4
typedef unsigned long uintptr_t;
# 73 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 2 3 4




typedef u_int64_t user_addr_t;
typedef u_int64_t user_size_t;
typedef int64_t user_ssize_t;
typedef int64_t user_long_t;
typedef u_int64_t user_ulong_t;
typedef int64_t user_time_t;
typedef int64_t user_off_t;
# 104 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/types.h" 3 4
typedef u_int64_t syscall_arg_t;
# 38 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/types.h" 2 3 4
# 38 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 2 3 4



struct __darwin_arm_exception_state
{
 __uint32_t __exception;
 __uint32_t __fsr;
 __uint32_t __far;
};
# 59 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __darwin_arm_exception_state64
{
 __uint64_t __far;
 __uint32_t __esr;
 __uint32_t __exception;
};
# 77 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __darwin_arm_thread_state
{
 __uint32_t __r[13];
 __uint32_t __sp;
 __uint32_t __lr;
 __uint32_t __pc;
 __uint32_t __cpsr;
};
# 136 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __darwin_arm_thread_state64
{
 __uint64_t __x[29];
 __uint64_t __fp;
 __uint64_t __lr;
 __uint64_t __sp;
 __uint64_t __pc;
 __uint32_t __cpsr;
 __uint32_t __pad;
};
# 477 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __darwin_arm_vfp_state
{
 __uint32_t __r[64];
 __uint32_t __fpscr;
};
# 496 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __darwin_arm_neon_state64
{
 __uint128_t __v[32];
 __uint32_t __fpsr;
 __uint32_t __fpcr;
};

struct __darwin_arm_neon_state
{
 __uint128_t __v[16];
 __uint32_t __fpsr;
 __uint32_t __fpcr;
};
# 567 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __arm_pagein_state
{
 int __pagein_error;
};
# 604 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __arm_legacy_debug_state
{
 __uint32_t __bvr[16];
 __uint32_t __bcr[16];
 __uint32_t __wvr[16];
 __uint32_t __wcr[16];
};
# 627 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __darwin_arm_debug_state32
{
 __uint32_t __bvr[16];
 __uint32_t __bcr[16];
 __uint32_t __wvr[16];
 __uint32_t __wcr[16];
 __uint64_t __mdscr_el1;
};


struct __darwin_arm_debug_state64
{
 __uint64_t __bvr[16];
 __uint64_t __bcr[16];
 __uint64_t __wvr[16];
 __uint64_t __wcr[16];
 __uint64_t __mdscr_el1;
};
# 669 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/arm/_structs.h" 3 4
struct __darwin_arm_cpmu_state64
{
 __uint64_t __ctrs[16];
};
# 36 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/machine/_structs.h" 2 3 4
# 37 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_mcontext.h" 2 3 4




struct __darwin_mcontext32
{
 struct __darwin_arm_exception_state __es;
 struct __darwin_arm_thread_state __ss;
 struct __darwin_arm_vfp_state __fs;
};
# 64 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_mcontext.h" 3 4
struct __darwin_mcontext64
{
 struct __darwin_arm_exception_state64 __es;
 struct __darwin_arm_thread_state64 __ss;
 struct __darwin_arm_neon_state64 __ns;
};
# 85 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/_mcontext.h" 3 4
typedef struct __darwin_mcontext64 *mcontext_t;
# 35 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/_mcontext.h" 2 3 4
# 147 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_pthread/_pthread_attr_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_pthread/_pthread_attr_t.h" 3 4
typedef __darwin_pthread_attr_t pthread_attr_t;
# 149 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_sigaltstack.h" 1 3 4
# 42 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_sigaltstack.h" 3 4
struct __darwin_sigaltstack
{
 void *ss_sp;
 __darwin_size_t ss_size;
 int ss_flags;
};
typedef struct __darwin_sigaltstack stack_t;
# 151 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_ucontext.h" 1 3 4
# 43 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_ucontext.h" 3 4
struct __darwin_ucontext
{
 int uc_onstack;
 __darwin_sigset_t uc_sigmask;
 struct __darwin_sigaltstack uc_stack;
 struct __darwin_ucontext *uc_link;
 __darwin_size_t uc_mcsize;
 struct __darwin_mcontext64 *uc_mcontext;



};


typedef struct __darwin_ucontext ucontext_t;
# 152 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4


# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_sigset_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_sigset_t.h" 3 4
typedef __darwin_sigset_t sigset_t;
# 155 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_size_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_size_t.h" 3 4
typedef __darwin_size_t size_t;
# 156 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_uid_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_uid_t.h" 3 4
typedef __darwin_uid_t uid_t;
# 157 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 2 3 4

union sigval {

 int sival_int;
 void *sival_ptr;
};





struct sigevent {
 int sigev_notify;
 int sigev_signo;
 union sigval sigev_value;
 void (*sigev_notify_function)(union sigval);
 pthread_attr_t *sigev_notify_attributes;
};


typedef struct __siginfo {
 int si_signo;
 int si_errno;
 int si_code;
 pid_t si_pid;
 uid_t si_uid;
 int si_status;
 void *si_addr;
 union sigval si_value;
 long si_band;
 unsigned long __pad[7];
} siginfo_t;
# 269 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 3 4
union __sigaction_u {
 void (*__sa_handler)(int);
 void (*__sa_sigaction)(int, struct __siginfo *,
     void *);
};


struct __sigaction {
 union __sigaction_u __sigaction_u;
 void (*sa_tramp)(void *, int, int, siginfo_t *, void *);
 sigset_t sa_mask;
 int sa_flags;
};




struct sigaction {
 union __sigaction_u __sigaction_u;
 sigset_t sa_mask;
 int sa_flags;
};
# 331 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 3 4
typedef void (*sig_t)(int);
# 348 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 3 4
struct sigvec {
 void (*sv_handler)(int);
 int sv_mask;
 int sv_flags;
};
# 367 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 3 4
struct sigstack {
 char *ss_sp;
 int ss_onstack;
};
# 390 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/signal.h" 3 4
void(*signal(int, void (*)(int)))(int);
# 110 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 1 3 4
# 72 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 3 4
# 1 "/Library/Developer/CommandLineTools/usr/lib/clang/15.0.0/include/stdint.h" 1 3 4
# 10 "/Library/Developer/CommandLineTools/usr/lib/clang/15.0.0/include/stdint.h" 3 4
// AIX system headers need stdint.h to be re-enterable while _STD_TYPES_T
// is defined until an inclusion of it without _STD_TYPES_T occurs, in which
// case the header guard macro is defined.
# 22 "/Library/Developer/CommandLineTools/usr/lib/clang/15.0.0/include/stdint.h" 3 4
// C99 7.18.3 Limits of other integer types
//
// Footnote 219, 220: C++ implementations should define these macros only when
// __STDC_LIMIT_MACROS is defined before <stdint.h> is included.
//
// Footnote 222: C++ implementations should define these macros only when
// __STDC_CONSTANT_MACROS is defined before <stdint.h> is included.
//
// C++11 [cstdint.syn]p2:
//
// The macros defined by <cstdint> are provided unconditionally. In particular,
// the symbols __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS (mentioned in
// footnotes 219, 220, and 222 in the C standard) play no role in C++.
//
// C11 removed the problematic footnotes.
//
// Work around this inconsistency by always defining those macros in C++ mode,
// so that a C library implementation which follows the C99 standard can be
// used in C++.
# 52 "/Library/Developer/CommandLineTools/usr/lib/clang/15.0.0/include/stdint.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 1 3 4
# 23 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint8_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint8_t.h" 3 4
typedef unsigned char uint8_t;
# 24 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint16_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint16_t.h" 3 4
typedef unsigned short uint16_t;
# 25 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint32_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint32_t.h" 3 4
typedef unsigned int uint32_t;
# 26 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint64_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint64_t.h" 3 4
typedef unsigned long long uint64_t;
# 27 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 2 3 4


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;
typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;



typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;
typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;
# 58 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_intmax_t.h" 1 3 4
# 32 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_intmax_t.h" 3 4
typedef long int intmax_t;
# 59 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uintmax_t.h" 1 3 4
# 32 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uintmax_t.h" 3 4
typedef long unsigned int uintmax_t;
# 60 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdint.h" 2 3 4
# 53 "/Library/Developer/CommandLineTools/usr/lib/clang/15.0.0/include/stdint.h" 2 3 4
# 73 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 2 3 4







# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_timeval.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_timeval.h" 3 4
struct timeval
{
 __darwin_time_t tv_sec;
 __darwin_suseconds_t tv_usec;
};
# 81 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 2 3 4








typedef __uint64_t rlim_t;
# 152 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 3 4
struct rusage {
 struct timeval ru_utime;
 struct timeval ru_stime;
# 163 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 3 4
 long ru_maxrss;

 long ru_ixrss;
 long ru_idrss;
 long ru_isrss;
 long ru_minflt;
 long ru_majflt;
 long ru_nswap;
 long ru_inblock;
 long ru_oublock;
 long ru_msgsnd;
 long ru_msgrcv;
 long ru_nsignals;
 long ru_nvcsw;
 long ru_nivcsw;


};
# 200 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 3 4
typedef void *rusage_info_t;

struct rusage_info_v0 {
 uint8_t ri_uuid[16];
 uint64_t ri_user_time;
 uint64_t ri_system_time;
 uint64_t ri_pkg_idle_wkups;
 uint64_t ri_interrupt_wkups;
 uint64_t ri_pageins;
 uint64_t ri_wired_size;
 uint64_t ri_resident_size;
 uint64_t ri_phys_footprint;
 uint64_t ri_proc_start_abstime;
 uint64_t ri_proc_exit_abstime;
};

struct rusage_info_v1 {
 uint8_t ri_uuid[16];
 uint64_t ri_user_time;
 uint64_t ri_system_time;
 uint64_t ri_pkg_idle_wkups;
 uint64_t ri_interrupt_wkups;
 uint64_t ri_pageins;
 uint64_t ri_wired_size;
 uint64_t ri_resident_size;
 uint64_t ri_phys_footprint;
 uint64_t ri_proc_start_abstime;
 uint64_t ri_proc_exit_abstime;
 uint64_t ri_child_user_time;
 uint64_t ri_child_system_time;
 uint64_t ri_child_pkg_idle_wkups;
 uint64_t ri_child_interrupt_wkups;
 uint64_t ri_child_pageins;
 uint64_t ri_child_elapsed_abstime;
};

struct rusage_info_v2 {
 uint8_t ri_uuid[16];
 uint64_t ri_user_time;
 uint64_t ri_system_time;
 uint64_t ri_pkg_idle_wkups;
 uint64_t ri_interrupt_wkups;
 uint64_t ri_pageins;
 uint64_t ri_wired_size;
 uint64_t ri_resident_size;
 uint64_t ri_phys_footprint;
 uint64_t ri_proc_start_abstime;
 uint64_t ri_proc_exit_abstime;
 uint64_t ri_child_user_time;
 uint64_t ri_child_system_time;
 uint64_t ri_child_pkg_idle_wkups;
 uint64_t ri_child_interrupt_wkups;
 uint64_t ri_child_pageins;
 uint64_t ri_child_elapsed_abstime;
 uint64_t ri_diskio_bytesread;
 uint64_t ri_diskio_byteswritten;
};

struct rusage_info_v3 {
 uint8_t ri_uuid[16];
 uint64_t ri_user_time;
 uint64_t ri_system_time;
 uint64_t ri_pkg_idle_wkups;
 uint64_t ri_interrupt_wkups;
 uint64_t ri_pageins;
 uint64_t ri_wired_size;
 uint64_t ri_resident_size;
 uint64_t ri_phys_footprint;
 uint64_t ri_proc_start_abstime;
 uint64_t ri_proc_exit_abstime;
 uint64_t ri_child_user_time;
 uint64_t ri_child_system_time;
 uint64_t ri_child_pkg_idle_wkups;
 uint64_t ri_child_interrupt_wkups;
 uint64_t ri_child_pageins;
 uint64_t ri_child_elapsed_abstime;
 uint64_t ri_diskio_bytesread;
 uint64_t ri_diskio_byteswritten;
 uint64_t ri_cpu_time_qos_default;
 uint64_t ri_cpu_time_qos_maintenance;
 uint64_t ri_cpu_time_qos_background;
 uint64_t ri_cpu_time_qos_utility;
 uint64_t ri_cpu_time_qos_legacy;
 uint64_t ri_cpu_time_qos_user_initiated;
 uint64_t ri_cpu_time_qos_user_interactive;
 uint64_t ri_billed_system_time;
 uint64_t ri_serviced_system_time;
};

struct rusage_info_v4 {
 uint8_t ri_uuid[16];
 uint64_t ri_user_time;
 uint64_t ri_system_time;
 uint64_t ri_pkg_idle_wkups;
 uint64_t ri_interrupt_wkups;
 uint64_t ri_pageins;
 uint64_t ri_wired_size;
 uint64_t ri_resident_size;
 uint64_t ri_phys_footprint;
 uint64_t ri_proc_start_abstime;
 uint64_t ri_proc_exit_abstime;
 uint64_t ri_child_user_time;
 uint64_t ri_child_system_time;
 uint64_t ri_child_pkg_idle_wkups;
 uint64_t ri_child_interrupt_wkups;
 uint64_t ri_child_pageins;
 uint64_t ri_child_elapsed_abstime;
 uint64_t ri_diskio_bytesread;
 uint64_t ri_diskio_byteswritten;
 uint64_t ri_cpu_time_qos_default;
 uint64_t ri_cpu_time_qos_maintenance;
 uint64_t ri_cpu_time_qos_background;
 uint64_t ri_cpu_time_qos_utility;
 uint64_t ri_cpu_time_qos_legacy;
 uint64_t ri_cpu_time_qos_user_initiated;
 uint64_t ri_cpu_time_qos_user_interactive;
 uint64_t ri_billed_system_time;
 uint64_t ri_serviced_system_time;
 uint64_t ri_logical_writes;
 uint64_t ri_lifetime_max_phys_footprint;
 uint64_t ri_instructions;
 uint64_t ri_cycles;
 uint64_t ri_billed_energy;
 uint64_t ri_serviced_energy;
 uint64_t ri_interval_max_phys_footprint;
 uint64_t ri_runnable_time;
};

struct rusage_info_v5 {
 uint8_t ri_uuid[16];
 uint64_t ri_user_time;
 uint64_t ri_system_time;
 uint64_t ri_pkg_idle_wkups;
 uint64_t ri_interrupt_wkups;
 uint64_t ri_pageins;
 uint64_t ri_wired_size;
 uint64_t ri_resident_size;
 uint64_t ri_phys_footprint;
 uint64_t ri_proc_start_abstime;
 uint64_t ri_proc_exit_abstime;
 uint64_t ri_child_user_time;
 uint64_t ri_child_system_time;
 uint64_t ri_child_pkg_idle_wkups;
 uint64_t ri_child_interrupt_wkups;
 uint64_t ri_child_pageins;
 uint64_t ri_child_elapsed_abstime;
 uint64_t ri_diskio_bytesread;
 uint64_t ri_diskio_byteswritten;
 uint64_t ri_cpu_time_qos_default;
 uint64_t ri_cpu_time_qos_maintenance;
 uint64_t ri_cpu_time_qos_background;
 uint64_t ri_cpu_time_qos_utility;
 uint64_t ri_cpu_time_qos_legacy;
 uint64_t ri_cpu_time_qos_user_initiated;
 uint64_t ri_cpu_time_qos_user_interactive;
 uint64_t ri_billed_system_time;
 uint64_t ri_serviced_system_time;
 uint64_t ri_logical_writes;
 uint64_t ri_lifetime_max_phys_footprint;
 uint64_t ri_instructions;
 uint64_t ri_cycles;
 uint64_t ri_billed_energy;
 uint64_t ri_serviced_energy;
 uint64_t ri_interval_max_phys_footprint;
 uint64_t ri_runnable_time;
 uint64_t ri_flags;
};

struct rusage_info_v6 {
 uint8_t ri_uuid[16];
 uint64_t ri_user_time;
 uint64_t ri_system_time;
 uint64_t ri_pkg_idle_wkups;
 uint64_t ri_interrupt_wkups;
 uint64_t ri_pageins;
 uint64_t ri_wired_size;
 uint64_t ri_resident_size;
 uint64_t ri_phys_footprint;
 uint64_t ri_proc_start_abstime;
 uint64_t ri_proc_exit_abstime;
 uint64_t ri_child_user_time;
 uint64_t ri_child_system_time;
 uint64_t ri_child_pkg_idle_wkups;
 uint64_t ri_child_interrupt_wkups;
 uint64_t ri_child_pageins;
 uint64_t ri_child_elapsed_abstime;
 uint64_t ri_diskio_bytesread;
 uint64_t ri_diskio_byteswritten;
 uint64_t ri_cpu_time_qos_default;
 uint64_t ri_cpu_time_qos_maintenance;
 uint64_t ri_cpu_time_qos_background;
 uint64_t ri_cpu_time_qos_utility;
 uint64_t ri_cpu_time_qos_legacy;
 uint64_t ri_cpu_time_qos_user_initiated;
 uint64_t ri_cpu_time_qos_user_interactive;
 uint64_t ri_billed_system_time;
 uint64_t ri_serviced_system_time;
 uint64_t ri_logical_writes;
 uint64_t ri_lifetime_max_phys_footprint;
 uint64_t ri_instructions;
 uint64_t ri_cycles;
 uint64_t ri_billed_energy;
 uint64_t ri_serviced_energy;
 uint64_t ri_interval_max_phys_footprint;
 uint64_t ri_runnable_time;
 uint64_t ri_flags;
 uint64_t ri_user_ptime;
 uint64_t ri_system_ptime;
 uint64_t ri_pinstructions;
 uint64_t ri_pcycles;
 uint64_t ri_energy_nj;
 uint64_t ri_penergy_nj;
 uint64_t ri_reserved[14];
};

typedef struct rusage_info_v6 rusage_info_current;
# 459 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 3 4
struct rlimit {
 rlim_t rlim_cur;
 rlim_t rlim_max;
};
# 494 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 3 4
struct proc_rlimit_control_wakeupmon {
 uint32_t wm_flags;
 int32_t wm_rate;
};
# 566 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/resource.h" 3 4
int getpriority(int, id_t);

int getiopolicy_np(int, int) __attribute__((availability(macosx,introduced=10.5)));

int getrlimit(int, struct rlimit *) __asm("_" "getrlimit" );
int getrusage(int, struct rusage *);
int setpriority(int, id_t, int);

int setiopolicy_np(int, int, int) __attribute__((availability(macosx,introduced=10.5)));

int setrlimit(int, const struct rlimit *) __asm("_" "setrlimit" );
# 111 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 2 3 4
# 186 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/endian.h" 1 3 4
# 37 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/endian.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/endian.h" 1 3 4
# 77 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/endian.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_endian.h" 1 3 4
# 94 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_endian.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/endian.h" 1 3 4
# 95 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_endian.h" 2 3 4
# 131 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_endian.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/_OSByteOrder.h" 1 3 4
# 80 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/_OSByteOrder.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 1 3 4








# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/arch.h" 1 3 4
# 10 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 2 3 4



static __inline__
uint16_t
_OSSwapInt16(
 uint16_t _data
 )
{

 return (uint16_t)(_data << 8 | _data >> 8);
}

static __inline__
uint32_t
_OSSwapInt32(
 uint32_t _data
 )
{

 _data = __builtin_bswap32(_data);





 return _data;
}

static __inline__
uint64_t
_OSSwapInt64(
 uint64_t _data
 )
{

 return __builtin_bswap64(_data);
# 60 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 3 4
}



struct _OSUnalignedU16 {
 volatile uint16_t __val;
} __attribute__((__packed__));

struct _OSUnalignedU32 {
 volatile uint32_t __val;
} __attribute__((__packed__));

struct _OSUnalignedU64 {
 volatile uint64_t __val;
} __attribute__((__packed__));
# 87 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 3 4
static __inline__
uint16_t
OSReadSwapInt16(
 const volatile void * _base,
 uintptr_t _offset
 )
{
 return _OSSwapInt16(((struct _OSUnalignedU16 *)((uintptr_t)_base + _offset))->__val);
}
# 109 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 3 4
static __inline__
uint32_t
OSReadSwapInt32(
 const volatile void * _base,
 uintptr_t _offset
 )
{
 return _OSSwapInt32(((struct _OSUnalignedU32 *)((uintptr_t)_base + _offset))->__val);
}
# 131 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 3 4
static __inline__
uint64_t
OSReadSwapInt64(
 const volatile void * _base,
 uintptr_t _offset
 )
{
 return _OSSwapInt64(((struct _OSUnalignedU64 *)((uintptr_t)_base + _offset))->__val);
}
# 156 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 3 4
static __inline__
void
OSWriteSwapInt16(
 volatile void * _base,
 uintptr_t _offset,
 uint16_t _data
 )
{
 ((struct _OSUnalignedU16 *)((uintptr_t)_base + _offset))->__val = _OSSwapInt16(_data);
}
# 180 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 3 4
static __inline__
void
OSWriteSwapInt32(
 volatile void * _base,
 uintptr_t _offset,
 uint32_t _data
 )
{
 ((struct _OSUnalignedU32 *)((uintptr_t)_base + _offset))->__val = _OSSwapInt32(_data);
}
# 204 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/arm/OSByteOrder.h" 3 4
static __inline__
void
OSWriteSwapInt64(
 volatile void * _base,
 uintptr_t _offset,
 uint64_t _data
 )
{
 ((struct _OSUnalignedU64 *)((uintptr_t)_base + _offset))->__val = _OSSwapInt64(_data);
}
# 81 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libkern/_OSByteOrder.h" 2 3 4
# 132 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_endian.h" 2 3 4
# 78 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/arm/endian.h" 2 3 4
# 38 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/machine/endian.h" 2 3 4
# 187 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 2 3 4







union wait {
 int w_status;



 struct {

  unsigned int w_Termsig:7,
      w_Coredump:1,
      w_Retcode:8,
      w_Filler:16;







 } w_T;





 struct {

  unsigned int w_Stopval:8,
      w_Stopsig:8,
      w_Filler:16;






 } w_S;
};
# 248 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/wait.h" 3 4
pid_t wait(int *) __asm("_" "wait" );
pid_t waitpid(pid_t, int *, int) __asm("_" "waitpid" );

int waitid(idtype_t, id_t, siginfo_t *, int) __asm("_" "waitid" );


pid_t wait3(int *, int, struct rusage *);
pid_t wait4(pid_t, int *, int, struct rusage *);
# 67 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/alloca.h" 1 3 4
# 32 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/alloca.h" 3 4
void *alloca(size_t);
# 69 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4








# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_ct_rune_t.h" 1 3 4
# 32 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_ct_rune_t.h" 3 4
typedef __darwin_ct_rune_t ct_rune_t;
# 78 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_rune_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_rune_t.h" 3 4
typedef __darwin_rune_t rune_t;
# 79 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4


# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_wchar_t.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_wchar_t.h" 3 4
typedef __darwin_wchar_t wchar_t;
# 82 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4

typedef struct {
 int quot;
 int rem;
} div_t;

typedef struct {
 long quot;
 long rem;
} ldiv_t;
# 100 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_null.h" 1 3 4
# 101 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4
# 118 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 3 4
extern int __mb_cur_max;
# 128 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/malloc/_malloc.h" 1 3 4
# 38 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/malloc/_malloc.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/malloc/_malloc_type.h" 1 3 4
# 46 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/malloc/_malloc_type.h" 3 4
typedef unsigned long long malloc_type_id_t;

__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_malloc(size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(1)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_calloc(size_t count, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(1,2)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void malloc_type_free(void *ptr, malloc_type_id_t type_id);
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_realloc(void *ptr, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(2)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_valloc(size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(1)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_aligned_alloc(size_t alignment, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(2)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) int malloc_type_posix_memalign(void **memptr, size_t alignment, size_t size, malloc_type_id_t type_id) ;




typedef struct _malloc_zone_t malloc_zone_t;

__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_zone_malloc(malloc_zone_t *zone, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(2)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_zone_calloc(malloc_zone_t *zone, size_t count, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(2,3)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void malloc_type_zone_free(malloc_zone_t *zone, void *ptr, malloc_type_id_t type_id);
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_zone_realloc(malloc_zone_t *zone, void *ptr, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(3)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_zone_valloc(malloc_zone_t *zone, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(2)));
__attribute__((availability(macos,unavailable))) __attribute__((availability(ios,unavailable))) __attribute__((availability(tvos,unavailable))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(visionos,unavailable))) void *malloc_type_zone_memalign(malloc_zone_t *zone, size_t alignment, size_t size, malloc_type_id_t type_id) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(3)));
# 39 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/malloc/_malloc.h" 2 3 4






void *malloc(size_t __size) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(1))) ;
void *calloc(size_t __count, size_t __size) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(1,2))) ;
void free(void *);
void *realloc(void *__ptr, size_t __size) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(2))) ;

void *valloc(size_t) __attribute__((alloc_size(1))) ;




void *aligned_alloc(size_t __alignment, size_t __size) __attribute__((__warn_unused_result__)) __attribute__((alloc_size(2))) __attribute__((availability(macosx,introduced=10.15))) __attribute__((availability(ios,introduced=13.0))) __attribute__((availability(tvos,introduced=13.0))) __attribute__((availability(watchos,introduced=6.0)));

int posix_memalign(void **__memptr, size_t __alignment, size_t __size) __attribute__((availability(macosx,introduced=10.6)));
# 129 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4


void abort(void) __attribute__((__cold__)) __attribute__((__noreturn__));
int abs(int) __attribute__((__const__));
int atexit(void (* _Nonnull)(void));
double atof(const char *);
int atoi(const char *);
long atol(const char *);




void *bsearch(const void *__key, const void *__base, size_t __nel,
     size_t __width, int (* _Nonnull __compar)(const void *, const void *));

div_t div(int, int) __attribute__((__const__));
void exit(int) __attribute__((__noreturn__));

char *getenv(const char *);
long labs(long) __attribute__((__const__));
ldiv_t ldiv(long, long) __attribute__((__const__));






int mblen(const char *__s, size_t __n);
size_t mbstowcs(wchar_t * , const char * , size_t);
int mbtowc(wchar_t * , const char * , size_t);

void qsort(void *__base, size_t __nel, size_t __width,
     int (* _Nonnull __compar)(const void *, const void *));
int rand(void) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));

void srand(unsigned) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));
double strtod(const char *, char **) __asm("_" "strtod" );
float strtof(const char *, char **) __asm("_" "strtof" );
long strtol(const char *__str, char **__endptr, int __base);
long double
  strtold(const char *, char **);




unsigned long
  strtoul(const char *__str, char **__endptr, int __base);





__attribute__((__availability__(swift, unavailable, message="Use posix_spawn APIs or NSTask instead. (On iOS, process spawning is unavailable.)")))
__attribute__((availability(macos,introduced=10.0))) __attribute__((availability(ios,unavailable)))
__attribute__((availability(watchos,unavailable))) __attribute__((availability(tvos,unavailable)))
int system(const char *) __asm("_" "system" );


size_t wcstombs(char * , const wchar_t * , size_t);
int wctomb(char *, wchar_t);


void _Exit(int) __attribute__((__noreturn__));
long a64l(const char *);
double drand48(void);
char *ecvt(double, int, int *, int *);
double erand48(unsigned short[3]);
char *fcvt(double, int, int *, int *);
char *gcvt(double, int, char *);
int getsubopt(char **, char * const *, char **);
int grantpt(int);

char *initstate(unsigned, char *, size_t);



long jrand48(unsigned short[3]) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));
char *l64a(long);
void lcong48(unsigned short[7]);
long lrand48(void) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));

__attribute__((__deprecated__("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of mktemp(3), it is highly recommended that you use mkstemp(3) instead.")))

char *mktemp(char *);
int mkstemp(char *);
long mrand48(void) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));
long nrand48(unsigned short[3]) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));
int posix_openpt(int);
char *ptsname(int);


int ptsname_r(int fildes, char *buffer, size_t buflen) __attribute__((availability(macos,introduced=10.13.4))) __attribute__((availability(ios,introduced=11.3))) __attribute__((availability(tvos,introduced=11.3))) __attribute__((availability(watchos,introduced=4.3)));


int putenv(char *) __asm("_" "putenv" );
long random(void) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));
int rand_r(unsigned *) __attribute__((__availability__(swift, unavailable, message="Use arc4random instead.")));

char *realpath(const char * , char * ) __asm("_" "realpath" "$DARWIN_EXTSN");



unsigned short
 *seed48(unsigned short[3]);
int setenv(const char * __name, const char * __value, int __overwrite) __asm("_" "setenv" );

void setkey(const char *) __asm("_" "setkey" );



char *setstate(const char *);
void srand48(long);

void srandom(unsigned);



int unlockpt(int);

int unsetenv(const char *) __asm("_" "unsetenv" );
# 257 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_dev_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_dev_t.h" 3 4
typedef __darwin_dev_t dev_t;
# 258 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_mode_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_mode_t.h" 3 4
typedef __darwin_mode_t mode_t;
# 259 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 2 3 4



uint32_t arc4random(void);
void arc4random_addrandom(unsigned char * , int )
    __attribute__((availability(macosx,introduced=10.0))) __attribute__((availability(macosx,deprecated=10.12,message="use arc4random_stir")))
    __attribute__((availability(ios,introduced=2.0))) __attribute__((availability(ios,deprecated=10.0,message="use arc4random_stir")))
    __attribute__((availability(tvos,introduced=2.0))) __attribute__((availability(tvos,deprecated=10.0,message="use arc4random_stir")))
    __attribute__((availability(watchos,introduced=1.0))) __attribute__((availability(watchos,deprecated=3.0,message="use arc4random_stir")));
void arc4random_buf(void * __buf, size_t __nbytes) __attribute__((availability(macosx,introduced=10.7)));
void arc4random_stir(void);
uint32_t
  arc4random_uniform(uint32_t __upper_bound) __attribute__((availability(macosx,introduced=10.7)));

int atexit_b(void (^ _Nonnull)(void)) __attribute__((availability(macosx,introduced=10.6)));
# 282 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 3 4
void *bsearch_b(const void *__key, const void *__base, size_t __nel,
     size_t __width, int (^ _Nonnull __compar)(const void *, const void *) __attribute__((__noescape__)))
     __attribute__((availability(macosx,introduced=10.6)));



char *cgetcap(char *, const char *, int);
int cgetclose(void);
int cgetent(char **, char **, const char *);
int cgetfirst(char **, char **);
int cgetmatch(const char *, const char *);
int cgetnext(char **, char **);
int cgetnum(char *, const char *, long *);
int cgetset(const char *);
int cgetstr(char *, const char *, char **);
int cgetustr(char *, const char *, char **);

int daemon(int, int) __asm("_" "daemon" ) __attribute__((availability(macosx,introduced=10.0,deprecated=10.5,message="Use posix_spawn APIs instead."))) __attribute__((availability(watchos,unavailable))) __attribute__((availability(tvos,unavailable)));
char *devname(dev_t, mode_t);
char *devname_r(dev_t, mode_t, char *buf, int len);
char *getbsize(int *, long *);
int getloadavg(double [], int);
const char
 *getprogname(void);
void setprogname(const char *);
# 316 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdlib.h" 3 4
int heapsort(void *__base, size_t __nel, size_t __width,
     int (* _Nonnull __compar)(const void *, const void *));

int heapsort_b(void *__base, size_t __nel, size_t __width,
     int (^ _Nonnull __compar)(const void *, const void *) __attribute__((__noescape__)))
     __attribute__((availability(macosx,introduced=10.6)));

int mergesort(void *__base, size_t __nel, size_t __width,
     int (* _Nonnull __compar)(const void *, const void *));

int mergesort_b(void *__base, size_t __nel, size_t __width,
     int (^ _Nonnull __compar)(const void *, const void *) __attribute__((__noescape__)))
     __attribute__((availability(macosx,introduced=10.6)));

void psort(void *__base, size_t __nel, size_t __width,
     int (* _Nonnull __compar)(const void *, const void *))
     __attribute__((availability(macosx,introduced=10.6)));

void psort_b(void *__base, size_t __nel, size_t __width,
     int (^ _Nonnull __compar)(const void *, const void *) __attribute__((__noescape__)))
     __attribute__((availability(macosx,introduced=10.6)));

void psort_r(void *__base, size_t __nel, size_t __width, void *,
     int (* _Nonnull __compar)(void *, const void *, const void *))
     __attribute__((availability(macosx,introduced=10.6)));

void qsort_b(void *__base, size_t __nel, size_t __width,
     int (^ _Nonnull __compar)(const void *, const void *) __attribute__((__noescape__)))
     __attribute__((availability(macosx,introduced=10.6)));

void qsort_r(void *__base, size_t __nel, size_t __width, void *,
     int (* _Nonnull __compar)(void *, const void *, const void *));
int radixsort(const unsigned char **__base, int __nel, const unsigned char *__table,
     unsigned __endbyte);
int rpmatch(const char *)
 __attribute__((availability(macos,introduced=10.15))) __attribute__((availability(ios,introduced=13.0))) __attribute__((availability(tvos,introduced=13.0))) __attribute__((availability(watchos,introduced=6.0)));
int sradixsort(const unsigned char **__base, int __nel, const unsigned char *__table,
     unsigned __endbyte);
void sranddev(void);
void srandomdev(void);
void *reallocf(void *__ptr, size_t __size) __attribute__((alloc_size(2)));
long long
 strtonum(const char *__numstr, long long __minval, long long __maxval, const char **__errstrp)
 __attribute__((availability(macos,introduced=11.0))) __attribute__((availability(ios,introduced=14.0))) __attribute__((availability(tvos,introduced=14.0))) __attribute__((availability(watchos,introduced=7.0)));






extern char *suboptarg;
# 3 "src/ncvm.c" 2
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 1 3 4
# 70 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 3 4
void *memchr(const void *__s, int __c, size_t __n);
int memcmp(const void *__s1, const void *__s2, size_t __n);
void *memcpy(void *__dst, const void *__src, size_t __n);
void *memmove(void *__dst, const void *__src, size_t __len);
void *memset(void *__b, int __c, size_t __len);
char *strcat(char *__s1, const char *__s2);
char *strchr(const char *__s, int __c);
int strcmp(const char *__s1, const char *__s2);
int strcoll(const char *__s1, const char *__s2);
char *strcpy(char *__dst, const char *__src);
size_t strcspn(const char *__s, const char *__charset);
char *strerror(int __errnum) __asm("_" "strerror" );
size_t strlen(const char *__s);
char *strncat(char *__s1, const char *__s2, size_t __n);
int strncmp(const char *__s1, const char *__s2, size_t __n);
char *strncpy(char *__dst, const char *__src, size_t __n);
char *strpbrk(const char *__s, const char *__charset);
char *strrchr(const char *__s, int __c);
size_t strspn(const char *__s, const char *__charset);
char *strstr(const char *__big, const char *__little);
char *strtok(char *__str, const char *__sep);
size_t strxfrm(char *__s1, const char *__s2, size_t __n);
# 104 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 3 4
char *strtok_r(char *__str, const char *__sep, char **__lasts);
# 116 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 3 4
int strerror_r(int __errnum, char *__strerrbuf, size_t __buflen);
char *strdup(const char *__s1);
void *memccpy(void *__dst, const void *__src, int __c, size_t __n);
# 130 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 3 4
char *stpcpy(char *__dst, const char *__src);
char *stpncpy(char *__dst, const char *__src, size_t __n) __attribute__((availability(macosx,introduced=10.7)));
char *strndup(const char *__s1, size_t __n) __attribute__((availability(macosx,introduced=10.7)));
size_t strnlen(const char *__s1, size_t __n) __attribute__((availability(macosx,introduced=10.7)));
char *strsignal(int __sig);
# 152 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_ssize_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_ssize_t.h" 3 4
typedef __darwin_ssize_t ssize_t;
# 153 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 2 3 4


void *memmem(const void *__big, size_t __big_len, const void *__little, size_t __little_len) __attribute__((availability(macosx,introduced=10.7)));
void memset_pattern4(void *__b, const void *__pattern4, size_t __len) __attribute__((availability(macosx,introduced=10.5)));
void memset_pattern8(void *__b, const void *__pattern8, size_t __len) __attribute__((availability(macosx,introduced=10.5)));
void memset_pattern16(void *__b, const void *__pattern16, size_t __len) __attribute__((availability(macosx,introduced=10.5)));

char *strcasestr(const char *__big, const char *__little);
char *strnstr(const char *__big, const char *__little, size_t __len);
size_t strlcat(char *__dst, const char *__source, size_t __size);
size_t strlcpy(char *__dst, const char *__source, size_t __size);
void strmode(int __mode, char *__bp);
char *strsep(char **__stringp, const char *__delim);


void swab(const void * , void * , ssize_t);

__attribute__((availability(macosx,introduced=10.12.1))) __attribute__((availability(ios,introduced=10.1)))
__attribute__((availability(tvos,introduced=10.0.1))) __attribute__((availability(watchos,introduced=3.1)))
int timingsafe_bcmp(const void *__b1, const void *__b2, size_t __len);

__attribute__((availability(macosx,introduced=11.0))) __attribute__((availability(ios,introduced=14.0)))
__attribute__((availability(tvos,introduced=14.0))) __attribute__((availability(watchos,introduced=7.0)))
int strsignal_r(int __sig, char *__strsignalbuf, size_t __buflen);







# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/strings.h" 1 3 4
# 70 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/strings.h" 3 4
int bcmp(const void *, const void *, size_t) ;
void bcopy(const void *, void *, size_t) ;
void bzero(void *, size_t) ;
char *index(const char *, int) ;
char *rindex(const char *, int) ;


int ffs(int);
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);





int ffsl(long) __attribute__((availability(macosx,introduced=10.5)));
int ffsll(long long) __attribute__((availability(macosx,introduced=10.9)));
int fls(int) __attribute__((availability(macosx,introduced=10.5)));
int flsl(long) __attribute__((availability(macosx,introduced=10.5)));
int flsll(long long) __attribute__((availability(macosx,introduced=10.9)));


# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 1 3 4
# 93 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/strings.h" 2 3 4




# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_strings.h" 1 3 4
# 33 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_strings.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_common.h" 1 3 4
# 34 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_strings.h" 2 3 4
# 98 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/strings.h" 2 3 4
# 185 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 2 3 4
# 194 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_string.h" 1 3 4
# 195 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/string.h" 2 3 4
# 4 "src/ncvm.c" 2
# 1 "/Users/ghost/.xmake/packages/e/extc/67103eab/a5b4e345986a4740863d88955955c4b5/include/extc_stack.h" 1 3


# 1 "/Users/ghost/.xmake/packages/e/extc/67103eab/a5b4e345986a4740863d88955955c4b5/include/extc_rint.h" 1 3


typedef char i8;
typedef short i16;
typedef int i32;
typedef long isize;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long usize;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;
# 4 "/Users/ghost/.xmake/packages/e/extc/67103eab/a5b4e345986a4740863d88955955c4b5/include/extc_stack.h" 2 3
# 5 "src/ncvm.c" 2

# 1 "src/util.h" 1
int is_little_endian();
# 7 "src/ncvm.c" 2

typedef struct stack_byte { usize capacity; usize size; u8* data; } stack_byte; stack_byte stack_byte_init(usize size, u8* result); u8 stack_byte_realloc(stack_byte * v, usize n); u8 stack_byte_push(stack_byte * v, u8 o); u8 stack_byte_push_ptr(stack_byte * v, u8* o, usize c); u8 stack_byte_pop(stack_byte * v, u8* e); u8 stack_byte_pop_ptr(stack_byte * v, usize c, u8** e); void stack_byte_free(stack_byte * v); void stack_byte_clean(stack_byte * v);
stack_byte stack_byte_init(usize _size, u8* result) { stack_byte res; res.size = 0; res.capacity = _size; res.data = (u8*)malloc(sizeof(u8)*_size); if ((void*)result != ((void *)0)) { if ((void*)res.data == ((void *)0)) *result = 1; else *result = 0; } return res; } u8 stack_byte_realloc(stack_byte * v, usize n){ if (v->capacity == n) return 0; u8* new_data = (u8*)realloc(v->data, n * sizeof(u8)); if ((void*)new_data == ((void *)0)) return 0; v->data = new_data; v->capacity = n; return 1; } u8 stack_byte_push(stack_byte * v, u8 o) { if(v->capacity < v->size+1) return 0; v->data[v->size++] = o; return 1; } u8 stack_byte_push_ptr(stack_byte * v, u8* o, usize c) { if(v->capacity < v->size+c) return 0; __builtin___memcpy_chk (&v->data[v->size], o, sizeof(u8)*c, __builtin_object_size (&v->data[v->size], 0)); v->size += c; return 1; } u8 stack_byte_pop(stack_byte * v, u8* e) { if(v->size == 0) return 0; if ((void*)e != ((void *)0)) *e = v->data[--v->size]; return 1; } u8 stack_byte_pop_ptr(stack_byte * v, usize c, u8** e) { if(v->size < c) return 0; v->size -= c; if ((void*)e != ((void *)0)) *e = &v->data[v->size]; return 1; } void stack_byte_free(stack_byte * v) { v->size = 0; v->capacity = 0; free(v->data); } void stack_byte_clean(stack_byte * v) { stack_byte_free(v); v->data = (u8*)malloc(0); }

typedef struct stack_usize { usize capacity; usize size; usize* data; } stack_usize; stack_usize stack_usize_init(usize size, u8* result); u8 stack_usize_realloc(stack_usize * v, usize n); u8 stack_usize_push(stack_usize * v, usize o); u8 stack_usize_push_ptr(stack_usize * v, usize* o, usize c); u8 stack_usize_pop(stack_usize * v, usize* e); u8 stack_usize_pop_ptr(stack_usize * v, usize c, usize** e); void stack_usize_free(stack_usize * v); void stack_usize_clean(stack_usize * v);
stack_usize stack_usize_init(usize _size, u8* result) { stack_usize res; res.size = 0; res.capacity = _size; res.data = (usize*)malloc(sizeof(usize)*_size); if ((void*)result != ((void *)0)) { if ((void*)res.data == ((void *)0)) *result = 1; else *result = 0; } return res; } u8 stack_usize_realloc(stack_usize * v, usize n){ if (v->capacity == n) return 0; usize* new_data = (usize*)realloc(v->data, n * sizeof(usize)); if ((void*)new_data == ((void *)0)) return 0; v->data = new_data; v->capacity = n; return 1; } u8 stack_usize_push(stack_usize * v, usize o) { if(v->capacity < v->size+1) return 0; v->data[v->size++] = o; return 1; } u8 stack_usize_push_ptr(stack_usize * v, usize* o, usize c) { if(v->capacity < v->size+c) return 0; __builtin___memcpy_chk (&v->data[v->size], o, sizeof(usize)*c, __builtin_object_size (&v->data[v->size], 0)); v->size += c; return 1; } u8 stack_usize_pop(stack_usize * v, usize* e) { if(v->size == 0) return 0; if ((void*)e != ((void *)0)) *e = v->data[--v->size]; return 1; } u8 stack_usize_pop_ptr(stack_usize * v, usize c, usize** e) { if(v->size < c) return 0; v->size -= c; if ((void*)e != ((void *)0)) *e = &v->data[v->size]; return 1; } void stack_usize_free(stack_usize * v) { v->size = 0; v->capacity = 0; free(v->data); } void stack_usize_clean(stack_usize * v) { stack_usize_free(v); v->data = (usize*)malloc(0); }


__attribute__((visibility("default"))) ncvm ncvm_init(
    Instruction* inst_p,
    unsigned char* static_mem_p,
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void* lib_data_p
) {
    ncvm ret;
    ret.inst_p = inst_p;

    ret.static_mem_p = static_mem_p;

    ret.main_thread_settings = (ThreadSettings){ .u32_reg_size=8, .u64_reg_size=8, .f32_reg_size=8, .f64_reg_size=8, .stack_size=1024*1024*1, .call_stack_size=1024*128};
    return ret;
}


__attribute__((visibility("default"))) void ncvm_free(ncvm* vm) {
    free(vm->inst_p);
    free(vm->static_mem_p);

}

__attribute__((visibility("default"))) u8 ncvm_execute(ncvm* vm) {

    int code;
    ncvm_thread thread = ncvm_create_thread(
        vm,
        vm->inst_p,
        ((void *)0),
        0,
        vm->main_thread_settings,
        &code
    );
    ncvm_execute_thread(&thread);
    return code;
}

__attribute__((visibility("default"))) ncvm_thread ncvm_create_thread(
    ncvm* vm, const Instruction* si_p, u8* EST, usize EST_size,
    ThreadSettings settings, int* ret_code
) {
    ncvm_thread result;

    result.vm = vm;
    result.current_instr_p = si_p;


    if (!is_little_endian()) {
        if (ret_code != ((void *)0))
            *ret_code = 1;
        return result;
    }


    u8 st_r;
    result.stack_p = malloc(sizeof(stack_byte));
    *(stack_byte*)result.stack_p = stack_byte_init(settings.stack_size, &st_r);
    if (st_r != 0) {
        if (ret_code != ((void *)0))
            *ret_code = 2;
        return result;
    }

    result.call_stack_p = malloc(sizeof(stack_usize));
    *(stack_usize*)result.call_stack_p = stack_usize_init(settings.call_stack_size, &st_r);
    if (st_r != 0) {
        if (ret_code != ((void *)0))
            *ret_code = 3;
        return result;
    }


    if (EST != ((void *)0))
        stack_byte_push_ptr((stack_byte*)result.stack_p, EST, EST_size);


    result.u32_registers = (u32*)malloc(sizeof(u32) * settings.u32_reg_size);
    result.u64_registers = (u64*)malloc(sizeof(u64) * settings.u64_reg_size+1);
    result.f32_registers = (f32*)malloc(sizeof(f32) * settings.f32_reg_size);
    result.f64_registers = (f64*)malloc(sizeof(f64) * settings.f64_reg_size);


    __builtin___memset_chk (result.u32_registers, 0, sizeof(u32) * settings.u32_reg_size, __builtin_object_size (result.u32_registers, 0));
    __builtin___memset_chk (result.u64_registers, 0, sizeof(u64) * settings.u64_reg_size, __builtin_object_size (result.u64_registers, 0));
    __builtin___memset_chk (result.f32_registers, 0, sizeof(f32) * settings.f32_reg_size, __builtin_object_size (result.f32_registers, 0));
    __builtin___memset_chk (result.f64_registers, 0, sizeof(f64) * settings.f64_reg_size, __builtin_object_size (result.f64_registers, 0));

    if (ret_code != ((void *)0))
        *ret_code = 0;
    return result;
}


__attribute__((visibility("default"))) void ncvm_thread_free(ncvm_thread* thread) {
    stack_byte_free((stack_byte*)thread->stack_p);
    stack_usize_free((stack_usize*)thread->call_stack_p);
    free(thread->u32_registers);
    free(thread->u64_registers);
    free(thread->f32_registers);
    free(thread->f64_registers);

}

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 1 3 4
# 64 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_stdio.h" 1 3 4
# 75 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_stdio.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_va_list.h" 1 3 4
# 32 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_va_list.h" 3 4
typedef __darwin_va_list va_list;
# 76 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_stdio.h" 2 3 4



# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/stdio.h" 1 3 4
# 47 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/stdio.h" 3 4
int renameat(int, const char *, int, const char *) __attribute__((availability(macosx,introduced=10.10)));



int renamex_np(const char *, const char *, unsigned int) __attribute__((availability(macosx,introduced=10.12))) __attribute__((availability(ios,introduced=10.0))) __attribute__((availability(tvos,introduced=10.0))) __attribute__((availability(watchos,introduced=3.0)));
int renameatx_np(int, const char *, int, const char *, unsigned int) __attribute__((availability(macosx,introduced=10.12))) __attribute__((availability(ios,introduced=10.0))) __attribute__((availability(tvos,introduced=10.0))) __attribute__((availability(watchos,introduced=3.0)));
# 80 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_stdio.h" 2 3 4

typedef __darwin_off_t fpos_t;
# 92 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_stdio.h" 3 4
struct __sbuf {
 unsigned char *_base;
 int _size;
};


struct __sFILEX;
# 126 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_stdio.h" 3 4
typedef struct __sFILE {
 unsigned char *_p;
 int _r;
 int _w;
 short _flags;
 short _file;
 struct __sbuf _bf;
 int _lbfsize;


 void *_cookie;
 int (* _Nullable _close)(void *);
 int (* _Nullable _read) (void *, char *, int);
 fpos_t (* _Nullable _seek) (void *, fpos_t, int);
 int (* _Nullable _write)(void *, const char *, int);


 struct __sbuf _ub;
 struct __sFILEX *_extra;
 int _ur;


 unsigned char _ubuf[3];
 unsigned char _nbuf[1];


 struct __sbuf _lb;


 int _blksize;
 fpos_t _offset;
} FILE;
# 65 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 2 3 4

# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_seek_set.h" 1 3 4
# 67 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 2 3 4


extern FILE *__stdinp;
extern FILE *__stdoutp;
extern FILE *__stderrp;
# 134 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
void clearerr(FILE *);
int fclose(FILE *);
int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
int fgetc(FILE *);
int fgetpos(FILE * , fpos_t *);
char *fgets(char * , int, FILE *);



FILE *fopen(const char * __filename, const char * __mode) __asm("_" "fopen" );

int fprintf(FILE * , const char * , ...) __attribute__((__format__ (__printf__, 2, 3)));
int fputc(int, FILE *);
int fputs(const char * , FILE * ) __asm("_" "fputs" );
size_t fread(void * __ptr, size_t __size, size_t __nitems, FILE * __stream);
FILE *freopen(const char * , const char * ,
                 FILE * ) __asm("_" "freopen" );
int fscanf(FILE * , const char * , ...) __attribute__((__format__ (__scanf__, 2, 3)));
int fseek(FILE *, long, int);
int fsetpos(FILE *, const fpos_t *);
long ftell(FILE *);
size_t fwrite(const void * __ptr, size_t __size, size_t __nitems, FILE * __stream) __asm("_" "fwrite" );
int getc(FILE *);
int getchar(void);


__attribute__((__deprecated__("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of gets(3), it is highly recommended that you use fgets(3) instead.")))

char *gets(char *);

void perror(const char *) __attribute__((__cold__));
int printf(const char * , ...) __attribute__((__format__ (__printf__, 1, 2)));
int putc(int, FILE *);
int putchar(int);
int puts(const char *);
int remove(const char *);
int rename (const char *__old, const char *__new);
void rewind(FILE *);
int scanf(const char * , ...) __attribute__((__format__ (__scanf__, 1, 2)));
void setbuf(FILE * , char * );
int setvbuf(FILE * , char * , int, size_t);

__attribute__((__availability__(swift, unavailable, message="Use snprintf instead.")))

__attribute__((__deprecated__("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of sprintf(3), it is highly recommended that you use snprintf(3) instead.")))

int sprintf(char * , const char * , ...) __attribute__((__format__ (__printf__, 2, 3)));

int sscanf(const char * , const char * , ...) __attribute__((__format__ (__scanf__, 2, 3)));
FILE *tmpfile(void);

__attribute__((__availability__(swift, unavailable, message="Use mkstemp(3) instead.")))

__attribute__((__deprecated__("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of tmpnam(3), it is highly recommended that you use mkstemp(3) instead.")))

char *tmpnam(char *);

int ungetc(int, FILE *);
int vfprintf(FILE * , const char * , va_list) __attribute__((__format__ (__printf__, 2, 0)));
int vprintf(const char * , va_list) __attribute__((__format__ (__printf__, 1, 0)));

__attribute__((__availability__(swift, unavailable, message="Use vsnprintf instead.")))

__attribute__((__deprecated__("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of sprintf(3), it is highly recommended that you use vsnprintf(3) instead.")))

int vsprintf(char * , const char * , va_list) __attribute__((__format__ (__printf__, 2, 0)));
# 213 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_ctermid.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_ctermid.h" 3 4
char *ctermid(char *);
# 214 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 2 3 4






FILE *fdopen(int, const char *) __asm("_" "fdopen" );

int fileno(FILE *);
# 233 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
int pclose(FILE *) __attribute__((__availability__(swift, unavailable, message="Use posix_spawn APIs or NSTask instead. (On iOS, process spawning is unavailable.)")));



FILE *popen(const char *, const char *) __asm("_" "popen" ) __attribute__((__availability__(swift, unavailable, message="Use posix_spawn APIs or NSTask instead. (On iOS, process spawning is unavailable.)")));
# 252 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
int __srget(FILE *);
int __svfscanf(FILE *, const char *, va_list) __attribute__((__format__ (__scanf__, 2, 0)));
int __swbuf(int, FILE *);
# 263 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
extern __inline __attribute__ ((__always_inline__)) int __sputc(int _c, FILE *_p) {
 if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))
  return (*_p->_p++ = _c);
 else
  return (__swbuf(_c, _p));
}
# 289 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
void flockfile(FILE *);
int ftrylockfile(FILE *);
void funlockfile(FILE *);
int getc_unlocked(FILE *);
int getchar_unlocked(void);
int putc_unlocked(int, FILE *);
int putchar_unlocked(int);



int getw(FILE *);
int putw(int, FILE *);


__attribute__((__availability__(swift, unavailable, message="Use mkstemp(3) instead.")))

__attribute__((__deprecated__("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of tempnam(3), it is highly recommended that you use mkstemp(3) instead.")))

char *tempnam(const char *__dir, const char *__prefix) __asm("_" "tempnam" );
# 327 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_off_t.h" 1 3 4
# 31 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_off_t.h" 3 4
typedef __darwin_off_t off_t;
# 328 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 2 3 4


int fseeko(FILE * __stream, off_t __offset, int __whence);
off_t ftello(FILE * __stream);





int snprintf(char * __str, size_t __size, const char * __format, ...) __attribute__((__format__ (__printf__, 3, 4)));
int vfscanf(FILE * __stream, const char * __format, va_list) __attribute__((__format__ (__scanf__, 2, 0)));
int vscanf(const char * __format, va_list) __attribute__((__format__ (__scanf__, 1, 0)));
int vsnprintf(char * __str, size_t __size, const char * __format, va_list) __attribute__((__format__ (__printf__, 3, 0)));
int vsscanf(const char * __str, const char * __format, va_list) __attribute__((__format__ (__scanf__, 2, 0)));
# 355 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
int dprintf(int, const char * , ...) __attribute__((__format__ (__printf__, 2, 3))) __attribute__((availability(macosx,introduced=10.7)));
int vdprintf(int, const char * , va_list) __attribute__((__format__ (__printf__, 2, 0))) __attribute__((availability(macosx,introduced=10.7)));
ssize_t getdelim(char ** __linep, size_t * __linecapp, int __delimiter, FILE * __stream) __attribute__((availability(macosx,introduced=10.7)));
ssize_t getline(char ** __linep, size_t * __linecapp, FILE * __stream) __attribute__((availability(macosx,introduced=10.7)));
FILE *fmemopen(void * __buf, size_t __size, const char * __mode) __attribute__((availability(macos,introduced=10.13))) __attribute__((availability(ios,introduced=11.0))) __attribute__((availability(tvos,introduced=11.0))) __attribute__((availability(watchos,introduced=4.0)));
FILE *open_memstream(char **__bufp, size_t *__sizep) __attribute__((availability(macos,introduced=10.13))) __attribute__((availability(ios,introduced=11.0))) __attribute__((availability(tvos,introduced=11.0))) __attribute__((availability(watchos,introduced=4.0)));
# 370 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
extern const int sys_nerr;
extern const char *const sys_errlist[];

int asprintf(char ** , const char * , ...) __attribute__((__format__ (__printf__, 2, 3)));
char *ctermid_r(char *);
char *fgetln(FILE *, size_t *);
const char *fmtcheck(const char *, const char *) __attribute__((format_arg(2)));
int fpurge(FILE *);
void setbuffer(FILE *, char *, int);
int setlinebuf(FILE *);
int vasprintf(char ** , const char * , va_list) __attribute__((__format__ (__printf__, 2, 0)));





FILE *funopen(const void *,
                 int (* _Nullable)(void *, char *, int),
                 int (* _Nullable)(void *, const char *, int),
                 fpos_t (* _Nullable)(void *, fpos_t, int),
                 int (* _Nullable)(void *));
# 409 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 3 4
# 1 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_stdio.h" 1 3 4
# 42 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_stdio.h" 3 4
extern int __sprintf_chk (char * , int, size_t,
     const char * , ...);
# 52 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/secure/_stdio.h" 3 4
extern int __snprintf_chk (char * , size_t, int, size_t,
      const char * , ...);







extern int __vsprintf_chk (char * , int, size_t,
      const char * , va_list);







extern int __vsnprintf_chk (char * , size_t, int, size_t,
       const char * , va_list);
# 410 "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h" 2 3 4
# 119 "src/ncvm.c" 2
# 605 "src/ncvm.c"
__attribute__((visibility("default"))) u8 ncvm_execute_thread_step(ncvm_thread* thread) {
    u32* u32_registers = thread->u32_registers;
    u64* u64_registers = thread->u64_registers;
    f32* f32_registers = thread->f32_registers;
    f64* f64_registers = thread->f64_registers;
    stack_byte* stack = (stack_byte*)thread->stack_p;
    stack_usize* call_stack = (stack_usize*)thread->call_stack_p;
    ncvm *vm = thread->vm;

    u64* const addr_register = &thread->u64_registers[0];
    const Instruction* IP = thread->current_instr_p;

    switch (IP->opcode) { case STOP: goto while_exit; break; case RET: if (stack_usize_pop((stack_usize*)call_stack, (usize*)addr_register) == 0) goto while_exit; IP = &vm->inst_p[*addr_register] - 1; break; case IMOV: u32_registers[IP->r1] = u32_registers[IP->r2]; break; case LMOV: u64_registers[IP->r1] = u64_registers[IP->r2]; break; case FMOV: f32_registers[IP->r1] = f32_registers[IP->r2]; break; case DMOV: f64_registers[IP->r1] = f64_registers[IP->r2]; break; case IRCLR: u32_registers[IP->r1] = 0; break; case LRCLR: u64_registers[IP->r1] = 0; break; case FRCLR: f32_registers[IP->r1] = 0; break; case DRCLR: f64_registers[IP->r1] = 0; break; case ISR: *((u8*)(u32_registers+IP->r1)) = IP->r2; *((u8*)(u32_registers+IP->r1)+1) = IP->r3; break; case LSR: *((u8*)(u64_registers+IP->r1)) = IP->r2; *((u8*)(u64_registers+IP->r1)+1) = IP->r3; break; case IRSI: u32_registers[IP->r1] >>= IP->r2; break; case ILSI: u32_registers[IP->r1] <<= IP->r2; break; case LRSI: u64_registers[IP->r1] >>= IP->r2; break; case LLSI: u64_registers[IP->r1] <<= IP->r2; break; case IRSA: u32_registers[IP->r1] >>= *addr_register; break; case ILSA: u32_registers[IP->r1] <<= *addr_register; break; case LRSA: u64_registers[IP->r1] >>= *addr_register; break; case LLSA: u64_registers[IP->r1] <<= *addr_register; break; case ISMLD: u32_registers[IP->r1] = 0; __builtin___memcpy_chk (&u32_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2, __builtin_object_size (&u32_registers[IP->r1], 0)); break; case ISMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &u32_registers[IP->r1], IP->r2, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case LSMLD: u64_registers[IP->r1] = 0; __builtin___memcpy_chk (&u64_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2, __builtin_object_size (&u64_registers[IP->r1], 0)); break; case LSMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &u64_registers[IP->r1], IP->r2, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case FSMLD: __builtin___memcpy_chk (&f32_registers[IP->r1], &vm->static_mem_p[*addr_register], 4, __builtin_object_size (&f32_registers[IP->r1], 0)); break; case FSMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &f32_registers[IP->r1], 4, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case DSMLD: __builtin___memcpy_chk (&f64_registers[IP->r1], &vm->static_mem_p[*addr_register], 8, __builtin_object_size (&f64_registers[IP->r1], 0)); break; case DSMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &f64_registers[IP->r1], 8, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case POPI: stack_byte_pop_ptr(stack, IP->r1 + (IP->r2*256) + (IP->r3*65536), ((void *)0)); break; case POPA: stack_byte_pop_ptr(stack, *addr_register, ((void *)0)); break; case IPUSH: stack_byte_push_ptr(stack, (u8*)&u32_registers[IP->r1], IP->r2); break; case ISTLD: u32_registers[IP->r1] = 0; __builtin___memcpy_chk (&u32_registers[IP->r1], &stack->data[*addr_register], IP->r2, __builtin_object_size (&u32_registers[IP->r1], 0)); break; case ISTST: __builtin___memcpy_chk (&stack->data[*addr_register], &u32_registers[IP->r1], IP->r2, __builtin_object_size (&stack->data[*addr_register], 0)); break; case LPUSH: stack_byte_push_ptr(stack, (u8*)&u64_registers[IP->r1], IP->r2); break; case LSTLD: u64_registers[IP->r1] = 0; __builtin___memcpy_chk (&u64_registers[IP->r1], &stack->data[*addr_register], IP->r2, __builtin_object_size (&u64_registers[IP->r1], 0)); break; case LSTST: __builtin___memcpy_chk (&stack->data[*addr_register], &u64_registers[IP->r1], IP->r2, __builtin_object_size (&stack->data[*addr_register], 0)); break; case FPUSH: stack_byte_push_ptr(stack, (u8*)&f32_registers[IP->r1], 4); break; case FSTLD: __builtin___memcpy_chk (&f32_registers[IP->r1], &stack->data[*addr_register], 4, __builtin_object_size (&f32_registers[IP->r1], 0)); break; case FSTST: __builtin___memcpy_chk (&stack->data[*addr_register], &f32_registers[IP->r1], 4, __builtin_object_size (&stack->data[*addr_register], 0)); break; case DPUSH: stack_byte_push_ptr(stack, (u8*)&f64_registers[IP->r1], 8); break; case DSTLD: __builtin___memcpy_chk (&f64_registers[IP->r1], &stack->data[*addr_register], 8, __builtin_object_size (&f64_registers[IP->r1], 0)); break; case DSTST: __builtin___memcpy_chk (&stack->data[*addr_register], &f64_registers[IP->r1], 8, __builtin_object_size (&stack->data[*addr_register], 0)); break; case IADD: u32_registers[IP->r1] = u32_registers[IP->r2] + u32_registers[IP->r3]; break; case ISUB: u32_registers[IP->r1] = u32_registers[IP->r2] - u32_registers[IP->r3]; break; case IMULT: u32_registers[IP->r1] = u32_registers[IP->r2] * u32_registers[IP->r3]; break; case IDIV: u32_registers[IP->r1] = u32_registers[IP->r2] / u32_registers[IP->r3]; break; case IMOD: u32_registers[IP->r1] = u32_registers[IP->r2] % u32_registers[IP->r3]; break; case IINC: ++u32_registers[IP->r1]; break; case IDEC: --u32_registers[IP->r1]; break; case LADD: u64_registers[IP->r1] = u64_registers[IP->r2] + u64_registers[IP->r3]; break; case LSUB: u64_registers[IP->r1] = u64_registers[IP->r2] - u64_registers[IP->r3]; break; case LMULT: u64_registers[IP->r1] = u64_registers[IP->r2] * u64_registers[IP->r3]; break; case LDIV: u64_registers[IP->r1] = u64_registers[IP->r2] / u64_registers[IP->r3]; break; case LMOD: u64_registers[IP->r1] = u64_registers[IP->r2] % u64_registers[IP->r3]; break; case LINC: ++u64_registers[IP->r1]; break; case LDEC: --u64_registers[IP->r1]; break; case FADD: f32_registers[IP->r1] = f32_registers[IP->r2] + f32_registers[IP->r3]; break; case FSUB: f32_registers[IP->r1] = f32_registers[IP->r2] - f32_registers[IP->r3]; break; case FMULT: f32_registers[IP->r1] = f32_registers[IP->r2] * f32_registers[IP->r3]; break; case FDIV: f32_registers[IP->r1] = f32_registers[IP->r2] / f32_registers[IP->r3]; break; case FINC: ++f32_registers[IP->r1]; break; case FDEC: --f32_registers[IP->r1]; break; case DADD: f64_registers[IP->r1] = f64_registers[IP->r2] + f64_registers[IP->r3]; break; case DSUB: f64_registers[IP->r1] = f64_registers[IP->r2] - f64_registers[IP->r3]; break; case DMULT: f64_registers[IP->r1] = f64_registers[IP->r2] * f64_registers[IP->r3]; break; case DDIV: f64_registers[IP->r1] = f64_registers[IP->r2] / f64_registers[IP->r3]; break; case DINC: ++f64_registers[IP->r1]; break; case DDEC: --f64_registers[IP->r1]; break; case LTOI: u32_registers[IP->r1] = (u32)u64_registers[IP->r2]; break; case FTOI: u32_registers[IP->r1] = (u32)f32_registers[IP->r2]; break; case DTOI: u32_registers[IP->r1] = (u32)f64_registers[IP->r2]; break; case ITOL: u64_registers[IP->r1] = (u64)u32_registers[IP->r2]; break; case FTOL: u64_registers[IP->r1] = (u64)f32_registers[IP->r2]; break; case DTOL: u64_registers[IP->r1] = (u64)f64_registers[IP->r2]; break; case ITOF: f32_registers[IP->r1] = (f32)u32_registers[IP->r2]; break; case LTOF: f32_registers[IP->r1] = (f32)u64_registers[IP->r2]; break; case DTOF: f32_registers[IP->r1] = (f32)f64_registers[IP->r2]; break; case ITOD: f64_registers[IP->r1] = (f64)u32_registers[IP->r2]; break; case LTOD: f64_registers[IP->r1] = (f64)u64_registers[IP->r2]; break; case FTOD: f64_registers[IP->r1] = (f64)f32_registers[IP->r2]; break; case JMP: IP = &vm->inst_p[*addr_register] - 1; break; case IJEZ: if (u32_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJNZ: if (u32_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJEQ: if (u32_registers[IP->r1] == u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJNQ: if (u32_registers[IP->r1] != u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJML: if (u32_registers[IP->r1] < u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJEL: if (u32_registers[IP->r1] <= u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJMG: if (u32_registers[IP->r1] > u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJEG: if (u32_registers[IP->r1] >= u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEZ: if (u64_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJNZ: if (u64_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEQ: if (u64_registers[IP->r1] == u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJNQ: if (u64_registers[IP->r1] != u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJML: if (u64_registers[IP->r1] < u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEL: if (u64_registers[IP->r1] <= u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJMG: if (u64_registers[IP->r1] > u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEG: if (u64_registers[IP->r1] >= u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEZ: if (f32_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJNZ: if (f32_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEQ: if (f32_registers[IP->r1] == f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJNQ: if (f32_registers[IP->r1] != f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJML: if (f32_registers[IP->r1] < f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEL: if (f32_registers[IP->r1] <= f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJMG: if (f32_registers[IP->r1] > f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEG: if (f32_registers[IP->r1] >= f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEZ: if (f64_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJNZ: if (f64_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEQ: if (f64_registers[IP->r1] == f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJNQ: if (f64_registers[IP->r1] != f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJML: if (f64_registers[IP->r1] < f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEL: if (f64_registers[IP->r1] <= f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJMG: if (f64_registers[IP->r1] > f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEG: if (f64_registers[IP->r1] >= f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case CALL: if (stack_usize_push((stack_usize*)call_stack, IP - vm->inst_p + 1) == 0) return 1; IP = &vm->inst_p[*addr_register] - 1; break; case LIBCALL: ((ncvm_lib_function)vm->lib_functions[*addr_register])(thread); break; } ++IP;

    while_exit:;
    thread->current_instr_p = IP;
    return 0;
}


__attribute__((visibility("default"))) u8 ncvm_execute_thread(ncvm_thread* thread) {
    u32* u32_registers = thread->u32_registers;
    u64* u64_registers = thread->u64_registers;
    f32* f32_registers = thread->f32_registers;
    f64* f64_registers = thread->f64_registers;
    stack_byte* stack = (stack_byte*)thread->stack_p;
    stack_usize* call_stack = (stack_usize*)thread->call_stack_p;
    ncvm *vm = thread->vm;

    u64* const addr_register = &thread->u64_registers[0];
    register const Instruction* IP = thread->current_instr_p;
    while (1) {
        switch (IP->opcode) { case STOP: goto while_exit; break; case RET: if (stack_usize_pop((stack_usize*)call_stack, (usize*)addr_register) == 0) goto while_exit; IP = &vm->inst_p[*addr_register] - 1; break; case IMOV: u32_registers[IP->r1] = u32_registers[IP->r2]; break; case LMOV: u64_registers[IP->r1] = u64_registers[IP->r2]; break; case FMOV: f32_registers[IP->r1] = f32_registers[IP->r2]; break; case DMOV: f64_registers[IP->r1] = f64_registers[IP->r2]; break; case IRCLR: u32_registers[IP->r1] = 0; break; case LRCLR: u64_registers[IP->r1] = 0; break; case FRCLR: f32_registers[IP->r1] = 0; break; case DRCLR: f64_registers[IP->r1] = 0; break; case ISR: *((u8*)(u32_registers+IP->r1)) = IP->r2; *((u8*)(u32_registers+IP->r1)+1) = IP->r3; break; case LSR: *((u8*)(u64_registers+IP->r1)) = IP->r2; *((u8*)(u64_registers+IP->r1)+1) = IP->r3; break; case IRSI: u32_registers[IP->r1] >>= IP->r2; break; case ILSI: u32_registers[IP->r1] <<= IP->r2; break; case LRSI: u64_registers[IP->r1] >>= IP->r2; break; case LLSI: u64_registers[IP->r1] <<= IP->r2; break; case IRSA: u32_registers[IP->r1] >>= *addr_register; break; case ILSA: u32_registers[IP->r1] <<= *addr_register; break; case LRSA: u64_registers[IP->r1] >>= *addr_register; break; case LLSA: u64_registers[IP->r1] <<= *addr_register; break; case ISMLD: u32_registers[IP->r1] = 0; __builtin___memcpy_chk (&u32_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2, __builtin_object_size (&u32_registers[IP->r1], 0)); break; case ISMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &u32_registers[IP->r1], IP->r2, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case LSMLD: u64_registers[IP->r1] = 0; __builtin___memcpy_chk (&u64_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2, __builtin_object_size (&u64_registers[IP->r1], 0)); break; case LSMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &u64_registers[IP->r1], IP->r2, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case FSMLD: __builtin___memcpy_chk (&f32_registers[IP->r1], &vm->static_mem_p[*addr_register], 4, __builtin_object_size (&f32_registers[IP->r1], 0)); break; case FSMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &f32_registers[IP->r1], 4, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case DSMLD: __builtin___memcpy_chk (&f64_registers[IP->r1], &vm->static_mem_p[*addr_register], 8, __builtin_object_size (&f64_registers[IP->r1], 0)); break; case DSMST: __builtin___memcpy_chk (&vm->static_mem_p[*addr_register], &f64_registers[IP->r1], 8, __builtin_object_size (&vm->static_mem_p[*addr_register], 0)); break; case POPI: stack_byte_pop_ptr(stack, IP->r1 + (IP->r2*256) + (IP->r3*65536), ((void *)0)); break; case POPA: stack_byte_pop_ptr(stack, *addr_register, ((void *)0)); break; case IPUSH: stack_byte_push_ptr(stack, (u8*)&u32_registers[IP->r1], IP->r2); break; case ISTLD: u32_registers[IP->r1] = 0; __builtin___memcpy_chk (&u32_registers[IP->r1], &stack->data[*addr_register], IP->r2, __builtin_object_size (&u32_registers[IP->r1], 0)); break; case ISTST: __builtin___memcpy_chk (&stack->data[*addr_register], &u32_registers[IP->r1], IP->r2, __builtin_object_size (&stack->data[*addr_register], 0)); break; case LPUSH: stack_byte_push_ptr(stack, (u8*)&u64_registers[IP->r1], IP->r2); break; case LSTLD: u64_registers[IP->r1] = 0; __builtin___memcpy_chk (&u64_registers[IP->r1], &stack->data[*addr_register], IP->r2, __builtin_object_size (&u64_registers[IP->r1], 0)); break; case LSTST: __builtin___memcpy_chk (&stack->data[*addr_register], &u64_registers[IP->r1], IP->r2, __builtin_object_size (&stack->data[*addr_register], 0)); break; case FPUSH: stack_byte_push_ptr(stack, (u8*)&f32_registers[IP->r1], 4); break; case FSTLD: __builtin___memcpy_chk (&f32_registers[IP->r1], &stack->data[*addr_register], 4, __builtin_object_size (&f32_registers[IP->r1], 0)); break; case FSTST: __builtin___memcpy_chk (&stack->data[*addr_register], &f32_registers[IP->r1], 4, __builtin_object_size (&stack->data[*addr_register], 0)); break; case DPUSH: stack_byte_push_ptr(stack, (u8*)&f64_registers[IP->r1], 8); break; case DSTLD: __builtin___memcpy_chk (&f64_registers[IP->r1], &stack->data[*addr_register], 8, __builtin_object_size (&f64_registers[IP->r1], 0)); break; case DSTST: __builtin___memcpy_chk (&stack->data[*addr_register], &f64_registers[IP->r1], 8, __builtin_object_size (&stack->data[*addr_register], 0)); break; case IADD: u32_registers[IP->r1] = u32_registers[IP->r2] + u32_registers[IP->r3]; break; case ISUB: u32_registers[IP->r1] = u32_registers[IP->r2] - u32_registers[IP->r3]; break; case IMULT: u32_registers[IP->r1] = u32_registers[IP->r2] * u32_registers[IP->r3]; break; case IDIV: u32_registers[IP->r1] = u32_registers[IP->r2] / u32_registers[IP->r3]; break; case IMOD: u32_registers[IP->r1] = u32_registers[IP->r2] % u32_registers[IP->r3]; break; case IINC: ++u32_registers[IP->r1]; break; case IDEC: --u32_registers[IP->r1]; break; case LADD: u64_registers[IP->r1] = u64_registers[IP->r2] + u64_registers[IP->r3]; break; case LSUB: u64_registers[IP->r1] = u64_registers[IP->r2] - u64_registers[IP->r3]; break; case LMULT: u64_registers[IP->r1] = u64_registers[IP->r2] * u64_registers[IP->r3]; break; case LDIV: u64_registers[IP->r1] = u64_registers[IP->r2] / u64_registers[IP->r3]; break; case LMOD: u64_registers[IP->r1] = u64_registers[IP->r2] % u64_registers[IP->r3]; break; case LINC: ++u64_registers[IP->r1]; break; case LDEC: --u64_registers[IP->r1]; break; case FADD: f32_registers[IP->r1] = f32_registers[IP->r2] + f32_registers[IP->r3]; break; case FSUB: f32_registers[IP->r1] = f32_registers[IP->r2] - f32_registers[IP->r3]; break; case FMULT: f32_registers[IP->r1] = f32_registers[IP->r2] * f32_registers[IP->r3]; break; case FDIV: f32_registers[IP->r1] = f32_registers[IP->r2] / f32_registers[IP->r3]; break; case FINC: ++f32_registers[IP->r1]; break; case FDEC: --f32_registers[IP->r1]; break; case DADD: f64_registers[IP->r1] = f64_registers[IP->r2] + f64_registers[IP->r3]; break; case DSUB: f64_registers[IP->r1] = f64_registers[IP->r2] - f64_registers[IP->r3]; break; case DMULT: f64_registers[IP->r1] = f64_registers[IP->r2] * f64_registers[IP->r3]; break; case DDIV: f64_registers[IP->r1] = f64_registers[IP->r2] / f64_registers[IP->r3]; break; case DINC: ++f64_registers[IP->r1]; break; case DDEC: --f64_registers[IP->r1]; break; case LTOI: u32_registers[IP->r1] = (u32)u64_registers[IP->r2]; break; case FTOI: u32_registers[IP->r1] = (u32)f32_registers[IP->r2]; break; case DTOI: u32_registers[IP->r1] = (u32)f64_registers[IP->r2]; break; case ITOL: u64_registers[IP->r1] = (u64)u32_registers[IP->r2]; break; case FTOL: u64_registers[IP->r1] = (u64)f32_registers[IP->r2]; break; case DTOL: u64_registers[IP->r1] = (u64)f64_registers[IP->r2]; break; case ITOF: f32_registers[IP->r1] = (f32)u32_registers[IP->r2]; break; case LTOF: f32_registers[IP->r1] = (f32)u64_registers[IP->r2]; break; case DTOF: f32_registers[IP->r1] = (f32)f64_registers[IP->r2]; break; case ITOD: f64_registers[IP->r1] = (f64)u32_registers[IP->r2]; break; case LTOD: f64_registers[IP->r1] = (f64)u64_registers[IP->r2]; break; case FTOD: f64_registers[IP->r1] = (f64)f32_registers[IP->r2]; break; case JMP: IP = &vm->inst_p[*addr_register] - 1; break; case IJEZ: if (u32_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJNZ: if (u32_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJEQ: if (u32_registers[IP->r1] == u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJNQ: if (u32_registers[IP->r1] != u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJML: if (u32_registers[IP->r1] < u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJEL: if (u32_registers[IP->r1] <= u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJMG: if (u32_registers[IP->r1] > u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case IJEG: if (u32_registers[IP->r1] >= u32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEZ: if (u64_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJNZ: if (u64_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEQ: if (u64_registers[IP->r1] == u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJNQ: if (u64_registers[IP->r1] != u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJML: if (u64_registers[IP->r1] < u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEL: if (u64_registers[IP->r1] <= u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJMG: if (u64_registers[IP->r1] > u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case LJEG: if (u64_registers[IP->r1] >= u64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEZ: if (f32_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJNZ: if (f32_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEQ: if (f32_registers[IP->r1] == f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJNQ: if (f32_registers[IP->r1] != f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJML: if (f32_registers[IP->r1] < f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEL: if (f32_registers[IP->r1] <= f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJMG: if (f32_registers[IP->r1] > f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case FJEG: if (f32_registers[IP->r1] >= f32_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEZ: if (f64_registers[IP->r1] == 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJNZ: if (f64_registers[IP->r1] != 0) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEQ: if (f64_registers[IP->r1] == f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJNQ: if (f64_registers[IP->r1] != f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJML: if (f64_registers[IP->r1] < f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEL: if (f64_registers[IP->r1] <= f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJMG: if (f64_registers[IP->r1] > f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case DJEG: if (f64_registers[IP->r1] >= f64_registers[IP->r2]) { IP = &vm->inst_p[*addr_register] - 1; } break; case CALL: if (stack_usize_push((stack_usize*)call_stack, IP - vm->inst_p + 1) == 0) return 1; IP = &vm->inst_p[*addr_register] - 1; break; case LIBCALL: ((ncvm_lib_function)vm->lib_functions[*addr_register])(thread); break; } ++IP;
    }
    while_exit:;
    thread->current_instr_p = IP;
    return 0;
}
