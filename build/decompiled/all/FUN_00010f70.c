
/* WARNING: Removing unreachable block (ram,0x00010fd4) */

undefined8 __regparm3 FUN_00010f70(int param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  undefined8 uVar3;
  uint local_1c;
  
  iVar1 = 0;
  if (param_1 != 0) {
    local_1c = param_1 + 0x1fU & 0xffffffe0;
    iVar1 = DAT_000adea4;
    while (iVar1 != 0) {
      FUN_00010884();
      uVar3 = FUN_00010884();
      if (local_1c <= (uint)uVar3) {
        iVar2 = (int)((ulonglong)uVar3 >> 0x20);
        if (7 < (uint)uVar3 - local_1c) {
          FUN_000108c0();
          FUN_000108c0();
          iVar2 = local_1c + iVar1;
        }
        DAT_000adea4 = iVar2;
        FUN_000108c0();
        goto LAB_00010ff8;
      }
      iVar1 = extraout_ECX;
    }
    iVar1 = 0;
  }
LAB_00010ff8:
  return CONCAT44(param_2,iVar1);
}

