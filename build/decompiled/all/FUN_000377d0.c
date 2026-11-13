
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __regparm3 FUN_000377d0(int param_1)

{
  int iVar1;
  uint uVar2;
  int unaff_EBX;
  
  if (param_1 == -1) {
    return 1;
  }
  if ((-1 < param_1) && (param_1 < 3)) {
    uVar2 = DAT_000a9088 - _DAT_000a9084 >> 0x1f;
    iVar1 = (int)(((DAT_000a9088 - _DAT_000a9084 ^ uVar2) - uVar2) + 0xc) / 2;
    if ((*(int *)(&DAT_000a9084 + param_1 * 4) - iVar1 <= unaff_EBX) &&
       (unaff_EBX <= iVar1 + *(int *)(&DAT_000a9084 + param_1 * 4))) {
      return 1;
    }
  }
  return 0;
}

