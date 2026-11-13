
int __regparm3 FUN_00058cd0(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_00058bf0();
  while (iVar2 = FUN_00058a70(iVar1,param_3), *(int *)(iVar2 + 0xe) != 0) {
    if (iVar1 == 0) {
      iVar1 = *(int *)(param_1 + 0x10);
    }
    iVar1 = iVar1 + -1;
  }
  return iVar1;
}

