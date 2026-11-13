
/* WARNING: Removing unreachable block (ram,0x00076625) */

undefined4 __regparm3 FUN_000765ab(undefined4 *param_1,int param_2)

{
  int iVar1;
  undefined4 *extraout_ECX;
  uint unaff_EBX;
  
  if ((*(byte *)(param_1 + 3) & 6) == 0) {
    if (unaff_EBX == 0) {
      FUN_0007a635();
      iVar1 = FUN_00076569();
      if ((iVar1 != 0) && (iVar1 = FUN_0007a5bd(), iVar1 == -1)) {
        return 0xffffffff;
      }
    }
    else if (unaff_EBX < 2) {
      iVar1 = FUN_00076569();
      if ((iVar1 != 0) && (iVar1 = FUN_0007a5bd(), iVar1 == -1)) {
        return 0xffffffff;
      }
    }
    else {
      if (unaff_EBX != 2) goto LAB_000765d1;
      *param_1 = param_1[2];
      *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) & 0xef;
      param_1[1] = 0;
      iVar1 = FUN_0007a5bd();
      if (iVar1 == -1) {
        return 0xffffffff;
      }
    }
  }
  else {
    if ((*(byte *)((int)param_1 + 0xd) & 0x10) == 0) {
      param_1[1] = 0;
      *param_1 = param_1[2];
    }
    else {
      iVar1 = FUN_0007a4e4();
      param_1 = extraout_ECX;
      if (iVar1 != 0) {
        if (unaff_EBX != 0) {
          return 0xffffffff;
        }
        if (-1 < param_2) {
          return 0xffffffff;
        }
LAB_000765d1:
        FUN_00078c12();
        return 0xffffffff;
      }
    }
    *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) & 0xeb;
    iVar1 = FUN_0007a5bd();
    if (iVar1 == -1) {
      return 0xffffffff;
    }
  }
  return 0;
}

