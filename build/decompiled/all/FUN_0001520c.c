
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_0001520c(byte *param_1)

{
  _DAT_000afb78 = param_1;
  DAT_000afb80 = param_1 + 1;
  DAT_000afb7c = param_1 + (*param_1 & 0x1f);
  return;
}

