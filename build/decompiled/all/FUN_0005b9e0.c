
undefined4 __regparm3 FUN_0005b9e0(int param_1,int param_2)

{
  int iVar1;
  
  if (param_2 < 1) {
    param_2 = 1;
  }
  iVar1 = (param_2 * 0x1d216) / 100;
  if (0xffff < iVar1) {
    iVar1 = 0xffff;
  }
  *(int *)(param_1 + 0xe) = (iVar1 * 100 + 99) / 0x1d216;
  FUN_0005b820();
  return 0;
}

