#include "ruby.h"

#include "extconf.h"
#include "potracelib.h"

#include "potracer.h"

static int
is_colored (unsigned char *str, int row, int col, int cols, int size)
{
  int i, start;
  unsigned char v, max, min;

  start = ((row * cols * size) + (col * size));
  max = min = str[start];

  for (i = 0; i < size; i++) {
    v =  str[start + i];
    min = min < v ? min : v;
    max = max > v ? max : v;
  }

  return ((min + max) / 2.0) < 128;
}

static void
rb_progress (double progress, void *callback)
{
  rb_funcall((VALUE)callback, rb_intern("call"), 1, rb_int_new(100 * progress));
}

static VALUE
params_alloc (VALUE klass)
{
  potrace_param_t *params = potrace_param_default();
  return Data_Wrap_Struct(klass, 0, potrace_param_free, params);
}

/**
 * call-seq: turd_size
 *
 * Get the current turd size
 */
static VALUE
params_get_turdsize (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_int_new(params->turdsize);
}

/**
 * call-seq: turd_size = threshold
 *
 * Set the turd size
 * * +threshold+ - used to "despeckle" the bitmap to be traced, by removing all
 *   curves whose enclosed area is below the given threshold. Default is 2.
 */
static VALUE
params_set_turdsize (VALUE klass, VALUE size)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->turdsize = NUM2INT(size);
  return size;
}

/**
 * call-seq: turn_policy
 *
 * Get the current Turnpolicy
 */
static VALUE
params_get_turnpolicy (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_int_new(params->turnpolicy);
}

/**
 * call-seq: turn_policy = policy
 *
 * Set the current Turnpolicy
 */
static VALUE
params_set_turnpolicy (VALUE klass, VALUE policy)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->turnpolicy = NUM2INT(policy);
  return policy;
}

/**
 * call-seq: alpha_max
 *
 * Get the current alpha max
 */
static VALUE
params_get_alphamax (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_float_new(params->alphamax);
}

/**
 * call-seq: alpha_max = max
 *
 * Set the alpha max
 *
 * * +max+ - threshold for the detection of corners. It controls the smoothness
 *   of the traced curve, as shown in Figure 9. The current default is 1.0. The
 *   useful range of this parameter is from 0.0 (polygon) to 1.3334 (no corners).
 */
static VALUE
params_set_alphamax (VALUE klass, VALUE max)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->alphamax = NUM2DBL(max);
  return max;
}

/**
 * call-seq: optimized?
 *
 * Are curves optimized
 */
static VALUE
params_get_opticurve (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return (params->opticurve == 1) ? Qtrue : Qfalse;
}

/**
 * call-seq: optimize!
 *
 * Turn on curve optimization
 */
static VALUE
params_set_opticurve_true (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->opticurve = 1;
  return Qtrue;
}

/**
 * call-seq: unoptimize!
 *
 * Turn off curve optimization
 */
static VALUE
params_set_opticurve_false (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  params->opticurve = 0;
  return Qfalse;
}

/**
 * call-seq: tolerance
 *
 * Get current curve optimization tolerance
 */
static VALUE
params_get_opttolerance (VALUE klass)
{
  potrace_param_t *params;
  Data_Get_Struct(klass, potrace_param_t, params);
  return rb_float_new(params->opttolerance);
}

/**
 * call-seq: tolerance = amount
 *
 * Set current curve optimization tolerance
 *
 * * +amount+ - defines the amount of error allowed in curve simplification.
 *   This value only applies if optimization is true. The current default is
 *   0.2. Larger values tend to decrease the number of segments, at the expense
 *   of less accuracy. The useful range is from 0 to infinity, although in
 *   practice one would hardly choose values greater than 1 or so. For most
 *   purposes, the default value is a good tradeoff between space and accuracy.
 */
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

  rb_iv_set(obj, "@width", INT2NUM(bm->w));
  rb_iv_set(obj, "@height", INT2NUM(bm->h));

  return obj;
}

