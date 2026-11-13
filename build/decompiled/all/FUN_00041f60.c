
void __regparm3 FUN_00041f60(undefined4 param_1,int param_2)

{
  short sVar1;
  short *extraout_ECX;
  short *extraout_ECX_00;
  short *extraout_ECX_01;
  short *extraout_ECX_02;
  short *extraout_ECX_03;
  short *extraout_ECX_04;
  short *extraout_ECX_05;
  short *psVar2;
  short *extraout_ECX_06;
  byte local_18;
  
  if (*(int *)(param_2 + 1) != 0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
  }
  FUN_00041d40();
  psVar2 = extraout_ECX;
  if (*(int **)(param_2 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
    psVar2 = extraout_ECX_00;
  }
  if (*(int **)(param_2 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
    psVar2 = extraout_ECX_01;
  }
  if (*(int **)(param_2 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
    psVar2 = extraout_ECX_02;
  }
  if (*(int **)(param_2 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
    psVar2 = extraout_ECX_03;
  }
  if ((local_18 & 4) != 0) {
    if (*(int **)(param_2 + 1) != (int *)0x0) {
      (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
      psVar2 = extraout_ECX_04;
    }
    if (*(int **)(param_2 + 1) != (int *)0x0) {
      (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
      psVar2 = extraout_ECX_05;
    }
  }
  if ((local_18 & 3) == 0) {
    if (*psVar2 == 0x7fff) {
      sVar1 = 0;
    }
    else {
      sVar1 = -*psVar2;
    }
    psVar2[6] = sVar1;
    if (*psVar2 == 0x7fff) {
      sVar1 = 0;
    }
    else {
      sVar1 = -psVar2[1];
    }
    psVar2[7] = sVar1;
    psVar2[8] = *(short *)((int)psVar2 + 0x19) * 0x50 + *(short *)((int)psVar2 + 0x15);
    psVar2[9] = *(short *)((int)psVar2 + 0x17);
  }
  else {
    FUN_00041da0();
    FUN_000420b0();
    psVar2 = extraout_ECX_06;
  }
  *(byte *)(psVar2 + 10) = local_18 & 8;
  if (((local_18 & 2) == 0) || ((local_18 & 1) != 0)) {
    FUN_00041e00();
  }
  return;
}

