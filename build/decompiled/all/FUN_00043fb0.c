
undefined8 __regparm3 FUN_00043fb0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar3;
  int *extraout_ECX_01;
  int *piVar4;
  
  iVar1 = FUN_00021880();
  *(undefined ***)(iVar1 + 0x16) = &PTR_LAB_000a0bd0;
  iVar1 = FUN_0005c320();
  uVar2 = 0;
  piVar3 = extraout_ECX;
  if (iVar1 != 0) {
    uVar2 = FUN_00043df0();
    piVar3 = extraout_ECX_00;
  }
  piVar4 = piVar3;
  if (piVar3[2] == piVar3[3]) {
    FUN_00022780();
    piVar4 = extraout_ECX_01;
  }
  iVar1 = piVar3[2];
  piVar3[2] = iVar1 + 1;
  *(undefined4 *)(iVar1 * 4 + *piVar3) = uVar2;
  return CONCAT44(param_2,piVar4);
}

