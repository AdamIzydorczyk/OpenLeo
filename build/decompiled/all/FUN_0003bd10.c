
undefined8 __regparm3 FUN_0003bd10(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  bool bVar2;
  undefined8 uVar3;
  
  uVar3 = FUN_0003bcf0();
  iVar1 = (int)((ulonglong)uVar3 >> 0x20);
  if ((int)uVar3 == 0) {
    bVar2 = *(int *)(iVar1 + 0xee) == 0;
  }
  else {
    bVar2 = *(int *)(iVar1 + 0xee) == 4;
  }
  return CONCAT44(param_2,(uint)bVar2);
}

