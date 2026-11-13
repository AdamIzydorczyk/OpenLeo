
uint __regparm3 FUN_000819d3(undefined4 param_1,char *param_2)

{
  code *pcVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  char *pcVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  byte bVar10;
  undefined4 local_24;
  undefined1 local_20 [4];
  undefined4 local_1c;
  uint local_14;
  
  local_1c = FUN_0007f975();
  if ((local_1c == 0) || ((local_1c & 1) == 0)) {
    FUN_00078c12();
    uVar3 = 0xffffffff;
  }
  else {
    bVar10 = 0;
    if ((local_1c & 0x40) == 0) {
      uVar3 = 0;
      puVar9 = &local_24;
      while( true ) {
        bVar10 = 0;
        pcVar1 = (code *)swi(0x21);
        iVar4 = (*pcVar1)();
        puVar8 = puVar9 + 1;
        bVar2 = (bVar10 & 1) != 0;
        uVar5 = (iVar4 << 1 | (uint)bVar10) >> 1 | (uint)bVar2 << 0x1f;
        puVar9[2] = uVar5;
        if (bVar2) goto LAB_00081a2b;
        if (uVar5 == 0) break;
        puVar9[5] = param_2 + uVar5;
        iVar4 = 0;
        for (pcVar6 = param_2; pcVar6 < (char *)puVar9[5]; pcVar6 = pcVar6 + 1) {
          if (*pcVar6 == '\x1a') {
            *puVar9 = 0x81a8f;
            FUN_0007a5bd();
            goto LAB_00081ac6;
          }
          iVar7 = iVar4;
          if (*pcVar6 != '\r') {
            uVar3 = uVar3 + 1;
            iVar7 = iVar4 + 1;
            param_2[iVar4] = *pcVar6;
          }
          iVar4 = iVar7;
        }
        iVar7 = puVar9[4];
        param_2 = param_2 + iVar4;
        puVar9[4] = iVar7 - iVar4;
        if (((*(byte *)((int)puVar9 + 0xd) & 0x20) != 0) ||
           (puVar9 = puVar9 + 1, iVar7 - iVar4 == 0)) break;
      }
    }
    else {
      pcVar1 = (code *)swi(0x21);
      iVar4 = (*pcVar1)();
      puVar8 = (undefined4 *)local_20;
      bVar2 = (bVar10 & 1) != 0;
      uVar3 = (iVar4 << 1 | (uint)bVar10) >> 1 | (uint)bVar2 << 0x1f;
      if (bVar2) {
LAB_00081a2b:
        *(undefined4 *)((int)puVar8 + -4) = 0x81a30;
        uVar3 = FUN_0007ba5f();
        return uVar3;
      }
    }
  }
LAB_00081ac6:
  return uVar3;
}

