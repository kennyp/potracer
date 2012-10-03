#include "ruby.h"
#include "extconf.h"
#include "potracelib.h"

#define BM_WIDTH 250
#define BM_HEIGHT 250
#define BM_WORDSIZE ((int)sizeof(potrace_word))
#define BM_WORDBITS (8*BM_WORDSIZE)
#define BM_HIBIT (((potrace_word)1)<<(BM_WORDBITS-1))
#define bm_scanline(bm, y) ((bm)->map + (y)*(bm)->dy)
#define bm_index(bm, x, y) (&bm_scanline(bm, y)[(x)/BM_WORDBITS])
#define bm_mask(x) (BM_HIBIT >> ((x) & (BM_WORDBITS-1)))
#define bm_range(x, a) ((int)(x) >= 0 && (int)(x) < (a))
#define bm_safe(bm, x, y) (bm_range(x, (bm)->w) && bm_range(y, (bm)->h))
#define BM_USET(bm, x, y) (*bm_index(bm, x, y) |= bm_mask(x))
#define BM_UCLR(bm, x, y) (*bm_index(bm, x, y) &= ~bm_mask(x))
#define BM_UPUT(bm, x, y, b) ((b) ? BM_USET(bm, x, y) : BM_UCLR(bm, x, y))
#define BM_PUT(bm, x, y, b) (bm_safe(bm, x, y) ? BM_UPUT(bm, x, y, b) : 0)
#define BM_UGET(bm, x, y) ((*bm_index(bm, x, y) & bm_mask(x)) != 0)
#define BM_GET(bm, x, y) (bm_safe(bm, x, y) ? BM_UGET(bm, x, y) : 0)

static VALUE rb_mPotracer;
static VALUE rb_mTurnpolicy;
static VALUE rb_cPotracerTrace;
static VALUE rb_cPotracerParams;
static VALUE rb_cPotracerBitmap;

static void
rb_progress (double progress, void *callback)
{
  rb_funcall((VALUE)callback, rb_intern("call"), 1, rb_float_new(progress));
}

static VALUE
params_alloc (VALUE klass)
{
  potrace_param_t *params = potrace_param_default();
  return Data_Wrap_Struct(klass, 0, potrace_param_free, params);
}

static VALUE
params_get_turdsize (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_int_new(params->turdsize);
}

static VALUE
params_set_turdsize (VALUE klass, VALUE size)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->turdsize = NUM2INT(size);
  return size;
}

static VALUE
params_get_turnpolicy (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_int_new(params->turnpolicy);
}

static VALUE
params_set_turnpolicy (VALUE klass, VALUE policy)
{
  potrace_param_t *params;
  printf("Setting Policy");
  Data_Get_Struct(klass, potrace_param_t, params);
  params->turnpolicy = NUM2INT(policy);
  return policy;
}

static VALUE
params_get_alphamax (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_float_new(params->alphamax);
}

static VALUE
params_set_alphamax (VALUE klass, VALUE max)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->alphamax = NUM2DBL(max);
  return max;
}

static VALUE
params_get_opticurve (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return (params->opticurve == 1) ? Qtrue : Qfalse;
}

static VALUE
params_set_opticurve_true (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->opticurve = 1;
  return Qtrue;
}

static VALUE
params_set_opticurve_false (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->opticurve = 0;
  return Qfalse;
}

static VALUE
params_get_opttolerance (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_float_new(params->opttolerance);
}

static VALUE
params_set_opttolerance (VALUE klass, VALUE tolerance)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->opttolerance = NUM2DBL(tolerance);
  return tolerance;
}

static void
trace_mark (potrace_state_t *state)
{
}

static VALUE
trace_alloc (VALUE klass)
{
  potrace_state_t *trace = NULL;
  return Data_Wrap_Struct(klass, trace_mark, potrace_state_free, trace);
}

static VALUE
trace_trace (VALUE obj, VALUE bitmap, VALUE params)
{
  const potrace_bitmap_t *bm;
  potrace_param_t *param;
  VALUE p;

  Data_Get_Struct(bitmap, potrace_bitmap_t, bm);
  Data_Get_Struct(params, potrace_param_t, param);

  if (rb_block_given_p()) {
    p = rb_block_proc();
    param->progress.callback = &rb_progress;
    param->progress.data = (void *)p;
  }

  DATA_PTR(obj) = potrace_trace(param, bm);

  rb_gc_mark(bitmap);
  rb_gc_mark(params);

  return Qnil;
}

static void
bitmap_mark (potrace_bitmap_t *bm)
{
}

static void
bitmap_free (potrace_bitmap_t *bm)
{
  if (bm != NULL) {
    free(bm->map);
  }
  free(bm);
}

static void
bitmap_init(int argc, VALUE *argv, VALUE klass)
{
  potrace_bitmap_t *bm;
}

static VALUE
bitmap_new (int argc, VALUE *argv, VALUE klass)
{
  potrace_bitmap_t *bm;
  VALUE bdata;
  
  bdata = Data_Make_Struct(klass, potrace_bitmap_t, bitmap_mark, bitmap_free, bm);
  bm->w = (argc > 0) ? NUM2INT(argv[0]) : BM_WIDTH;
  bm->h = (argc > 1) ? NUM2INT(argv[1]) : BM_HEIGHT;
  bm->dy = (bm->w + BM_WORDBITS - 1) / BM_WORDBITS;
  bm->map = ALLOC_N(potrace_word, bm->dy * bm->h * BM_WORDSIZE);

  if (argc > 2) {
  }

  return bdata;
}

static VALUE
bitmap_get_width (VALUE klass)
{
  potrace_bitmap_t *bm;
  Data_Get_Struct(klass, potrace_bitmap_t, bm);
  return rb_int_new(bm->w);
}

static VALUE
bitmap_get_height (VALUE klass)
{
  potrace_bitmap_t *bm;
  Data_Get_Struct(klass, potrace_bitmap_t, bm);
  return rb_int_new(bm->h);
}

static VALUE
bitmap_as_array (VALUE klass)
{
  int i, j;
  potrace_bitmap_t *bm;
  VALUE result, row;
  Data_Get_Struct(klass, potrace_bitmap_t, bm);
  result = rb_ary_new2((long)bm->h);
  for (i = 0; i < bm->w; i++) {
    row = rb_ary_new2((long)bm->w);
    for (j = 0; j < bm->h; j++) {
      rb_ary_store(row, (long)j, rb_int_new(BM_GET(bm, i, j)));
    }
    rb_ary_store(result, (long)i, row);
  }

  return result;
}

void
Init_potracer () {
  // Define the Potracer module
  rb_mPotracer = rb_define_module("Potracer");
  rb_define_const(rb_mPotracer, "VERSION", rb_str_new2(potrace_version()));

  // Define the Trace class inside the Potracer module
  rb_cPotracerTrace = rb_define_class_under(rb_mPotracer, "Trace", rb_cObject);
  rb_define_alloc_func(rb_cPotracerTrace, trace_alloc);
  rb_define_private_method(rb_cPotracerTrace, "do_trace", trace_trace, 2);

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

  // Define the Bitmap class inside the Potracer module
  rb_cPotracerBitmap = rb_define_class_under(rb_mPotracer, "Bitmap", rb_cObject);
  rb_define_singleton_method(rb_cPotracerBitmap, "new", bitmap_new, -1);
  rb_define_method(rb_cPotracerBitmap, "width", bitmap_get_width, 0);
  rb_define_method(rb_cPotracerBitmap, "height", bitmap_get_height, 0);
  rb_define_method(rb_cPotracerBitmap, "to_a", bitmap_as_array, 0);
}
