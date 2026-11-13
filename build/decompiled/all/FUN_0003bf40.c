
undefined8 __regparm3 FUN_0003bf40(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  bool bVar2;
  undefined8 uVar3;
  
  uVar3 = FUN_0003bcf0(param_2,param_3);
  iVar1 = (int)((ulonglong)uVar3 >> 0x20);
  if ((int)uVar3 == 0) {
    bVar2 = *(int *)(iVar1 + 0xf6) == 0;
  }
  else {
    bVar2 = *(int *)(iVar1 + 0x10e + *(int *)(iVar1 + 0xf2) * 0xb8) + -1 == *(int *)(iVar1 + 0xf6);
  }
  return CONCAT44(param_2,(uint)bVar2);
}

