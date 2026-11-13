
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_000813d2(undefined4 param_1,undefined4 param_2,int param_3)

{
  short *psVar1;
  int extraout_ECX;
  int iVar2;
  undefined4 *puVar3;
  double *pdVar4;
  longdouble extraout_ST0;
  longdouble lVar5;
  undefined8 uVar6;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined1 auStack_1c [8];
  undefined1 local_c [4];
  
  pdVar4 = (double *)auStack_1c;
  puVar3 = (undefined4 *)auStack_1c;
  if ((double)CONCAT44(param_2,param_1) == _DAT_000a6be8) {
    param_1 = DAT_000a6bf8;
    param_2 = DAT_000a6bfc;
  }
  else if ((double)CONCAT44(param_2,param_1) == _DAT_000a6bf0) {
    param_1 = DAT_000a6c00;
    param_2 = DAT_000a6c04;
  }
  psVar1 = &DAT_000a6c08;
  if (param_3 < 0) {
    iVar2 = -param_3;
    pdVar4 = (double *)auStack_1c;
    if (param_3 == -0x134 || iVar2 < 0x134) goto LAB_0008147f;
    if ((DAT_000ad610 & 1) == 0) {
      lVar5 = (longdouble)(double)CONCAT44(param_2,param_1) /
              (longdouble)(double)CONCAT44(DAT_000a6c0e,DAT_000a6c0a);
    }
    else {
      uStack_20 = DAT_000a6c0e;
      local_24 = DAT_000a6c0a;
      uVar6 = FUN_000823f4();
      iVar2 = (int)((ulonglong)uVar6 >> 0x20);
      psVar1 = (short *)uVar6;
      puVar3 = &local_24;
      param_3 = extraout_ECX;
      lVar5 = extraout_ST0;
    }
  }
  else {
    iVar2 = param_3;
    if (param_3 < 0x135) goto LAB_0008147f;
    lVar5 = (longdouble)(double)CONCAT44(param_2,param_1) *
            (longdouble)(double)CONCAT44(DAT_000a6c0e,DAT_000a6c0a);
    puVar3 = (undefined4 *)auStack_1c;
  }
  *(double *)((int)puVar3 + 0x20) = (double)lVar5;
  iVar2 = iVar2 + -0xd8;
  pdVar4 = (double *)puVar3;
LAB_0008147f:
  *(undefined4 *)pdVar4 = 0;
  *(undefined4 *)((int)pdVar4 + 4) = 0x3ff00000;
  while( true ) {
    if (*psVar1 <= iVar2) {
      iVar2 = iVar2 - *psVar1;
      *pdVar4 = *pdVar4 * *(double *)(psVar1 + 1);
    }
    if (iVar2 == 0) break;
    if (*psVar1 != 1) {
      psVar1 = psVar1 + 5;
    }
  }
  if (param_3 < 0) {
    if ((DAT_000ad610 & 1) == 0) {
      lVar5 = (longdouble)pdVar4[4] / (longdouble)*pdVar4;
    }
    else {
      *(undefined4 *)((int)pdVar4 + -4) = *(undefined4 *)((int)pdVar4 + 4);
      *(undefined4 *)(pdVar4 + -1) = *(undefined4 *)pdVar4;
      *(undefined4 *)((int)pdVar4 + -0xc) = 0x814ce;
      lVar5 = (longdouble)FUN_000823f4();
      pdVar4 = pdVar4 + -1;
    }
  }
  else {
    lVar5 = (longdouble)pdVar4[4] * (longdouble)*pdVar4;
  }
  *(double *)((int)pdVar4 + 0x20) = (double)lVar5;
  return CONCAT44(*(undefined4 *)((int)pdVar4 + 0xc),*(undefined4 *)((int)pdVar4 + 0x18));
}

