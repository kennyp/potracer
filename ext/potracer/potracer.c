#include "ruby.h"
#include "extconf.h"
#include "potracelib.h"

// Tracing
static VALUE rb_mPotrace;
static VALUE rb_cPotraceTrace;

static VALUE trace_alloc (VALUE klass, VALUE bitmap, VALUE params) {
  const potrace_bitmap_t *bm;
  const potrace_param_t *param;
  potrace_state_t *trace;

  Data_Get_Struct(bitmap, potrace_bitmap_t, bm);
  Data_Get_Struct(params, potrace_param_t, param);

  trace = potrace_trace(param, bm);

  return Data_Wrap_Struct(klass, 0, potrace_state_free, trace);
}

void Init_potracer () {
  // Define the Potrace module
  rb_mPotrace = rb_define_module("Potrace");
  rb_define_const(rb_mPotrace, "VERSION", rb_str_new2(potrace_version()));

  // Define the Trace class inside the Potrace module
  rb_cPotraceTrace = rb_define_class_under(rb_mPotrace, "Trace", rb_cObject);
  rb_define_singleton_method(rb_cPotraceTrace, "new", trace_alloc, 2);
}
