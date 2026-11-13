
uint __regparm3 FUN_00081801(undefined4 param_1,char *param_2)

{
  code *pcVar1;
  bool bVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  uint extraout_EDX;
  uint extraout_EDX_00;
  uint unaff_EBX;
  char *pcVar6;
  char **ppcVar7;
  char **ppcVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  uint uVar12;
  byte bVar13;
  undefined1 auStack_a8 [128];
  char *local_24 [2];
  uint local_1c;
  uint local_18;
  undefined1 *local_14;
  uint local_10;
  char *local_c;
  uint local_8;
  
  local_24[0] = param_2;
  local_18 = FUN_0007f975();
  ppcVar8 = (char **)(auStack_a8 + 0x80);
  if ((local_18 == 0) || ((local_18 & 2) == 0)) {
    FUN_00078c12();
    return 0xffffffff;
  }
  if ((local_18 & 0x80) == 0) {
LAB_0008186c:
    bVar13 = 0;
    if ((local_18 & 0x40) == 0) {
      ppcVar8[-1] = (char *)0x818ac;
      uVar5 = FUN_0007bfac();
      if (uVar5 < 0xb0) {
        ppcVar8[-1] = (char *)0x818ba;
        FUN_00082581();
        uVar5 = extraout_EDX_00;
      }
      uVar12 = 0x200;
      if (uVar5 < 0x230) {
        uVar12 = 0x80;
      }
      uVar5 = 0;
      local_14 = (undefined1 *)((int)ppcVar8 + -uVar12);
      local_8 = 0;
      local_1c = 0;
      local_c = local_24[0];
      puVar9 = (undefined4 *)((int)ppcVar8 + -uVar12);
      while (local_8 < unaff_EBX) {
        puVar11 = puVar9;
        if (*local_c == '\n') {
          local_14[uVar5] = 0xd;
          uVar5 = uVar5 + 1;
          bVar13 = uVar5 < uVar12;
          if (uVar5 == uVar12) {
            pcVar1 = (code *)swi(0x21);
            iVar4 = (*pcVar1)();
            ppcVar7 = (char **)(puVar9 + 1);
            puVar11 = puVar9 + 1;
            bVar2 = (bVar13 & 1) != 0;
            local_10 = (iVar4 << 1 | (uint)bVar13) >> 1 | (uint)bVar2 << 0x1f;
            if (bVar2) goto LAB_0008185c;
            puVar10 = puVar9;
            if (local_10 != uVar12) goto LAB_0008192c;
            uVar5 = local_10 ^ uVar12;
            local_1c = local_8;
          }
        }
        pcVar6 = local_c + 1;
        local_8 = local_8 + 1;
        local_14[uVar5] = *local_c;
        uVar5 = uVar5 + 1;
        bVar13 = uVar5 < uVar12;
        puVar9 = puVar11;
        local_c = pcVar6;
        if (uVar5 == uVar12) {
          pcVar1 = (code *)swi(0x21);
          iVar4 = (*pcVar1)();
          ppcVar7 = (char **)((int)puVar11 + 4);
          puVar9 = (undefined4 *)((int)puVar11 + 4);
          bVar2 = (bVar13 & 1) != 0;
          local_10 = (iVar4 << 1 | (uint)bVar13) >> 1 | (uint)bVar2 << 0x1f;
          if (bVar2) goto LAB_0008185c;
          puVar10 = puVar11;
          if (local_10 != uVar12) {
LAB_0008192c:
            uVar5 = local_10;
            *puVar10 = 0x81936;
            FUN_00078c12();
            return local_1c + uVar5;
          }
          uVar5 = local_10 ^ uVar12;
          local_1c = local_8;
        }
      }
      bVar13 = 0;
      if (uVar5 == 0) goto LAB_000819cc;
      pcVar1 = (code *)swi(0x21);
      iVar4 = (*pcVar1)();
      ppcVar7 = (char **)(puVar9 + 1);
      bVar2 = (bVar13 & 1) != 0;
      uVar12 = (iVar4 << 1 | (uint)bVar13) >> 1 | (uint)bVar2 << 0x1f;
      local_10 = uVar12;
      if (!bVar2) {
        if (uVar12 != uVar5) {
          *puVar9 = 0x819c2;
          FUN_00078c12();
          return local_1c + uVar12;
        }
        goto LAB_000819cc;
      }
    }
    else {
      pcVar1 = (code *)swi(0x21);
      iVar4 = (*pcVar1)();
      ppcVar7 = ppcVar8 + 1;
      bVar2 = (bVar13 & 1) != 0;
      uVar5 = (iVar4 << 1 | (uint)bVar13) >> 1 | (uint)bVar2 << 0x1f;
      local_10 = uVar5;
      if (!bVar2) {
        if (uVar5 != unaff_EBX) {
          *ppcVar8 = (char *)0x8189f;
          FUN_00078c12();
          unaff_EBX = uVar5;
        }
        goto LAB_000819cc;
      }
    }
  }
  else {
    bVar13 = 0;
    pcVar1 = (code *)swi(0x21);
    uVar3 = (*pcVar1)();
    ppcVar7 = local_24;
    ppcVar8 = local_24;
    bVar2 = (bVar13 & 1) != 0;
    local_10 = CONCAT22((ushort)((ushort)((extraout_EDX & 0xffff) << 1) | (ushort)bVar13) >> 1 |
                        (ushort)bVar2 << 0xf,uVar3);
    if (!bVar2) goto LAB_0008186c;
  }
LAB_0008185c:
  *(undefined4 *)((int)ppcVar7 + -4) = 0x81867;
  unaff_EBX = FUN_0007ba5f();
LAB_000819cc:
  return unaff_EBX;
}

