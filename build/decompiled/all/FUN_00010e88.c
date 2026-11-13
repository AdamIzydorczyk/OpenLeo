
undefined4 __regparm3 FUN_00010e88(int param_1,int param_2)

{
  undefined8 uVar1;
  
  if ((param_1 != 0) && (param_2 != 0)) {
    uVar1 = FUN_00010884();
    if ((int)uVar1 + param_1 == (int)((ulonglong)uVar1 >> 0x20)) {
      return 1;
    }
  }
  return 0;
}

