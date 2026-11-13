
void __regparm3 FUN_00019ec0(undefined4 param_1,undefined4 param_2)

{
  int extraout_EDX;
  
  if (DAT_000c4064 != 0) {
    FUN_00018d10();
    return;
  }
  if (DAT_000c40a8 != 0) {
    FUN_00019ff0(param_2);
    FUN_0001a5c0();
    DAT_000c40a8 = extraout_EDX;
  }
  return;
}

