
undefined4 * __regparm3 FUN_0005e280(undefined4 *param_1,undefined4 param_2)

{
  param_1[0x2b] = &PTR_FUN_000a2c70;
  *param_1 = param_2;
  FUN_0005e3a0();
  FUN_0005dae0();
  FUN_00058140();
  if ((DAT_000cf443 == 0) || (DAT_000cf4c4 == 0)) {
    DAT_000cf443 = 0;
  }
  else {
    DAT_000cf443 = 1;
  }
  FUN_00058250(DAT_000cf4cc,DAT_000cf4d8,DAT_000cf4dc);
  return param_1;
}

