
undefined8 __regparm3 FUN_00077a14(undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  
  uVar2 = 0;
  for (iVar3 = DAT_000a9b58; iVar3 != 0; iVar3 = *(int *)(iVar3 + 8)) {
    for (piVar1 = *(int **)(iVar3 + 0x28); piVar1 != (int *)(iVar3 + 0x20);
        piVar1 = (int *)piVar1[2]) {
      if (uVar2 < *piVar1 - 4U) {
        uVar2 = *piVar1 - 4U;
      }
    }
  }
  return CONCAT44(param_2,uVar2);
}

