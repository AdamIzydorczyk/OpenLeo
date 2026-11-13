
int * __regparm3 FUN_00057e70(int *param_1,int param_2)

{
  char cVar1;
  bool bVar2;
  char *pcVar3;
  uint uVar4;
  int iVar5;
  int extraout_ECX;
  int unaff_EBX;
  char *pcVar6;
  char *pcVar7;
  byte bVar8;
  
  bVar8 = 0;
  if (((char *)*param_1 == (char *)0x0) || (*(char *)*param_1 == '\0')) {
    bVar2 = true;
  }
  else {
    bVar2 = false;
  }
  if ((!bVar2) && (-1 < param_2)) {
    if ((char *)*param_1 == (char *)0x0) {
      iVar5 = 0;
    }
    else {
      uVar4 = 0xffffffff;
      pcVar3 = (char *)*param_1;
      do {
        if (uVar4 == 0) break;
        uVar4 = uVar4 - 1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar3 + 1;
      } while (cVar1 != '\0');
      iVar5 = ~uVar4 - 1;
    }
    if (iVar5 != unaff_EBX) {
      pcVar3 = (char *)thunk_FUN_0005c320();
      *pcVar3 = '\0';
      FUN_00076f64();
      *(undefined1 *)(extraout_ECX + param_2) = 0;
      if ((char *)*param_1 == (char *)0x0) {
        iVar5 = 0;
      }
      else {
        uVar4 = 0xffffffff;
        pcVar6 = (char *)*param_1;
        do {
          if (uVar4 == 0) break;
          uVar4 = uVar4 - 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar6 + (uint)bVar8 * -2 + 1;
        } while (cVar1 != '\0');
        iVar5 = ~uVar4 - 1;
      }
      if (unaff_EBX + param_2 < iVar5) {
        pcVar6 = (char *)(param_2 + *param_1 + unaff_EBX);
        iVar5 = -1;
        do {
          pcVar7 = pcVar3;
          if (iVar5 == 0) break;
          iVar5 = iVar5 + -1;
          pcVar7 = pcVar3 + (uint)bVar8 * -2 + 1;
          cVar1 = *pcVar3;
          pcVar3 = pcVar7;
        } while (cVar1 != '\0');
        pcVar7 = pcVar7 + -1;
        do {
          cVar1 = *pcVar6;
          *pcVar7 = cVar1;
          if (cVar1 == '\0') break;
          cVar1 = pcVar6[1];
          pcVar6 = pcVar6 + 2;
          pcVar7[1] = cVar1;
          pcVar7 = pcVar7 + 2;
        } while (cVar1 != '\0');
      }
      FUN_00057990();
      thunk_FUN_0005c370();
      return param_1;
    }
    FUN_00057990();
  }
  return param_1;
}

