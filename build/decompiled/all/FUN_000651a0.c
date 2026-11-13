
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_000651a0(int param_1,undefined4 param_2)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int *piVar6;
  longdouble lVar7;
  double dVar8;
  
  piVar6 = (int *)(param_1 + 0x32 + *(int *)(param_1 + 0x62) * 0xc);
  uVar1 = **(int **)(param_1 + 0x26) - *piVar6;
  uVar4 = (int)uVar1 >> 0x1f;
  uVar2 = (*(int **)(param_1 + 0x26))[1] - piVar6[1];
  uVar5 = (int)uVar2 >> 0x1f;
  iVar3 = ((uVar2 ^ uVar5) - uVar5) + ((uVar1 ^ uVar4) - uVar4) + 1;
  if (iVar3 < 6) {
    dVar8 = 0.0;
  }
  else {
    dVar8 = ((double)(*(int *)(param_1 + 0x32 + *(int *)(param_1 + 0x62) * 0xc) -
                     **(int **)(param_1 + 0x26)) * _DAT_00093979) / (double)iVar3;
  }
  lVar7 = (longdouble)FUN_00075d96(dVar8);
  return CONCAT44(param_2,(int)ROUND(lVar7));
}

