#include "yield.h"

#include <stdlib.h>
#include <ucontext.h>

struct yield_ctx {
  ucontext_t caller;
  ucontext_t callee;
  void *value;
  char stack[];
};

int get_high(void *ptr) {
  unsigned long value = (unsigned long) ptr;
  return value >> 32;
}

int get_low(void *ptr) {
  unsigned long value = (unsigned long) ptr;
  return value & 0xFFFFFFFFl;
}

void *make_ptr(int h, int l) {
  unsigned high = h, low = l;
  unsigned long lhigh = high, llow = low;
  return (void *) (lhigh << 32 | llow);
}

struct yield_ctx *get_yield_ctx(int h, int l) {
  return make_ptr(h, l);
}

void yield_impl(struct yield_ctx *ctx, void *value) {
  ctx->value = value;
  swapcontext(&ctx->callee, &ctx->caller);
  ctx->value = 0;
}

struct yield_ctx *init_yield_ctx(void *buf,
                                 size_t buf_size,
                                 void (*yieldfn)()) {
  struct yield_ctx *rv = buf;
  rv->value = 0;
  getcontext(&rv->callee);
  rv->callee.uc_link = &rv->caller;
  rv->callee.uc_stack.ss_sp = &rv->stack;
  rv->callee.uc_stack.ss_size = buf_size - sizeof(struct yield_ctx);
  makecontext(&rv->callee, yieldfn, 2, get_high(rv), get_low(rv));
  return rv;
}

void yield_swap(struct yield_ctx *ctx) {
  swapcontext(&ctx->caller, &ctx->callee);
}

int yield_more(struct yield_ctx *ctx) {
  return ctx->value != 0;
}

void *get_yield_value(struct yield_ctx *ctx) {
  return ctx->value;
}
