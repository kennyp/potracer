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
#define STRSYM(s) ID2SYM(rb_intern(s))
#define RB_PAIR(p) rb_float_new(p.x), rb_float_new(p.y)
#define PUSH_TR(ar, n, s, ps...) rb_ary_push(ar, rb_ary_new3(n, STRSYM(s), ps))
#define MOVE_TO(ar, c) PUSH_TR(ar, 3, "moveto", RB_PAIR(c))
#define LINE_TO(ar, c) PUSH_TR(ar, 3, "lineto", RB_PAIR(c))
#define CURVE_TO(ar, c) PUSH_TR(ar, 7, "curveto", RB_PAIR(c[0]), RB_PAIR(c[1]), RB_PAIR(c[2]))
#define SVG_PAIR(p) p.x, p.y
#define SVG_MOVE_TO(f, c) fprintf(f, "M %f %f ", SVG_PAIR(c))
#define SVG_LINE_TO(f, c) fprintf(f, "L %f %f ", SVG_PAIR(c))
#define SVG_CURVE_TO(f, c) fprintf(f, "C %f %f %f %f %f %f", SVG_PAIR(c[0]), SVG_PAIR(c[1]), SVG_PAIR(c[2]))

static VALUE rb_mPotracer;
static VALUE rb_mTurnpolicy;
static VALUE rb_cPotracerTrace;
static VALUE rb_cPotracerParams;
static VALUE rb_cPotracerBitmap;
