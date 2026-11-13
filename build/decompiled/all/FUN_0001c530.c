
undefined4 __regparm3 FUN_0001c530(int param_1,uint param_2)

{
  if ((param_2 & 0x200) == 0) {
    param_2 = (&DAT_000ce284)[param_2];
  }
  return *(undefined4 *)(param_1 + 10 + (param_2 & 0x7f) * 4);
}

