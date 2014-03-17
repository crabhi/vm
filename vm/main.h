#ifndef _MAIN_H_
#define _MAIN_H_

#define IN
#define OUT // parameters used to output value

typedef unsigned char vm_status;

#define VM_OK  (vm_status) 1 //
#define VM_OOM (vm_status) 2 // out of memory
#define VM_ERR (vm_status) 3

/**
 * Used memory for code
 */
size_t code_length();

#endif
