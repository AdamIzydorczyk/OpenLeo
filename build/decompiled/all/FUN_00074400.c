
uint __regparm3 FUN_00074400(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_ECX_07;
  uint uVar4;
  int unaff_EBX;
  longdouble lVar5;
  
  switch(unaff_EBX * 2 + (uint)(param_2 == 0)) {
  case 0:
    piVar2 = *(int **)(param_1 + 0x26);
    if ((*(int *)((int)piVar2 + 0xf2) < *piVar2) ||
       (iVar1 = (**(code **)(piVar2[2] + 0x70))(), iVar1 != -1)) {
      return 0;
    }
  case 5:
  case 7:
  case 9:
    uVar3 = 1;
    break;
  case 2:
    piVar2 = *(int **)(param_1 + 0x26);
    if ((*piVar2 < *(int *)((int)piVar2 + 0xf6)) ||
       (iVar1 = (**(code **)(piVar2[2] + 0x70))(), iVar1 != 1)) {
      return 0;
    }
    return 1;
  case 4:
    iVar1 = FUN_0003ff60();
    if (*(int *)(iVar1 + 0x5c) == 5) {
      piVar2 = (int *)FUN_0003ff60();
      uVar3 = **(int **)(extraout_ECX + 0x26) - *piVar2;
      uVar4 = (int)uVar3 >> 0x1f;
      lVar5 = (longdouble)FUN_00075d96(((uVar3 ^ uVar4) - uVar4) + -10);
      *(int *)(extraout_ECX_00 + 0x32) = (int)ROUND(lVar5);
    }
    return 0;
  case 6:
    *(int *)(param_1 + 0x32) = *(int *)(param_1 + 0x32) + -1;
    return 0;
  case 8:
    if (*(int *)(*(int *)(param_1 + 0x26) + 0x5c) == 5) {
      *(undefined4 *)(param_1 + 0x32) = 0xffffffff;
    }
    return 0;
  case 10:
    if (((*(int *)(*(int *)(param_1 + 0x26) + 0x5c) == 5) ||
        (iVar1 = FUN_0003ff60(),
        uVar3 = *(int *)(*(int *)(extraout_ECX_01 + 0x26) + 4) - *(int *)(iVar1 + 4),
        uVar4 = (int)uVar3 >> 0x1f, 0x1d < (int)((uVar3 ^ uVar4) - uVar4))) ||
       ((piVar2 = (int *)FUN_0003ff60(), uVar3 = **(int **)(extraout_ECX_02 + 0x26) - *piVar2,
        uVar4 = (int)uVar3 >> 0x1f, 0x95 < (int)((uVar3 ^ uVar4) - uVar4) ||
        (((piVar2 = (int *)FUN_0003ff60(), uVar3 = **(int **)(extraout_ECX_03 + 0x26) - *piVar2,
          uVar4 = (int)uVar3 >> 0x1f, (int)((uVar3 ^ uVar4) - uVar4) < 0x5b ||
          (iVar1 = FUN_0003ff20(0,0x40140000), iVar1 == 0)) ||
         ((*(byte *)(*(int *)(extraout_ECX_04 + 0x26) + 0x62) & 4) != 0)))))) {
      return 0;
    }
    return 1;
  case 0xb:
  case 0xd:
    *(undefined4 *)(param_1 + 0x32) = 0xffffffff;
  case 1:
  case 3:
  case 0xf:
    return *(uint *)(param_1 + 0x1a);
  case 0xc:
    if ((((*(int *)(*(int *)(param_1 + 0x26) + 0x5c) == 5) ||
         (iVar1 = FUN_0003ff60(),
         uVar3 = *(int *)(*(int *)(extraout_ECX_05 + 0x26) + 4) - *(int *)(iVar1 + 4),
         uVar4 = (int)uVar3 >> 0x1f, 0x1d < (int)((uVar3 ^ uVar4) - uVar4))) ||
        (piVar2 = (int *)FUN_0003ff60(), uVar3 = **(int **)(extraout_ECX_06 + 0x26) - *piVar2,
        uVar4 = (int)uVar3 >> 0x1f, 0xf9 < (int)((uVar3 ^ uVar4) - uVar4))) ||
       ((iVar1 = FUN_0003ff20(0,0x40040000), iVar1 == 0 ||
        ((*(byte *)(*(int *)(extraout_ECX_07 + 0x26) + 0x62) & 4) != 0)))) {
      return 0;
    }
    return 1;
  case 0xe:
    if (*(int *)(param_1 + 0x32) == 0) {
      *(undefined4 *)(param_1 + 0x32) = 0xffffffff;
      return (uint)(*(int *)(*(int *)(param_1 + 0x26) + 0x5c) == 1);
    }
    return 0;
  case 0x10:
    return (uint)(*(int *)(*(int *)(param_1 + 0x26) + 0x5c) != 5);
  case 0x11:
    uVar3 = FUN_0003fdb0();
    return uVar3;
  default:
    ll_printf(&DAT_000a92d4,s_Blad_zakresu_w_TExtSeqenceCLASSN_00095b4e);
    ll_rethrow_report();
    uVar3 = 0;
  }
  return uVar3;
}

