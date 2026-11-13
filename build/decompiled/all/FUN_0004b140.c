
int __regparm3 FUN_0004b140(int param_1,int param_2)

{
  char *pcVar1;
  int extraout_ECX;
  int iVar2;
  
  if (param_2 == *(int *)(param_1 + 0x394)) {
    iVar2 = -1;
    for (pcVar1 = *(char **)(param_1 + 0x30); *pcVar1 != 'L'; pcVar1 = pcVar1 + 1) {
      if (*pcVar1 == 'A') {
        iVar2 = iVar2 + 1;
      }
    }
    if (iVar2 == -1) {
      iVar2 = 0;
    }
  }
  else {
    do {
      iVar2 = FUN_0004b0f0();
      if (iVar2 != 0) {
        return extraout_ECX;
      }
    } while (extraout_ECX + 1 < 5);
    ll_printf(&DAT_000a92d4,s_Nie_znaleziono_obiektu_na_liscie_00091d46);
    ll_rethrow_report();
    iVar2 = -1;
  }
  return iVar2;
}

