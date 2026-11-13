
void __regparm3 FUN_00076bf0(undefined4 param_1,undefined4 param_2)

{
  undefined8 uVar1;
  
  uVar1 = FUN_00076bc6(param_2);
  if ((undefined4 *)uVar1 != (undefined4 *)0x0) {
    *(undefined4 *)uVar1 = (int)((ulonglong)uVar1 >> 0x20);
  }
  return;
}

