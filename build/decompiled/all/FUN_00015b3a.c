
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __regparm2
FUN_00015b3a(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4,byte param_5)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int extraout_ECX;
  int iVar4;
  int unaff_EBX;
  int iVar5;
  
  iVar5 = 0;
  FUN_00012728();
  for (iVar2 = 0; (iVar2 < 0x200 && (*(int *)((int)&DAT_000c3d74 + iVar2) != 0)); iVar2 = iVar2 + 4)
  {
    iVar5 = iVar5 + 1;
  }
  if (iVar5 == 0x80) {
    _DAT_000c3fb0 = PTR_s_Out_of_sample_handles_000a7cdc;
    iVar5 = -1;
  }
  else {
    iVar2 = param_2 + 0x10;
    uVar3 = ll_alloc_try();
    (&DAT_000c3d74)[iVar5] = uVar3;
    FUN_00015b04();
    if ((param_5 & 0x10) == 0) {
      do {
        *(undefined1 *)((&DAT_000c3d74)[iVar5] + param_2) = 0;
        param_2 = param_2 + 1;
      } while (param_2 != iVar2);
    }
    else if ((param_5 & 0x20) == 0) {
      iVar2 = unaff_EBX + 0x10;
      iVar4 = extraout_ECX;
      do {
        puVar1 = (undefined1 *)((&DAT_000c3d74)[iVar5] + unaff_EBX);
        unaff_EBX = unaff_EBX + 1;
        *(undefined1 *)(iVar4 + (&DAT_000c3d74)[iVar5]) = *puVar1;
        iVar4 = iVar4 + 1;
      } while (unaff_EBX != iVar2);
    }
    else {
      iVar2 = extraout_ECX;
      iVar4 = extraout_ECX;
      do {
        puVar1 = (undefined1 *)((&DAT_000c3d74)[iVar5] + -1 + iVar2);
        iVar2 = iVar2 + -1;
        *(undefined1 *)(iVar4 + (&DAT_000c3d74)[iVar5]) = *puVar1;
        iVar4 = iVar4 + 1;
      } while (iVar2 != extraout_ECX + -0x10);
    }
  }
  return iVar5;
}

