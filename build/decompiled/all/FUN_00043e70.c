
/* WARNING: Removing unreachable block (ram,0x00043f1b) */

int FUN_00043e70(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *extraout_ECX_01;
  int iVar5;
  short local_14;
  
  FUN_00057990();
  FUN_00043d40();
  FUN_00057a00();
  iVar1 = FUN_00021910();
  FUN_0005c6b0();
  FUN_00057a70();
  FUN_00049aa0();
  FUN_00057a00();
  for (iVar5 = 0; iVar5 < local_14; iVar5 = iVar5 + 1) {
    iVar2 = FUN_0005c320();
    uVar3 = 0;
    piVar4 = extraout_ECX;
    if (iVar2 != 0) {
      uVar3 = FUN_00041ef0();
      piVar4 = extraout_ECX_00;
    }
    if (*(int *)(iVar1 + 8) == *(int *)(iVar1 + 0xc)) {
      FUN_00022910();
      piVar4 = extraout_ECX_01;
    }
    iVar2 = piVar4[2];
    piVar4[2] = iVar2 + 1;
    *(undefined4 *)(*piVar4 + iVar2 * 4) = uVar3;
  }
  FUN_00049af0();
  FUN_00057a00();
  return iVar1 + -8;
}

