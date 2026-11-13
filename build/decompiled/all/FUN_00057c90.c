
undefined8 __regparm3 FUN_00057c90(undefined4 *param_1,undefined4 param_2)

{
  char cVar1;
  bool bVar2;
  char *pcVar3;
  int iVar4;
  char *pcVar5;
  
  if (((char *)*param_1 == (char *)0x0) || (*(char *)*param_1 == '\0')) {
    bVar2 = true;
  }
  else {
    bVar2 = false;
  }
  if (!bVar2) {
    if ((char *)*param_1 != (char *)0x0) {
      iVar4 = -1;
      pcVar5 = (char *)*param_1;
      do {
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar5 + 1;
      } while (cVar1 != '\0');
    }
    pcVar3 = (char *)thunk_FUN_0005c320();
    *pcVar3 = '\0';
    pcVar5 = (char *)*param_1;
    do {
      cVar1 = *pcVar5;
      *pcVar3 = cVar1;
      if (cVar1 == '\0') break;
      cVar1 = pcVar5[1];
      pcVar5 = pcVar5 + 2;
      pcVar3[1] = cVar1;
      pcVar3 = pcVar3 + 2;
    } while (cVar1 != '\0');
    FUN_00075e88();
    FUN_00057990();
    thunk_FUN_0005c370();
  }
  return CONCAT44(param_2,param_1);
}

