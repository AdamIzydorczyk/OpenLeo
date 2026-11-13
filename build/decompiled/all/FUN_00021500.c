
void __regparm3 FUN_00021500(undefined4 *param_1)

{
  undefined4 uVar1;
  undefined4 extraout_ECX;
  int extraout_EDX;
  
  FUN_000214d0();
  if (extraout_EDX != 0) {
    thunk_FUN_0005c320();
    uVar1 = FUN_00076bb7();
    param_1[1] = uVar1;
    *param_1 = extraout_ECX;
  }
  return;
}

