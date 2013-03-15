#include "mruby.h"
#include "mruby/irep.h"

#define CAT2(x,y) CAT2_(x,y)
#define CAT3(x,y,z)  x ## y ## z
#define GEM_INIT(n)  CAT3(mrb_, n, _gem_init)
#define GEM_FINAL(n) CAT3(mrb_, n, _gem_final)
#define GEM_IREP(n)  CAT3(mrb_, n, _irep)

#define XGEM(n) \
  extern void GEM_INIT(n)  (mrb_state *mrb); \
  extern void GEM_FINAL(n) (mrb_state *mrb);
#define XIREP(n) \
  extern const char GEM_IREP(n) [];
#include "mrbgems_registration_list.h"
#undef XGEM
#undef XIREP

static void init_irep(mrb_state* mrb, const char irep[])
{
  int ai = mrb_gc_arena_save(mrb);
  mrb_load_irep(mrb, irep);
  if (mrb->exc) {
    mrb_p(mrb, mrb_obj_value(mrb->exc));
    exit(0);
  }
  mrb_gc_arena_restore(mrb, ai);
}

void mrb_init_mrbgems(mrb_state *mrb)
{
#define XGEM(n) GEM_INIT(n) (mrb);
#define XIREP(n) init_irep(mrb, GEM_IREP(n));
#include "mrbgems_registration_list.h"
#undef XGEM
#undef XIREP
}

void mrb_final_mrbgems(mrb_state *mrb)
{
#define XGEM(n) GEM_FINAL(n) (mrb);
#define XIREP(n)
#include "mrbgems_registration_list.h"
#undef XGEM
#undef XIREP
}
