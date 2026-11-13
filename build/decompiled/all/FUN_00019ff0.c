
void __regparm3 FUN_00019ff0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  undefined4 extraout_EDX;
  
  bVar1 = in(DAT_000c40cc);
  out(DAT_000c40cc,bVar1 | (byte)DAT_000c40d8);
  FUN_00075c8c(param_2,param_3);
  DAT_000c40a8 = extraout_EDX;
  return;
}

