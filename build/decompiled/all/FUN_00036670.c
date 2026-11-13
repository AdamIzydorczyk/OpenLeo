
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_00036670(int param_1,undefined4 param_2)

{
  longdouble lVar1;
  
  if (*(int *)(param_1 + 0x5c) == 0x22) {
    _DAT_000ced10 = *(float *)(param_1 + 0x136) + _DAT_000ced10;
  }
  if (*(int *)(param_1 + 0x5c) == 0x23) {
    _DAT_000ced10 = *(float *)(param_1 + 0x12e) + _DAT_000ced10;
  }
  lVar1 = (longdouble)FUN_00075d96();
  _DAT_000ced10 = _DAT_000ced10 - (float)(int)ROUND(lVar1);
  return CONCAT44(param_2,(int)ROUND(lVar1));
}

