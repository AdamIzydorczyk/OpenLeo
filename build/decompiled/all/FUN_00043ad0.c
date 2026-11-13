
void __regparm2 FUN_00043ad0(int param_1,int param_2,undefined4 param_3,int param_4)

{
  short sVar1;
  int iVar2;
  char *pcVar3;
  int extraout_ECX;
  int iVar4;
  int iVar5;
  int iVar6;
  int unaff_EBX;
  undefined1 *puVar7;
  int iVar8;
  int iVar9;
  bool bVar10;
  bool bVar11;
  int local_14;
  
  local_14 = FUN_0004a170();
  iVar2 = thunk_FUN_0005c320();
  sVar1 = *(short *)(param_1 + 0x17);
  iVar9 = 0;
  iVar8 = 0;
  FUN_000433e0();
  if ((-1 < unaff_EBX) && (sVar1 + unaff_EBX < 200)) {
    if (param_2 < 0) {
      iVar9 = -param_2;
    }
    if (0x13f < param_2 + extraout_ECX) {
      iVar8 = param_2 + extraout_ECX + -0x140;
    }
    iVar9 = iVar9 + 1;
    iVar4 = sVar1 + unaff_EBX;
    iVar8 = extraout_ECX - iVar8;
    if (param_4 == 0) {
      local_14 = unaff_EBX * 400 + param_2 + local_14;
      for (; unaff_EBX < iVar4; unaff_EBX = unaff_EBX + 1) {
        pcVar3 = (char *)(iVar2 + iVar9);
        puVar7 = (undefined1 *)(local_14 + iVar9);
        iVar6 = iVar9 - iVar8;
        iVar5 = iVar9;
        while (SBORROW4(iVar5,iVar8) != iVar6 < 0) {
          if (((*pcVar3 != '\0') && (pcVar3[-1] == '\0')) ||
             ((*pcVar3 == '\0' && (pcVar3[-1] != '\0')))) {
            *puVar7 = 1;
          }
          pcVar3 = pcVar3 + 1;
          iVar5 = iVar5 + 1;
          puVar7 = puVar7 + 1;
          iVar6 = iVar5 - iVar8;
        }
        local_14 = local_14 + 400;
        iVar2 = iVar2 + extraout_ECX;
      }
    }
    else {
      local_14 = unaff_EBX * 400 + extraout_ECX + param_2 + local_14;
      if (unaff_EBX < iVar4) {
        pcVar3 = (char *)(iVar9 + iVar2);
        iVar5 = local_14 - iVar9;
        bVar11 = SBORROW4(iVar9,iVar8);
        bVar10 = iVar9 - iVar8 < 0;
        iVar6 = iVar9;
        while( true ) {
          while (bVar11 != bVar10) {
            if (((*pcVar3 != '\0') && (pcVar3[-1] == '\0')) ||
               ((*pcVar3 == '\0' && (pcVar3[-1] != '\0')))) {
              *(undefined1 *)(iVar5 + 1) = 1;
            }
            pcVar3 = pcVar3 + 1;
            iVar6 = iVar6 + 1;
            iVar5 = iVar5 + -1;
            bVar10 = iVar6 - iVar8 < 0;
            bVar11 = SBORROW4(iVar6,iVar8);
          }
          local_14 = local_14 + 400;
          unaff_EBX = unaff_EBX + 1;
          iVar2 = iVar2 + extraout_ECX;
          if (iVar4 <= unaff_EBX) break;
          pcVar3 = (char *)(iVar9 + iVar2);
          iVar5 = local_14 - iVar9;
          bVar11 = SBORROW4(iVar9,iVar8);
          bVar10 = iVar9 - iVar8 < 0;
          iVar6 = iVar9;
        }
      }
    }
    thunk_FUN_0005c370();
  }
  return;
}

