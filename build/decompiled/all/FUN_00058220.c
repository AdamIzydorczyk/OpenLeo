
undefined8 __regparm3 FUN_00058220(int param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  
  if (*(int *)(param_1 + 0x14) == 0) {
    uVar1 = FUN_0001df34(param_2,param_3);
    uVar1 = uVar1 & 0x40;
  }
  else {
    uVar1 = FUN_0001df34(param_2,param_3);
    uVar1 = uVar1 & 0x10;
  }
  return CONCAT44(param_2,(uint)(uVar1 == 0));
}

