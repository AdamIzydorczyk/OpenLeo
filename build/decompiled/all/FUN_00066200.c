
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __regparm3 FUN_00066200(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined4 uVar5;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  uint uVar6;
  int unaff_EBX;
  undefined8 uVar7;
  
  switch(unaff_EBX * 2 + (uint)(param_2 == 0)) {
  case 0:
    (**(code **)(*(int *)(*(int *)(param_1 + 0x26) + 8) + 0x6c))();
    iVar2 = FUN_0003ff60();
    uVar7 = (**(code **)(*(int *)(iVar2 + 8) + 0x6c))();
    uVar3 = *(int *)((int)uVar7 + 4) - *(int *)((int)((ulonglong)uVar7 >> 0x20) + 4);
    uVar6 = (int)uVar3 >> 0x1f;
    if (((0x4f < (int)((uVar3 ^ uVar6) - uVar6)) || (iVar2 = FUN_0003ff20(0,0x3ff00000), iVar2 == 0)
        ) || ((piVar4 = (int *)FUN_0003ff60(), uVar3 = **(int **)(extraout_ECX + 0x26) - *piVar4,
              uVar6 = (int)uVar3 >> 0x1f, (int)((uVar3 ^ uVar6) - uVar6) < 0x1e ||
              (piVar4 = (int *)FUN_0003ff60(), uVar3 = **(int **)(extraout_ECX_00 + 0x26) - *piVar4,
              uVar6 = (int)uVar3 >> 0x1f, 0x3b < (int)((uVar3 ^ uVar6) - uVar6))))) {
      return 0;
    }
    uVar5 = 1;
    break;
  case 1:
  case 3:
  case 5:
  case 7:
    return *(undefined4 *)(param_1 + 0x1a);
  case 2:
    (**(code **)(*(int *)(*(int *)(param_1 + 0x26) + 8) + 0x6c))();
    iVar2 = FUN_0003ff60();
    uVar7 = (**(code **)(*(int *)(iVar2 + 8) + 0x6c))();
    uVar3 = *(int *)((int)uVar7 + 4) - *(int *)((int)((ulonglong)uVar7 >> 0x20) + 4);
    uVar6 = (int)uVar3 >> 0x1f;
    if (((((int)((uVar3 ^ uVar6) - uVar6) < 0x50) &&
         (iVar2 = FUN_0003ff20(0,0x3ff00000), iVar2 != 0)) &&
        (piVar4 = (int *)FUN_0003ff60(), uVar3 = **(int **)(extraout_ECX_01 + 0x26) - *piVar4,
        uVar6 = (int)uVar3 >> 0x1f, 0x1d < (int)((uVar3 ^ uVar6) - uVar6))) &&
       (piVar4 = (int *)FUN_0003ff60(), uVar3 = **(int **)(extraout_ECX_02 + 0x26) - *piVar4,
       uVar6 = (int)uVar3 >> 0x1f, ABS((double)(int)((uVar3 ^ uVar6) - uVar6)) < _DAT_00093bb8)) {
      return 1;
    }
    return 0;
  case 4:
    iVar2 = (**(code **)(*(int *)(*(int *)(param_1 + 0x26) + 8) + 0x6c))();
    iVar1 = FUN_0003ff60();
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 0x6c))();
    uVar3 = *(int *)(iVar1 + 4) - *(int *)(iVar2 + 4);
    uVar6 = (int)uVar3 >> 0x1f;
    if ((((int)((uVar3 ^ uVar6) - uVar6) < 0x50) &&
        (piVar4 = (int *)FUN_0003ff60(), **(int **)(extraout_ECX_03 + 0x26) < *piVar4 + 0x1e)) &&
       ((uVar7 = FUN_0003ff60(), *(int *)uVar7 < *(int *)((ulonglong)uVar7 >> 0x20) &&
        ((iVar2 = (**(code **)(*(int *)(*(int *)(extraout_ECX_04 + 0x26) + 8) + 0x70))(),
         iVar2 == -1 && (iVar2 = FUN_0003ff20(0x33333333,0x3fd33333), iVar2 != 0)))))) {
      return 1;
    }
    return 0;
  case 6:
    (**(code **)(*(int *)(*(int *)(param_1 + 0x26) + 8) + 0x6c))();
    iVar2 = FUN_0003ff60();
    uVar7 = (**(code **)(*(int *)(iVar2 + 8) + 0x6c))();
    uVar3 = *(int *)((int)uVar7 + 4) - *(int *)((int)((ulonglong)uVar7 >> 0x20) + 4);
    uVar6 = (int)uVar3 >> 0x1f;
    if ((((0x4f < (int)((uVar3 ^ uVar6) - uVar6)) ||
         (piVar4 = (int *)FUN_0003ff60(), **(int **)(extraout_ECX_05 + 0x26) <= *piVar4 + -0x1e)) ||
        (uVar7 = FUN_0003ff60(), *(int *)uVar7 <= *(int *)((ulonglong)uVar7 >> 0x20))) ||
       ((iVar2 = (**(code **)(*(int *)(*(int *)(extraout_ECX_06 + 0x26) + 8) + 0x70))(), iVar2 != 1
        || (iVar2 = FUN_0003ff20(0x33333333,0x3fd33333), iVar2 == 0)))) {
      return 0;
    }
  case 8:
    return 1;
  case 9:
    uVar5 = FUN_0003fdb0();
    return uVar5;
  default:
    ll_printf(&DAT_000a92d4,s_Blad_zakresu_w_TExtSeqenceCLASSN_00093b85);
    ll_rethrow_report();
    uVar5 = 0;
  }
  return uVar5;
}

