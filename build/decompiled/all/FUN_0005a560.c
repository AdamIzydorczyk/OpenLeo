
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_0005a560(undefined4 param_1,undefined4 param_2)

{
  if ((DAT_000cf104 & 1) == 0) {
    DAT_000cf104 = DAT_000cf104 | 1;
    _DAT_000cf100 = &PTR_FUN_000961fc;
    DAT_000cf0fc = 0;
    thunk_FUN_00076a53();
  }
  FUN_00057a70();
  FUN_00057990();
  FUN_00057a00();
  return CONCAT44(param_2,DAT_000cf0fc);
}

