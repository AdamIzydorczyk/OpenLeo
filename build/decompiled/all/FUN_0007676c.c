
int * __regparm3 FUN_0007676c(int *param_1,int param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  byte bVar5;
  char local_30 [16];
  int local_20;
  undefined4 local_1c;
  int local_18;
  
  bVar5 = 0;
  local_18 = FUN_0007aa40();
  if (((param_2 == 0) || ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x400) == 0)) ||
     (local_18 != 10)) {
    local_20 = 0;
  }
  else {
    local_30[0] = '+';
    local_20 = 1;
  }
  FUN_0007aa64(*(undefined4 *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc));
  local_1c = local_20;
  FUN_00079689();
  if ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x200) != 0) {
    FUN_00075e88();
  }
  uVar3 = 0xffffffff;
  pcVar4 = local_30;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + (uint)bVar5 * -2 + 1;
  } while (cVar1 != '\0');
  local_20 = ~uVar3 - 1;
  iVar2 = 0;
  if (*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 0x10) == 0) {
    if ((*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 8) == 0) &&
       ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x4000) == 0)) {
      iVar2 = 1;
    }
    else {
      iVar2 = FUN_0007a648();
    }
  }
  if (iVar2 != 0) {
    FUN_0007a69c();
    FUN_0007a9c0();
    if ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x2000) != 0) {
      FUN_0007a9fc();
    }
  }
  return param_1;
}

