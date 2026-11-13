
void __regparm3 FUN_0004af10(int param_1)

{
  uint uVar1;
  undefined4 extraout_ECX;
  char *pcVar2;
  char *pcVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  
  iVar4 = 0;
  uVar6 = 0;
  pcVar3 = *(char **)(param_1 + 0x30);
  if (*(int *)(param_1 + 0x3a0) != 0) {
    FUN_0001cfd0();
  }
  iVar5 = param_1 + 0x3ce;
LAB_0004af40:
  pcVar2 = pcVar3;
  if (*pcVar3 == '\0') {
    return;
  }
LAB_0004b076:
  pcVar3 = pcVar2 + 1;
  uVar1 = FUN_00076e98();
  if (uVar1 < 0x42) {
    if (uVar1 == 0x41) {
      if (*(int *)(param_1 + 0x3a0) != 0) goto LAB_0004af40;
      FUN_0004b360();
      pcVar2 = pcVar3;
      uVar6 = extraout_ECX;
      if (*pcVar3 == '\0') {
        return;
      }
      goto LAB_0004b076;
    }
  }
  else {
    if (uVar1 < 0x43) {
      if (*(int *)(iVar5 + 8) != 0) {
        FUN_0004a170();
        FUN_00049bd0(*(undefined4 *)(param_1 + 0x3b4),iVar5,uVar6);
      }
      if ((*(int *)(param_1 + 0x3a0) != 0) || (*(int *)(param_1 + 0x38c) == 0)) goto LAB_0004af40;
      FUN_000448c0();
      pcVar2 = pcVar3;
      if (*pcVar3 == '\0') {
        return;
      }
      goto LAB_0004b076;
    }
    if (0x4b < uVar1) {
      if (uVar1 < 0x4d) {
        (**(code **)(*(int *)(*(int *)(param_1 + 0x394) + 8) + 0x74))();
        FUN_0004b2f0();
        pcVar2 = pcVar3;
        if (*pcVar3 == '\0') {
          return;
        }
        goto LAB_0004b076;
      }
      if (uVar1 != 0x54) goto code_r0x0004b0b0;
      if (*(int *)(param_1 + 0x3a0) == 0) {
        if ((iVar4 == 0) && (*pcVar3 == 'T')) {
          FUN_00057780();
          pcVar2 = pcVar2 + 2;
          iVar4 = 2;
          if (*pcVar2 == '\0') {
            return;
          }
        }
        else {
          FUN_00057710();
          iVar4 = iVar4 + 1;
          pcVar2 = pcVar3;
          if (*pcVar3 == '\0') {
            return;
          }
        }
        goto LAB_0004b076;
      }
      goto LAB_0004af40;
    }
  }
code_r0x0004b0b0:
  ll_printf(&DAT_000a92d4,s_Bledny_opis_poziomu___plany_00091d2a);
  ll_rethrow_report();
  pcVar2 = pcVar3;
  if (*pcVar3 == '\0') {
    return;
  }
  goto LAB_0004b076;
}

