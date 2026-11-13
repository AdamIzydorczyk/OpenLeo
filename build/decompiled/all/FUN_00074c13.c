
undefined8 __regparm3 FUN_00074c13(char *param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  int extraout_ECX;
  int *piVar4;
  
  if ((DAT_000a9bb4 != (int *)0x0) && (param_1 != (char *)0x0)) {
    uVar3 = 0xffffffff;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *param_1;
      param_1 = param_1 + 1;
    } while (cVar1 != '\0');
    for (piVar4 = DAT_000a9bb4; *piVar4 != 0; piVar4 = piVar4 + 1) {
      iVar2 = FUN_00077e36();
      if ((iVar2 == 0) && (*(char *)(extraout_ECX + (~uVar3 - 1)) == '=')) {
        iVar2 = ~uVar3 + extraout_ECX;
        goto LAB_00074c6d;
      }
    }
  }
  iVar2 = 0;
LAB_00074c6d:
  return CONCAT44(param_2,iVar2);
}

