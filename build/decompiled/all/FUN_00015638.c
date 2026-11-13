
void __regparm3 FUN_00015638(undefined4 param_1,undefined4 param_2)

{
  DAT_000cf2cc = DAT_000cf2cc + 1;
  *(undefined4 *)(&DAT_000cf2d0 + DAT_000cf2cc * 4) = DAT_000afb84;
  FUN_0005c370(param_2);
  DAT_000afb84 = 0;
  return;
}

