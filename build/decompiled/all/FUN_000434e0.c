
void __regparm2 FUN_000434e0(int param_1,int param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int extraout_ECX;
  undefined1 *puVar8;
  char *pcVar9;
  int unaff_EBX;
  undefined1 *puVar10;
  int iVar11;
  char *pcVar12;
  char *local_74;
  char *local_70;
  char *local_6c;
  int local_40;
  char *local_38;
  undefined1 *local_24;
  undefined1 *local_20;
  undefined1 *local_1c;
  undefined1 *local_18;
  
  iVar4 = FUN_0004a170();
  local_74 = (char *)thunk_FUN_0005c320();
  iVar11 = 0;
  iVar5 = *(short *)(param_1 + 0x19) * 0x50 + (int)*(short *)(param_1 + 0x15);
  local_40 = 0;
  sVar3 = *(short *)(param_1 + 0x17);
  FUN_000433e0();
  if ((-1 < unaff_EBX) && (extraout_ECX + unaff_EBX < 200)) {
    if (param_2 < 0) {
      local_40 = -param_2;
    }
    if (0x13f < param_2 + iVar5) {
      iVar11 = param_2 + iVar5 + -0x140;
    }
    iVar6 = iVar5 / 2;
    iVar1 = unaff_EBX + sVar3;
    local_70 = local_74 + iVar5;
    if (param_4 == 0) {
      if (unaff_EBX < iVar1) {
        local_38 = local_74;
        do {
          puVar8 = (undefined1 *)(param_2 + unaff_EBX * 400 + iVar4);
          iVar7 = 0;
          local_1c = puVar8 + iVar5;
          puVar10 = puVar8 + local_40;
          local_20 = local_1c + -iVar11;
          if (0 < iVar6) {
            pcVar9 = local_70;
            pcVar12 = local_38;
            do {
              uVar2 = *puVar10;
              if (*pcVar12 != '\0') {
                *puVar8 = *local_20;
              }
              if (pcVar9[-1] != '\0') {
                *local_1c = uVar2;
              }
              pcVar12 = pcVar12 + 1;
              puVar10 = puVar10 + 1;
              pcVar9 = pcVar9 + -1;
              puVar8 = puVar8 + 1;
              iVar7 = iVar7 + 1;
              local_20 = local_20 + -1;
              local_1c = local_1c + -1;
            } while (iVar7 < iVar6);
          }
          unaff_EBX = unaff_EBX + 1;
          local_70 = local_70 + iVar5;
          local_38 = local_38 + iVar5;
        } while (unaff_EBX < iVar1);
      }
    }
    else if (unaff_EBX < iVar1) {
      local_6c = local_70;
      do {
        local_24 = (undefined1 *)(param_2 + unaff_EBX * 400 + iVar4);
        puVar8 = local_24 + iVar5;
        iVar7 = 0;
        local_18 = puVar8 + -iVar11;
        puVar10 = local_24 + local_40;
        if (0 < iVar6) {
          pcVar9 = local_6c;
          pcVar12 = local_74;
          do {
            uVar2 = *puVar10;
            if (pcVar9[-1] != '\0') {
              *local_24 = *local_18;
            }
            if (*pcVar12 != '\0') {
              *puVar8 = uVar2;
            }
            pcVar9 = pcVar9 + -1;
            puVar10 = puVar10 + 1;
            pcVar12 = pcVar12 + 1;
            puVar8 = puVar8 + -1;
            iVar7 = iVar7 + 1;
            local_18 = local_18 + -1;
            local_24 = local_24 + 1;
          } while (iVar7 < iVar6);
        }
        unaff_EBX = unaff_EBX + 1;
        local_6c = local_6c + iVar5;
        local_74 = local_74 + iVar5;
      } while (unaff_EBX < iVar1);
    }
    thunk_FUN_0005c370();
  }
  return;
}

