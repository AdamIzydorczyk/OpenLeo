
void __regparm2 FUN_000437a0(int param_1,int param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined1 *puVar7;
  int extraout_ECX;
  int iVar8;
  undefined1 *puVar9;
  int unaff_EBX;
  int iVar10;
  char *pcVar11;
  int iVar12;
  char *pcVar13;
  int local_44;
  int local_40;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  
  iVar4 = FUN_0004a170();
  local_34 = thunk_FUN_0005c320();
  local_40 = 0;
  iVar5 = *(short *)(param_1 + 0x19) * 0x50 + (int)*(short *)(param_1 + 0x15);
  local_44 = 0;
  sVar3 = *(short *)(param_1 + 0x17);
  FUN_000433e0();
  if ((-1 < unaff_EBX) && (extraout_ECX + unaff_EBX < 200)) {
    if (param_2 < 0) {
      local_40 = -param_2;
    }
    if (0x13f < param_2 + iVar5) {
      local_44 = param_2 + iVar5 + -0x140;
    }
    iVar1 = unaff_EBX + sVar3;
    local_38 = iVar5 + local_34;
    if (param_4 == 0) {
      local_24 = param_4;
      if (unaff_EBX < iVar1) {
        local_2c = local_34;
        do {
          iVar8 = param_2 + unaff_EBX * 400 + iVar4;
          iVar10 = (uint)*(byte *)(*(int *)(param_1 + 0x27) + local_24) + local_40;
          iVar12 = (uint)*(byte *)(*(int *)(param_1 + 0x2f) + local_24) + local_44;
          iVar6 = ((iVar5 - iVar10) - iVar12) / 2;
          iVar12 = -iVar12;
          pcVar13 = (char *)(iVar12 + local_38);
          puVar9 = (undefined1 *)(iVar8 + iVar10);
          pcVar11 = (char *)(iVar10 + local_2c);
          iVar10 = 0;
          puVar7 = (undefined1 *)(iVar12 + iVar8 + iVar5);
          if (0 < iVar6) {
            do {
              uVar2 = *puVar9;
              if (*pcVar13 != '\0') {
                *puVar9 = *puVar7;
              }
              if (*pcVar11 != '\0') {
                *puVar7 = uVar2;
              }
              pcVar13 = pcVar13 + -1;
              puVar9 = puVar9 + 1;
              pcVar11 = pcVar11 + 1;
              iVar10 = iVar10 + 1;
              puVar7 = puVar7 + -1;
            } while (iVar10 < iVar6);
          }
          unaff_EBX = unaff_EBX + 1;
          local_24 = local_24 + 1;
          local_38 = local_38 + iVar5;
          local_2c = local_2c + iVar5;
        } while (unaff_EBX < iVar1);
      }
    }
    else {
      local_28 = 0;
      if (unaff_EBX < iVar1) {
        local_30 = local_38;
        do {
          iVar10 = param_2 + unaff_EBX * 400 + iVar4;
          iVar12 = (uint)*(byte *)(*(int *)(param_1 + 0x2f) + local_28) + local_40;
          iVar8 = (uint)*(byte *)(*(int *)(param_1 + 0x27) + local_28) + local_44;
          iVar6 = ((iVar5 - iVar12) - iVar8) / 2;
          iVar8 = -iVar8;
          pcVar11 = (char *)(iVar8 + local_30);
          puVar7 = (undefined1 *)(iVar10 + iVar12);
          pcVar13 = (char *)(iVar12 + local_34);
          iVar12 = 0;
          puVar9 = (undefined1 *)(iVar8 + iVar10 + iVar5);
          if (0 < iVar6) {
            do {
              uVar2 = *puVar7;
              if (*pcVar11 != '\0') {
                *puVar7 = *puVar9;
              }
              if (*pcVar13 != '\0') {
                *puVar9 = uVar2;
              }
              pcVar11 = pcVar11 + -1;
              puVar7 = puVar7 + 1;
              pcVar13 = pcVar13 + 1;
              iVar12 = iVar12 + 1;
              puVar9 = puVar9 + -1;
            } while (iVar12 < iVar6);
          }
          unaff_EBX = unaff_EBX + 1;
          local_28 = local_28 + 1;
          local_30 = local_30 + iVar5;
          local_34 = local_34 + iVar5;
        } while (unaff_EBX < iVar1);
      }
    }
    thunk_FUN_0005c370();
  }
  return;
}

