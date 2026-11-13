
void __regparm3 FUN_00010318(int param_1,undefined4 param_2)

{
  undefined4 extraout_EDX;
  
  if (param_1 != 0) {
    FUN_000129a4(param_2);
    DAT_000cf2cc = DAT_000cf2cc + 1;
    *(undefined4 *)(&DAT_000cf2d0 + DAT_000cf2cc * 4) = extraout_EDX;
    FUN_0005c370();
  }
  return;
}

