
undefined8 __regparm3 FUN_00043df0(undefined4 *param_1,undefined4 param_2)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  int *extraout_ECX;
  
  *param_1 = 0;
  param_1[1] = &PTR_FUN_000961fc;
  piVar2 = (int *)FUN_00021910();
  piVar1 = piVar2 + -2;
  iVar3 = FUN_0005c320();
  uVar4 = 0;
  if (iVar3 != 0) {
    uVar4 = FUN_00043cf0();
  }
  if (piVar2[2] == piVar2[3]) {
    FUN_00022910();
    piVar2 = extraout_ECX;
  }
  iVar3 = piVar2[2];
  piVar2[2] = iVar3 + 1;
  *(undefined4 *)(iVar3 * 4 + *piVar2) = uVar4;
  return CONCAT44(param_2,piVar1);
}

