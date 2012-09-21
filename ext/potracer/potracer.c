#include "ruby.h"
#include "extconf.h"
#include "potracelib.h"

// Tracing
static VALUE rb_mPotracer;
static VALUE rb_mTurnpolicy;
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

static VALUE params_get_alphamax (VALUE klass) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    return rb_float_new(params->alphamax);
}

static VALUE params_set_alphamax (VALUE klass, VALUE max) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    params->alphamax = NUM2DBL(max);
    return max;
}

static VALUE params_get_opticurve (VALUE klass) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    return (params->opticurve == 1) ? Qtrue : Qfalse;
}

static VALUE params_set_opticurve_true (VALUE klass) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    params->opticurve = 1;
    return Qtrue;
}

static VALUE params_set_opticurve_false (VALUE klass) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    params->opticurve = 0;
    return Qfalse;
}

static VALUE params_get_opttolerance (VALUE klass) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    return rb_float_new(params->opttolerance);
}

static VALUE params_set_opttolerance (VALUE klass, VALUE tolerance) {
    potrace_param_t *params;
    Data_Get_Struct(klass, potrace_param_t, params);
    params->opttolerance = NUM2DBL(tolerance);
    return tolerance;
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
  rb_define_method(rb_cPotracerParams, "alpha_max", params_get_alphamax, 0);
  rb_define_method(rb_cPotracerParams, "alpha_max=", params_set_alphamax, 1);
  rb_define_method(rb_cPotracerParams, "optimized?", params_get_opticurve, 0);
  rb_define_method(rb_cPotracerParams, "optimize!", params_set_opticurve_true, 0);
  rb_define_method(rb_cPotracerParams, "unoptimize!", params_set_opticurve_false, 0);
  rb_define_method(rb_cPotracerParams, "tolerance", params_get_opttolerance, 0);
  rb_define_method(rb_cPotracerParams, "tolerance=", params_set_opttolerance, 1);

  // Define the Turnpolicy module inside the Potracer module
  rb_mTurnpolicy = rb_define_module_under(rb_mPotracer, "Turnpolicy");
  rb_define_const(rb_mTurnpolicy, "BLACK", rb_int_new(POTRACE_TURNPOLICY_BLACK));
  rb_define_const(rb_mTurnpolicy, "WHITE", rb_int_new(POTRACE_TURNPOLICY_WHITE));
  rb_define_const(rb_mTurnpolicy, "LEFT", rb_int_new(POTRACE_TURNPOLICY_LEFT));
  rb_define_const(rb_mTurnpolicy, "RIGHT", rb_int_new(POTRACE_TURNPOLICY_RIGHT));
  rb_define_const(rb_mTurnpolicy, "MINORITY", rb_int_new(POTRACE_TURNPOLICY_MINORITY));
  rb_define_const(rb_mTurnpolicy, "MAJORITY", rb_int_new(POTRACE_TURNPOLICY_MAJORITY));
  rb_define_const(rb_mTurnpolicy, "RANDOM", rb_int_new(POTRACE_TURNPOLICY_RANDOM));
}
