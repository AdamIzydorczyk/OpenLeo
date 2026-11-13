
void __regparm3 FUN_000582d0(undefined4 param_1,undefined4 param_2)

{
  undefined8 uVar1;
  
  do {
    uVar1 = FUN_0001c4d0(param_2);
  } while ((int)uVar1 != 0);
  *(undefined4 *)((int)((ulonglong)uVar1 >> 0x20) + 1) = 0;
  FUN_0001c490();
  return;
}

