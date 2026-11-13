
undefined8 __regparm3 FUN_0002f6e0(int param_1,undefined4 param_2)

{
  int *piVar1;
  uint uVar2;
  
  piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
  uVar2 = (int)(*piVar1 - 0x1aeU) >> 0x1f;
  return CONCAT44(param_2,(uint)((int)((*piVar1 - 0x1aeU ^ uVar2) - uVar2) < 6));
}

