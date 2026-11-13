
void __regparm3 FUN_0003be60(undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  undefined8 uVar3;
  
  uVar3 = FUN_0003bcf0(param_2);
  iVar2 = (int)((ulonglong)uVar3 >> 0x20);
  if ((int)uVar3 == 0) {
    piVar1 = (int *)(iVar2 + 0xee);
    *piVar1 = *piVar1 + -1;
    return;
  }
  piVar1 = (int *)(iVar2 + 0xee);
  *piVar1 = *piVar1 + 1;
  return;
}

