
undefined4 * __regparm3 FUN_000402c0(undefined4 *param_1,uint param_2)

{
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *param_1 = &PTR_LAB_000a0b2c;
  if ((param_2 & 2) != 0) {
    FUN_0005c370();
  }
  return param_1;
}

