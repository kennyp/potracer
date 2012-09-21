#include "ruby.h"
#include "extconf.h"
#include "potracelib.h"

// Tracing
static VALUE rb_mPotracer;
static VALUE rb_cPotracerTrace;
static VALUE rb_cPotracerParams;

static VALUE params_alloc (VALUE klass) {
    potrace_param_t *params = potrace_param_default();
    return Data_Wrap_Struct(klass, 0, potrace_param_free, params);
}

static VALUE params_get_turdsize (VALUE klass) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    return rb_int_new(params->turdsize);
}

static VALUE params_set_turdsize (VALUE klass, VALUE size) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    params->turdsize = NUM2INT(size);
    return size;
}

static VALUE params_get_turnpolicy (VALUE klass) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    return rb_int_new(params->turnpolicy);
}

static VALUE params_set_turnpolicy (VALUE klass, VALUE policy) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    params->turdsize = NUM2INT(policy);
    return policy;
}

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
  // Define the Potracer module
  rb_mPotracer = rb_define_module("Potracer");
  rb_define_const(rb_mPotracer, "VERSION", rb_str_new2(potrace_version()));

  // Define the Trace class inside the Potracer module
  rb_cPotracerTrace = rb_define_class_under(rb_mPotracer, "Trace", rb_cObject);
  rb_define_singleton_method(rb_cPotracerTrace, "new", trace_alloc, 2);

  // Define the Params class inside the Potracer module
  rb_cPotracerParams = rb_define_class_under(rb_mPotracer, "Params", rb_cObject);
  rb_define_alloc_func(rb_cPotracerParams, params_alloc);
  rb_define_method(rb_cPotracerParams, "turd_size", params_get_turdsize, 0);
  rb_define_method(rb_cPotracerParams, "turd_size=", params_set_turdsize, 1);
  rb_define_method(rb_cPotracerParams, "turn_policy", params_get_turnpolicy, 0);
  rb_define_method(rb_cPotracerParams, "turn_policy=", params_set_turnpolicy, 1);
}
