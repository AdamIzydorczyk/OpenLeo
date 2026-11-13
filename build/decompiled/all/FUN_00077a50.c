
undefined8 __regparm3 FUN_00077a50(undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  for (iVar3 = DAT_000a9b58; iVar3 != 0; iVar3 = *(int *)(iVar3 + 8)) {
    for (piVar1 = *(int **)(iVar3 + 0x28); piVar1 != (int *)(iVar3 + 0x20);
        piVar1 = (int *)piVar1[2]) {
      iVar2 = iVar2 + *piVar1 + -4;
    }
  }
  return CONCAT44(param_2,iVar2);
}

