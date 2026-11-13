
void __regparm3 FUN_000215a0(undefined4 *param_1)

{
  int extraout_EDX;
  undefined8 uVar1;
  
  FUN_00021580();
  if (extraout_EDX != 0) {
    uVar1 = thunk_FUN_0005c320();
    param_1[1] = (int)uVar1;
    *param_1 = (int)((ulonglong)uVar1 >> 0x20);
  }
  return;
}

