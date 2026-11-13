
undefined8 __regparm3 FUN_00078a94(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  
  iVar1 = *(int *)(param_1 + 0xc);
  *(int *)(param_1 + 0xc) = iVar1 + -1;
  if (iVar1 != 0) {
    uVar3 = FUN_00077e89();
    uVar2 = (undefined4)uVar3;
    if ((*(byte *)((int)((ulonglong)uVar3 >> 0x20) + 0x10) & 2) == 0) goto LAB_00078ab7;
  }
  uVar2 = 0xffffffff;
LAB_00078ab7:
  return CONCAT44(param_2,uVar2);
}

