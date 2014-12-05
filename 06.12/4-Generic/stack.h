#pragma once

#define STACK_OF(type) struct _stack_of_ ## type
#define STACK_F(name, type) _stack_f_ ## name ## _ ## type

#define DECLARE_STACK(type) \
  extern STACK_OF(type) *STACK_F(allocate, type)(); \
  extern void STACK_F(deallocate, type)(STACK_OF(type) *); \
  extern void STACK_F(push, type)(STACK_OF(type) *, type); \
  extern type STACK_F(top, type)(STACK_OF(type) *); \
  extern void STACK_F(pop, type)(STACK_OF(type) *);
