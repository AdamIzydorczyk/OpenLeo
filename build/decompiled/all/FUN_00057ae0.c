
undefined4 * __regparm3 FUN_00057ae0(undefined4 *param_1,char *param_2)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  char *pcVar6;
  char *pcVar7;
  byte bVar8;
  undefined8 uVar9;
  
  bVar8 = 0;
  if (param_2 == (char *)0x0) {
    iVar3 = 0;
  }
  else {
    uVar2 = 0xffffffff;
    do {
      if (uVar2 == 0) break;
      uVar2 = uVar2 - 1;
      cVar1 = *param_2;
      param_2 = param_2 + 1;
    } while (cVar1 != '\0');
    iVar3 = ~uVar2 - 1;
  }
  if (iVar3 != 0) {
    if ((char *)*param_1 != (char *)0x0) {
      iVar3 = -1;
      pcVar4 = (char *)*param_1;
      do {
        if (iVar3 == 0) break;
        iVar3 = iVar3 + -1;
        cVar1 = *pcVar4;
        pcVar4 = pcVar4 + 1;
      } while (cVar1 != '\0');
    }
    uVar9 = thunk_FUN_0005c320();
    pcVar5 = (char *)((ulonglong)uVar9 >> 0x20);
    pcVar7 = (char *)uVar9;
    *pcVar7 = '\0';
    pcVar4 = (char *)*param_1;
    pcVar6 = pcVar7;
    if (pcVar4 == (char *)0x0) {
      do {
        pcVar4 = (char *)((ulonglong)uVar9 >> 0x20);
        pcVar7 = (char *)uVar9;
        cVar1 = *pcVar4;
        *pcVar7 = cVar1;
        if (cVar1 == '\0') break;
        cVar1 = pcVar4[1];
        pcVar7[1] = cVar1;
        uVar9 = CONCAT44(pcVar4 + 2,pcVar7 + 2);
      } while (cVar1 != '\0');
    }
    else {
      do {
        cVar1 = *pcVar4;
        *pcVar6 = cVar1;
        if (cVar1 == '\0') break;
        cVar1 = pcVar4[1];
        pcVar4 = pcVar4 + 2;
        pcVar6[1] = cVar1;
        pcVar6 = pcVar6 + 2;
      } while (cVar1 != '\0');
      iVar3 = -1;
      do {
        pcVar4 = pcVar7;
        if (iVar3 == 0) break;
        iVar3 = iVar3 + -1;
        pcVar4 = pcVar7 + (uint)bVar8 * -2 + 1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar4;
      } while (cVar1 != '\0');
      pcVar4 = pcVar4 + -1;
      do {
        cVar1 = *pcVar5;
        *pcVar4 = cVar1;
        if (cVar1 == '\0') break;
        cVar1 = pcVar5[1];
        pcVar5 = pcVar5 + 2;
        pcVar4[1] = cVar1;
        pcVar4 = pcVar4 + 2;
      } while (cVar1 != '\0');
    }
    FUN_00057990();
    thunk_FUN_0005c370();
  }
  return param_1;
}