/**
 * call-seq: to_svg
 *
 * Render the traced bitmap as an SVG
 *
 * ==== Example
 *
 *  bmp = Potracer::Bitmap.new(5, 5, [
 *    [1, 1, 1, 1, 1],
 *    [1, 0, 0, 0, 1],
 *    [1, 0, 0, 0, 1],
 *    [1, 0, 0, 0, 1],
 *    [1, 1, 1, 1, 1]
 *  ])
 *  params = Potracer::Params.new
 *  trace = Potracer::Trace.new
 *  trace.trace(bmp, params).to_svg # =>
 *  # <svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="5" height="5">
 *  #   <path fill-rule="evenodd" fill="rgb(0,0,0)" d="M 0.000000 2.500000 C 0.000000 0.500000 0.500000 0.000000 2.500000 0.000000C 4.500000 0.000000 5.000000 0.500000 5.000000 2.500000C 5.000000 4.500000 4.500000 5.000000 2.500000 5.000000C 0.500000 5.000000 0.000000 4.500000 0.000000 2.500000M 4.000000 2.500000 C 4.000000 1.675000 3.325000 1.000000 2.500000 1.000000C 1.675000 1.000000 1.000000 1.675000 1.000000 2.500000C 1.000000 3.325000 1.675000 4.000000 2.500000 4.000000C 3.325000 4.000000 4.000000 3.325000 4.000000 2.500000" />
 *  # </svg>"
 */
static VALUE
trace_as_svg (VALUE klass)
{
  FILE *out = tmpfile();
  char *out_buffer;
  long size;
  VALUE svg;
  potrace_path_t *path;
  int i, num_segments, *tag;
  potrace_dpoint_t (*c)[3];
  potrace_state_t *trace = NULL;
  Data_Get_Struct(klass, potrace_state_t, trace);

  fprintf(out, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"");
  fprintf(out, " width=\"%d\" height=\"%d\">",
          NUM2INT(rb_iv_get(klass, "@width")),
          NUM2INT(rb_iv_get(klass, "@height")));
  fprintf(out, "<path fill-rule=\"evenodd\" fill=\"rgb(0,0,0)\" d=\"");
  if (trace->status == POTRACE_STATUS_OK) {
    path = trace->plist;
    while (path != NULL) {
      num_segments = path->curve.n;
      tag = path->curve.tag;
      c = path->curve.c;
      SVG_MOVE_TO(out, c[num_segments-1][2]);
      for (i = 0; i < num_segments; i++) {
        switch (tag[i]) {
        case POTRACE_CORNER:
          SVG_LINE_TO(out, c[i][1]);
          SVG_LINE_TO(out, c[i][2]);
          break;
        case POTRACE_CURVETO:
          SVG_CURVE_TO(out, c[i]);
          break;
        }
      }
      path = path->next;
    }
  }
  fprintf(out, "\" /></svg>");

  size = ftell(out);
  out_buffer = ALLOC_N(char, size);
  rewind(out);
  fread(out_buffer, 1, size, out);
  fclose(out);
  svg = rb_str_new(out_buffer, size);
  xfree(out_buffer);
  return svg;
}

/**
 * call-seq: to_a
 *
 * Convert the traced bitmap to an array in the form:
 *
 *   [
 *     {:area=>25, :sign=>"+", :parts=> [
 *       [:moveto, 0.0, 2.5],
 *       [:curveto, 0.0, 0.5, 0.5, 0.0, 2.5, 0.0],
 *       [:curveto, 4.5, 0.0, 5.0, 0.5, 5.0, 2.5],
 *       [:curveto, 5.0, 4.5, 4.5, 5.0, 2.5, 5.0],
 *       [:curveto, 0.5, 5.0, 0.0, 4.5, 0.0, 2.5]
 *     ]},
 *     {:area=>9, :sign=>"-", :parts=> [
 *       [:moveto, 4.0, 2.5],
 *       [:curveto, 4.0, 1.6749999999999998, 3.325, 1.0, 2.5, 1.0],
 *       [:curveto, 1.6749999999999998, 1.0, 1.0, 1.6749999999999998, 1.0, 2.5],
 *       [:curveto, 1.0, 3.325, 1.6749999999999998, 4.0, 2.5, 4.0],
 *       [:curveto, 3.325, 4.0, 4.0, 3.325, 4.0, 2.5]
 *     ]}
 *   ]
 */
