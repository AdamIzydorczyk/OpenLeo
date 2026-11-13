
undefined4 __regparm3 FUN_0001c550(int param_1,uint param_2)

{
  undefined4 uVar1;
  
  if ((param_2 & 0x200) == 0) {
    param_2 = (&DAT_000ce284)[param_2];
  }
  param_1 = (param_2 & 0x7f) * 4 + param_1;
  uVar1 = *(undefined4 *)(param_1 + 0x20a);
  *(undefined4 *)(param_1 + 0x20a) = 0;
  return uVar1;
}

