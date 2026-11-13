
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __regparm3 FUN_000416d0(int param_1)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  undefined4 *extraout_ECX;
  undefined4 *extraout_ECX_00;
  int *extraout_ECX_01;
  int *extraout_ECX_02;
  uint uVar4;
  undefined4 uVar5;
  longdouble lVar6;
  int iVar7;
  int iVar8;
  undefined4 uVar9;
  undefined **ppuVar10;
  
  iVar8 = 0;
  uVar9 = 0;
  ppuVar10 = &PTR_LAB_00098278;
  piVar2 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
  iVar7 = *piVar2;
  uVar5 = 0;
  iVar3 = (**(code **)(*(int *)(param_1 + 8) + 0x108))
                    (iVar7,piVar2[1],&PTR_LAB_00098278,iVar8,uVar9,ppuVar10);
  if (iVar3 == 0) {
    *extraout_ECX = 0;
    extraout_ECX[1] = 0;
    uVar5 = 0;
  }
  else {
    lVar6 = (longdouble)FUN_00076ce8();
    if (lVar6 <= (longdouble)_DAT_00091681) {
      *extraout_ECX_00 = 0;
      extraout_ECX_00[1] = 0;
      uVar5 = 1;
    }
    else {
      iVar3 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
      if (((float)iVar3 * (float)(iVar8 - iVar7) < 0.0) &&
         ((float)*(int *)(param_1 + 0xfa) < ABS((float)(iVar8 - iVar7)))) {
        FUN_00053670();
      }
      (**(code **)(*(int *)(param_1 + 8) + 0x70))();
      lVar6 = (longdouble)FUN_00075d96();
      *extraout_ECX_01 = (int)ROUND(lVar6);
      lVar6 = (longdouble)FUN_00075d96();
      extraout_ECX_02[1] = (int)ROUND(lVar6);
      cVar1 = *(char *)(param_1 + *(char *)(param_1 + 0x160) + 0x13b);
      if ((cVar1 == 'D') || (cVar1 == 'd')) {
        if (*extraout_ECX_02 < 0) {
          uVar4 = 0xffffffff;
        }
        else {
          uVar4 = (uint)(0 < *extraout_ECX_02);
        }
        *extraout_ECX_02 = uVar4 * 10;
        if (extraout_ECX_02[1] < 0) {
          uVar4 = 0xffffffff;
        }
        else {
          uVar4 = (uint)(0 < extraout_ECX_02[1]);
        }
        extraout_ECX_02[1] = uVar4 * 10;
      }
    }
  }
  return uVar5;
}

