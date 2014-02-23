#ifndef _MAIN_H_
#define _MAIN_H_

#define OUT // parameters used to output value

typedef unsigned char vm_status;

#define VM_OK  (vm_status) 1 //
#define VM_OOM (vm_status) 2 // out of memory
#define VM_ERR (vm_status) 3


void fprintfln(FILE *file, const char *format, ...);

#endif
