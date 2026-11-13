
int __regparm2 FUN_00017cfd(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  uint unaff_EBX;
  
  iVar1 = FUN_00017c38(param_3);
  if (iVar1 == 1) {
    if (unaff_EBX >> 0x12 != (unaff_EBX + param_2) - 1 >> 0x12) {
      if (param_3 == 0) {
        FUN_00017c24();
      }
      iVar1 = FUN_00017c38(param_3);
      if (iVar1 != 1) {
        return iVar1;
      }
    }
    iVar1 = 1;
  }
  return iVar1;
}

