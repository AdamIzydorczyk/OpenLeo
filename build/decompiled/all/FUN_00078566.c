
int __regparm3 FUN_00078566(int param_1,int *param_2)

{
  byte bVar1;
  int iVar2;
  undefined6 *puVar3;
  undefined4 *puVar4;
  bool bVar5;
  int iVar6;
  int iVar7;
  char *pcVar8;
  int *piVar9;
  undefined4 unaff_EBP;
  char *pcVar10;
  int iVar11;
  char local_84;
  char local_83 [79];
  int local_34;
  int local_30;
  undefined4 local_2c;
  int *local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined2 uStack_18;
  undefined2 uStack_16;
  
  uStack_18 = (undefined2)unaff_EBP;
  uStack_16 = (undefined2)((uint)unaff_EBP >> 0x10);
  pcVar8 = &local_84;
  iVar11 = 0;
  local_24 = 0;
  local_28 = param_2;
  while( true ) {
    iVar6 = FUN_00077e89();
    iVar7 = local_24;
    if (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 2) == 0) break;
    local_24 = local_24 + 1;
  }
  if ((*(byte *)(param_1 + 0x10) & 2) != 0) goto LAB_000787b7;
  iVar2 = *(int *)(param_1 + 0xc);
  *(int *)(param_1 + 0xc) = iVar2 + -1;
  pcVar8 = &local_84;
  if (iVar2 != 0) {
    if ((iVar6 == 0x2b) || (pcVar8 = &local_84, iVar6 == 0x2d)) {
      local_84 = (char)iVar6;
      iVar6 = FUN_00078a94();
      pcVar8 = local_83;
      local_24 = iVar7 + 1;
      if (iVar6 == -1) goto LAB_000787b7;
    }
    if (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 0x20) == 0) {
      if (iVar6 != 0x2e) goto LAB_000787ae;
    }
    local_1c._0_2_ = 0;
    local_1c._2_2_ = 0;
    bVar5 = false;
    if (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 0x20) != 0) {
      bVar5 = true;
      do {
        *pcVar8 = (char)iVar6;
        pcVar8 = pcVar8 + 1;
        if ((*(byte *)(param_1 + 0x10) & 0x10) != 0) {
          local_1c._2_2_ = (short)iVar6 + local_1c._2_2_ * 10 + -0x30;
        }
        iVar11 = iVar11 + 1;
        iVar6 = FUN_00078a94();
        if (iVar6 == -1) goto LAB_000787b7;
      } while (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 0x20) != 0);
    }
    pcVar10 = pcVar8;
    iVar7 = iVar11;
    if (iVar6 == 0x2e) {
      *pcVar8 = '.';
      iVar6 = FUN_00078a94();
      pcVar8 = pcVar8 + 1;
      if (iVar6 == -1) goto LAB_000787b7;
      if (!bVar5) {
        if (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 0x20) == 0) goto LAB_000787ae;
      }
      iVar11 = iVar11 + 1;
      do {
        if (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 0x20) == 0) break;
        iVar11 = iVar11 + 1;
        *pcVar8 = (char)iVar6;
        iVar6 = FUN_00078a94();
        pcVar8 = pcVar8 + 1;
      } while (iVar6 != -1);
      if ((*(byte *)(param_1 + 0x10) & 0x10) != 0) {
        local_2c = 0;
        pcVar10 = pcVar8;
        while( true ) {
          pcVar10 = pcVar10 + -1;
          if (*pcVar10 == '.') break;
          local_20 = 10;
          local_2c = (uint)CONCAT12(*pcVar10 + -0x30,(undefined2)local_2c);
          local_2c = local_2c / 10;
        }
        local_1c._0_2_ = (undefined2)local_2c;
      }
      pcVar10 = pcVar8;
      iVar7 = iVar11;
      if (iVar6 == -1) goto LAB_000787b7;
    }
    pcVar8 = pcVar10;
    iVar11 = iVar7;
    if (((*(byte *)(param_1 + 0x10) & 0x10) == 0) && ((iVar6 == 0x65 || (iVar6 == 0x45)))) {
      iVar11 = iVar7 + 1;
      *pcVar10 = (char)iVar6;
      iVar6 = FUN_00078a94();
      pcVar8 = pcVar10 + 1;
      if (iVar6 == -1) goto LAB_000787b7;
      if ((iVar6 == 0x2b) || (iVar6 == 0x2d)) {
        iVar11 = iVar7 + 2;
        *pcVar8 = (char)iVar6;
        iVar6 = FUN_00078a94();
        pcVar8 = pcVar10 + 2;
        if (iVar6 == -1) goto LAB_000787b7;
      }
      if (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 0x20) == 0) {
        iVar11 = 0;
      }
      else {
        do {
          iVar11 = iVar11 + 1;
          *pcVar8 = (char)iVar6;
          iVar6 = FUN_00078a94();
          pcVar8 = pcVar8 + 1;
          if (iVar6 == -1) goto LAB_000787b7;
        } while (((&DAT_000a68b4)[(byte)((char)iVar6 + 1)] & 0x20) != 0);
      }
    }
  }
LAB_000787ae:
  FUN_00077e90();
LAB_000787b7:
  if ((0 < iVar11) && (iVar11 = iVar11 + local_24, (*(byte *)(param_1 + 0x10) & 1) != 0)) {
    *pcVar8 = '\0';
    iVar7 = CONCAT22(local_1c._2_2_,(undefined2)local_1c);
    if ((*(byte *)(param_1 + 0x10) & 0x10) == 0) {
      (*(code *)PTR_FUN_000a9fa4)();
      iVar7 = CONCAT22(local_1c._2_2_,(undefined2)local_1c);
    }
    else if (local_84 == '-') {
      iVar7 = -CONCAT22(local_1c._2_2_,(undefined2)local_1c);
    }
    if ((*(byte *)(param_1 + 0x10) & 4) == 0) {
      if ((*(byte *)(param_1 + 0x10) & 8) == 0) {
        puVar4 = (undefined4 *)*local_28;
        *local_28 = (int)(puVar4 + 1);
        piVar9 = (int *)*puVar4;
      }
      else {
        puVar4 = (undefined4 *)*local_28;
        *local_28 = (int)(puVar4 + 1);
        piVar9 = (int *)*puVar4;
      }
    }
    else {
      puVar3 = (undefined6 *)*local_28;
      *local_28 = (int)(puVar3 + 1);
      piVar9 = (int *)*puVar3;
    }
    bVar1 = *(byte *)(param_1 + 0x10);
    if ((bVar1 & 0x10) == 0) {
      if (((bVar1 & 0x20) != 0) || ((bVar1 & 0x40) != 0)) {
        *piVar9 = local_34;
        piVar9[1] = local_30;
        return iVar11;
      }
      local_1c = iVar7;
      iVar7 = FUN_0007bf60();
    }
    *piVar9 = iVar7;
  }
  return iVar11;
}

