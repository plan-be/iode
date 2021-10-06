# IODE API

## Utilities

### buf.c
Allocates or extends a global buffer of at least len bytes. 
  - char *BUF_alloc(int len) :  allocates or extends a global buffer of at least len bytes. 
  - void BUF_free() : free the buffer
  - void BUF_lock() : reserve the buffer utilisation
  - void BUF_unlock() : unlock the buffer
  - char *BUF_memcpy(char *ptr, int lg) : copy the the first lg bytes following address ptr to the buffer
  - char *BUF_strcpy(char *ptr) : copy a null terminated string to the buffer
  - char *BUF_DATA : NULL or pointer to the allocated buffer

### pack.c
Packing (serialize) and unpacking (deserialize) objects.
  - void *P_create()
  - int P_free(char *ptr)
  - void *P_add(void *vptr1, void *vptr2, int lg)
  - void *P_get_ptr(void *vptr, int i)
  - OSIZE P_get_len(void *vptr, int i)
  - void *P_alloc_get_ptr(void *ptr, int p)
  - int P_nb(char *ptr)

## WS and Objects management

###  k_pack.c
Functions for "packing" and "unpacking" IODE objects.

### objs.c

###  k_ws.c

### k_cc*.c
Function to import / export IODE files from/to ascii and csv format.

## LEC 

## Estimation

## Simulation

## Reports

## 