static VALUE
trace_as_array (VALUE klass)
{
  VALUE rpath, rparts;
  VALUE result = rb_ary_new();
  potrace_path_t *path;
  int i, num_segments, *tag;
  potrace_dpoint_t (*c)[3];
  potrace_state_t *trace = NULL;
  Data_Get_Struct(klass, potrace_state_t, trace);

  if (trace->status == POTRACE_STATUS_OK) {
    path = trace->plist;
    while (path != NULL) {
      rparts = rb_ary_new();
      num_segments = path->curve.n;
      tag = path->curve.tag;
      c = path->curve.c;
      MOVE_TO(rparts, c[num_segments-1][2]);
      for (i = 0; i < num_segments; i++) {
        switch (tag[i]) {
        case POTRACE_CORNER:
          LINE_TO(rparts, c[i][1]);
          LINE_TO(rparts, c[i][2]);
          break;
        case POTRACE_CURVETO:
          CURVE_TO(rparts, c[i]);
          break;
        }
      }
      rpath = rb_hash_new();
      rb_hash_aset(rpath, STRSYM("area"), rb_int_new(path->area));
      rb_hash_aset(rpath, STRSYM("sign"), rb_str_new2(path->sign == '+' ? "+" : "-"));
      rb_hash_aset(rpath, STRSYM("parts"), rparts);
      rb_ary_push(result, rpath);
      path = path->next;
    }
  }

  return result;
}

static void
bitmap_mark (potrace_bitmap_t *bm)
{
}

static void
bitmap_free (potrace_bitmap_t *bm)
{
  if (bm != NULL) {
    xfree(bm->map);
  }
  free(bm);
}

/**
 * call-seq: new(width=250, height=250, bits='000...', map='RGB')
 *
 * Creat a new Bitmap
 *
 * * +width+ - width of the bitmap to be traced
 * * +height+ - height of the bitmap to be traced
 * * +bits+ - bitmap data. Can be a multi-dimensional array or a string of pixel
 *   data
 * * +map+ - how pixel data is mapped if +bits+ is a string
 */
static VALUE
bitmap_new (int argc, VALUE *argv, VALUE klass)
{
  int i, j, m;
  unsigned char *bits;
  potrace_bitmap_t *bm;
  VALUE bdata, row;

  bdata = Data_Make_Struct(klass, potrace_bitmap_t, bitmap_mark, bitmap_free, bm);
  bm->w = (argc > 0) ? NUM2INT(argv[0]) : BM_WIDTH;
  bm->h = (argc > 1) ? NUM2INT(argv[1]) : BM_HEIGHT;
  bm->dy = (bm->w + BM_WORDBITS - 1) / BM_WORDBITS;
  bm->map = ALLOC_N(potrace_word, bm->dy * bm->h * BM_WORDSIZE);
  memset(bm->map, 0, bm->dy * bm->h * BM_WORDSIZE);

  if (argc > 2) {
    if (T_STRING == TYPE(argv[2])) {
      bits = (unsigned char *)StringValuePtr(argv[2]);
      m = strlen(StringValuePtr(argv[3]));
      for (i = 0; i < bm->h; i++) {
        for (j = 0; j < bm->w; j++) {
          BM_PUT(bm, j, i, is_colored(bits, i, j, bm->w, m));
        }
      }
    } else {
      for (i = 0; i < bm->h; i++) {
        row = rb_ary_entry(argv[2], (long)i);
        for (j = 0; j < bm->w; j++) {
          BM_PUT(bm, j, i, NUM2INT(rb_ary_entry(row, (long)j)));
        }
      }
    }
  }

  return bdata;
}

/**
 * call-seq: width
 *
 * Get the width in pixels
 */
static VALUE
bitmap_get_width (VALUE klass)
{
  potrace_bitmap_t *bm;
  Data_Get_Struct(klass, potrace_bitmap_t, bm);
  return rb_int_new(bm->w);
}

/**
 * call-seq: height
 *
 * Get the height in pixels
 */
static VALUE
bitmap_get_height (VALUE klass)
{
  potrace_bitmap_t *bm;
  Data_Get_Struct(klass, potrace_bitmap_t, bm);
  return rb_int_new(bm->h);
}

/**
 * call-seq: to_a
 *
 * Retrieve the bitmap data as a multi-dimensional array of 1s and 0s
 */
