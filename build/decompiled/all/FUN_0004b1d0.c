
void __regparm3 FUN_0004b1d0(int param_1)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  char *pcVar7;
  int local_28;
  int local_20;
  
  if (*(char **)(param_1 + 0x30) == (char *)0x0) {
    iVar4 = 0;
  }
  else {
    uVar3 = 0xffffffff;
    pcVar7 = *(char **)(param_1 + 0x30);
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar7 + 1;
    } while (cVar1 != '\0');
    iVar4 = ~uVar3 - 1;
  }
  local_28 = 0;
  do {
    iVar5 = param_1 + local_28 * 4;
    uVar3 = 0;
    local_20 = 0;
    if (0 < *(int *)(iVar5 + 0x44)) {
      do {
        iVar2 = -1;
        iVar6 = 0;
        if (uVar3 < 0x80000000) {
          do {
            if (iVar4 <= iVar6) break;
            if (*(char *)(*(int *)(param_1 + 0x30) + iVar6) == 'A') {
              iVar2 = iVar2 + 1;
            }
            iVar6 = iVar6 + 1;
          } while (iVar2 < (int)uVar3);
        }
        for (iVar2 = *(int *)(*(int *)(iVar5 + 0x58) + local_20); iVar2 != 0;
            iVar2 = *(int *)(iVar2 + 0x34)) {
          (**(code **)(*(int *)(iVar2 + 8) + 0x5c))();
        }
        uVar3 = uVar3 + 1;
        local_20 = local_20 + 4;
      } while ((int)uVar3 < *(int *)(iVar5 + 0x44));
    }
    local_28 = local_28 + 1;
    if (2 < local_28) {
      while (iVar4 = FUN_00057dd0(), iVar4 != -1) {
        FUN_00057e70();
      }
      return;
    }
  } while( true );
}

