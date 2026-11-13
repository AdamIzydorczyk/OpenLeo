
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __regparm3 FUN_000165d8(undefined4 param_1,ushort param_2)

{
  int iVar1;
  int *extraout_ECX;
  int iVar2;
  
  DAT_000c3f78 = 0x100;
  _DAT_000c3f7c = (int)((uint)param_2 * 2 + 0xf) >> 4;
  FUN_00075c3d();
  if (DAT_000c3f90 != 0) {
    return 0;
  }
  *(undefined2 *)(extraout_ECX + 1) = DAT_000c3f84;
  iVar1 = (DAT_000c3f78 & 0xffff) * 0x10;
  iVar2 = (uint)param_2 + iVar1;
  if ((DAT_000c3f78 & 0xffff) >> 0xc != iVar2 - 1U >> 0x10) {
    iVar1 = iVar2;
  }
  *extraout_ECX = iVar1;
  return 1;
}