static VALUE
bitmap_as_array (VALUE klass)
{
  int i, j;
  potrace_bitmap_t *bm;
  VALUE result, row;
  Data_Get_Struct(klass, potrace_bitmap_t, bm);
  result = rb_ary_new2((long)bm->h);
  for (i = 0; i < bm->h; i++) {
    row = rb_ary_new2((long)bm->w);
    for (j = 0; j < bm->w; j++) {
      rb_ary_store(row, (long)j, rb_int_new(BM_GET(bm, j, i)));
    }
    rb_ary_store(result, (long)i, row);
  }

  return result;
}

void
Init_potracer () {
  /**
   * Define-module: Potracer
   * A home for all things Potrace
   */
  rb_mPotracer = rb_define_module("Potracer");
  /* VERSION: Version of Potrace */
  rb_define_const(rb_mPotracer, "VERSION", rb_str_new2(potrace_version()));

  // Define the Trace class inside the Potracer module
  rb_cPotracerTrace = rb_define_class_under(rb_mPotracer, "Trace", rb_cObject);
  rb_define_alloc_func(rb_cPotracerTrace, trace_alloc);
  rb_define_protected_method(rb_cPotracerTrace, "do_trace", trace_trace, 2);
  rb_define_method(rb_cPotracerTrace, "to_a", trace_as_array, 0);
  rb_define_method(rb_cPotracerTrace, "to_svg", trace_as_svg, 0);

  /**
   * Document-class: Potracer::Params
   *
   * Params define how the Bitmap is traced
   */
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

  /**
   * Document-const: BLACK
   *  prefers to connect black (foreground) components
   */
  /**
   * Document-const: WHITE
   *  prefers to connect white (background) components
   */
  /**
   * Document-const: LEFT
   *  always take a left turn
   */
  /**
   * Document-const: RIGHT
   *  always take a right turn
   */
  /**
   * Document-const: MINORITY
   *  prefers to connect the color (black or white) that occurs least
   *  frequently in a local neighborhood of the current position
   */
  /**
   * Document-const: MAJORITY
   *  prefers to connect the color (black or white) that occurs most
   *  frequently in a local neighborhood of the current position
   */
  /**
   * Document-const: RANDOM
   *  choose pseudo-ramdomly
   */
  /**
   * Document-class: Potracer::Turnpolicy
   *  The Turnpolicy determines how to resolve ambiguities during decomposition
   *  of bitmaps into paths.
   */
  rb_mTurnpolicy = rb_define_module_under(rb_mPotracer, "Turnpolicy");
  rb_define_const(rb_mTurnpolicy, "BLACK", rb_int_new(POTRACE_TURNPOLICY_BLACK));
  rb_define_const(rb_mTurnpolicy, "WHITE", rb_int_new(POTRACE_TURNPOLICY_WHITE));
  rb_define_const(rb_mTurnpolicy, "LEFT", rb_int_new(POTRACE_TURNPOLICY_LEFT));
  rb_define_const(rb_mTurnpolicy, "RIGHT", rb_int_new(POTRACE_TURNPOLICY_RIGHT));
  rb_define_const(rb_mTurnpolicy, "MINORITY", rb_int_new(POTRACE_TURNPOLICY_MINORITY));
  rb_define_const(rb_mTurnpolicy, "MAJORITY", rb_int_new(POTRACE_TURNPOLICY_MAJORITY));
  rb_define_const(rb_mTurnpolicy, "RANDOM", rb_int_new(POTRACE_TURNPOLICY_RANDOM));

  /**
   * Document-class: Potracer::Bitmap
   *   The Bitmap is Potracer's representation of a bitmap in memory
   */
  rb_cPotracerBitmap = rb_define_class_under(rb_mPotracer, "Bitmap", rb_cObject);
  rb_define_singleton_method(rb_cPotracerBitmap, "new", bitmap_new, -1);
  rb_define_method(rb_cPotracerBitmap, "width", bitmap_get_width, 0);
  rb_define_method(rb_cPotracerBitmap, "height", bitmap_get_height, 0);
  rb_define_method(rb_cPotracerBitmap, "to_a", bitmap_as_array, 0);
}
