
int __regparm3 FUN_0007bbeb(char *param_1,int param_2)

{
  int iVar1;
  char *extraout_ECX;
  char *extraout_ECX_00;
  char *extraout_ECX_01;
  char *pcVar2;
  int iVar3;
  bool bVar4;
  
  if (*(int *)(param_2 + 8) == 0) {
    FUN_0007a448();
    param_1 = extraout_ECX;
  }
  bVar4 = (*(byte *)(param_2 + 0xd) & 4) != 0;
  if (bVar4) {
    *(byte *)(param_2 + 0xd) = *(byte *)(param_2 + 0xd) & 0xf9 | 2;
  }
  iVar3 = 0;
  pcVar2 = param_1;
  do {
    if (*pcVar2 == '\0') goto LAB_0007bc38;
    iVar1 = FUN_0007b9b0();
    pcVar2 = extraout_ECX_00;
  } while (iVar1 != -1);
  iVar3 = -1;
LAB_0007bc38:
  if ((bVar4) && (*(byte *)(param_2 + 0xd) = *(byte *)(param_2 + 0xd) & 0xf9 | 4, iVar3 == 0)) {
    iVar3 = FUN_0007a4e4();
    pcVar2 = extraout_ECX_01;
  }
  if (iVar3 == 0) {
    iVar3 = (int)pcVar2 - (int)param_1;
  }
  return iVar3;
}

