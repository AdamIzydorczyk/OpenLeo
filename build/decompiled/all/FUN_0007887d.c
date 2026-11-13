
int __regparm3 FUN_0007887d(int param_1,int *param_2,int param_3)

{
  byte bVar1;
  undefined6 *puVar2;
  undefined4 *puVar3;
  int iVar4;
  int *piVar5;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar6;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int iVar7;
  longlong lVar8;
  undefined8 uVar9;
  int local_20;
  int local_18;
  
  iVar7 = 0;
  local_20 = 0;
  while( true ) {
    lVar8 = FUN_00077e89();
    iVar4 = (int)lVar8;
    if (((&DAT_000a68b4)[(byte)((char)lVar8 + 1)] & 2) == 0) break;
    local_20 = local_20 + 1;
  }
  iVar6 = extraout_ECX;
  if ((*(byte *)(param_1 + 0x10) & 2) == 0) {
    iVar6 = *(int *)(param_1 + 0xc);
    *(int *)(param_1 + 0xc) = iVar6 + -1;
    if (iVar6 == 0) {
LAB_000789ec:
      FUN_00077e90();
      iVar6 = extraout_ECX_05;
    }
    else {
      local_18 = 0x2b;
      if ((param_3 != 0) && ((iVar4 == 0x2b || (iVar4 == 0x2d)))) {
        local_20 = local_20 + 1;
        lVar8 = FUN_00078a94();
        iVar6 = extraout_ECX_00;
        local_18 = iVar4;
        if ((int)lVar8 == -1) goto LAB_000789f5;
      }
      iVar4 = (int)lVar8;
      if ((int)((ulonglong)lVar8 >> 0x20) == 0) {
        if (iVar4 == 0x30) {
          iVar4 = FUN_00078a94();
          iVar6 = extraout_ECX_01;
          if (iVar4 == -1) goto LAB_000789f5;
          if ((iVar4 == 0x78) || (iVar4 == 0x58)) {
            iVar4 = FUN_00078a94();
            iVar6 = 0;
            local_20 = local_20 + 2;
            if (iVar4 == -1) goto LAB_000789f5;
          }
        }
      }
      else if (lVar8 == 0x1000000030) {
        iVar4 = FUN_00078a94();
        iVar6 = extraout_ECX_02;
        if (iVar4 == -1) goto LAB_000789f5;
        if ((iVar4 == 0x78) || (iVar4 == 0x58)) {
          iVar4 = FUN_00078a94();
          local_20 = local_20 + 2;
          iVar6 = 0;
          goto LAB_000789a1;
        }
      }
      do {
        uVar9 = FUN_00078a6d();
        iVar6 = (int)((ulonglong)uVar9 >> 0x20);
        if (iVar6 <= (int)uVar9) {
          if ((iVar4 != 0x3a) || ((*(byte *)(param_1 + 0x10) & 0x80) == 0)) goto LAB_000789ec;
          goto LAB_000789cd;
        }
        iVar7 = iVar7 * iVar6 + (int)uVar9;
        iVar4 = FUN_00078a94();
        iVar6 = extraout_ECX_03;
LAB_000789a1:
      } while (iVar4 != -1);
    }
  }
LAB_000789f5:
  if (local_18 == 0x2d) {
    iVar7 = -iVar7;
  }
  if (0 < iVar6) {
    bVar1 = *(byte *)(param_1 + 0x10);
    iVar6 = iVar6 + local_20;
    if ((bVar1 & 1) != 0) {
      if ((bVar1 & 4) == 0) {
        if ((bVar1 & 8) == 0) {
          puVar3 = (undefined4 *)*param_2;
          *param_2 = (int)(puVar3 + 1);
          piVar5 = (int *)*puVar3;
        }
        else {
          puVar3 = (undefined4 *)*param_2;
          *param_2 = (int)(puVar3 + 1);
          piVar5 = (int *)*puVar3;
        }
      }
      else {
        puVar2 = (undefined6 *)*param_2;
        *param_2 = (int)(puVar2 + 1);
        piVar5 = (int *)*puVar2;
      }
      if ((*(byte *)(param_1 + 0x10) & 0x10) == 0) {
        *piVar5 = iVar7;
      }
      else {
        *(short *)piVar5 = (short)iVar7;
      }
    }
  }
  return iVar6;
LAB_000789cd:
  iVar4 = FUN_00078a94();
  iVar6 = extraout_ECX_04;
  if (iVar4 == -1) goto LAB_000789f5;
  uVar9 = FUN_00078a6d();
  iVar4 = (int)((ulonglong)uVar9 >> 0x20);
  if (iVar4 <= (int)uVar9) goto LAB_000789ec;
  iVar7 = iVar7 * iVar4 + (int)uVar9;
  goto LAB_000789cd;
}

