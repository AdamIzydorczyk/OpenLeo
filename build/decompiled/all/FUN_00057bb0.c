
undefined4 * __regparm3 FUN_00057bb0(undefined4 *param_1,char *param_2)

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  byte bVar7;
  
  bVar7 = 0;
  if (param_2 == (char *)0x0) {
    iVar4 = 0;
  }
  else {
    uVar3 = 0xffffffff;
    pcVar2 = param_2;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar2;
      pcVar2 = pcVar2 + 1;
    } while (cVar1 != '\0');
    iVar4 = ~uVar3 - 1;
  }
  if (iVar4 != 0) {
    if ((char *)*param_1 != (char *)0x0) {
      iVar4 = -1;
      pcVar2 = (char *)*param_1;
      do {
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        cVar1 = *pcVar2;
        pcVar2 = pcVar2 + 1;
      } while (cVar1 != '\0');
    }
    pcVar2 = (char *)thunk_FUN_0005c320();
    *pcVar2 = '\0';
    pcVar5 = pcVar2;
    do {
      cVar1 = *param_2;
      *pcVar5 = cVar1;
      if (cVar1 == '\0') break;
      cVar1 = param_2[1];
      param_2 = param_2 + 2;
      pcVar5[1] = cVar1;
      pcVar5 = pcVar5 + 2;
    } while (cVar1 != '\0');
    pcVar5 = (char *)*param_1;
    if (pcVar5 != (char *)0x0) {
      iVar4 = -1;
      do {
        pcVar6 = pcVar2;
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        pcVar6 = pcVar2 + (uint)bVar7 * -2 + 1;
        cVar1 = *pcVar2;
        pcVar2 = pcVar6;
      } while (cVar1 != '\0');
      pcVar6 = pcVar6 + -1;
      do {
        cVar1 = *pcVar5;
        *pcVar6 = cVar1;
        if (cVar1 == '\0') break;
        cVar1 = pcVar5[1];
        pcVar5 = pcVar5 + 2;
        pcVar6[1] = cVar1;
        pcVar6 = pcVar6 + 2;
      } while (cVar1 != '\0');
    }
    FUN_00057990();
    thunk_FUN_0005c370();
  }
  return param_1;
}

