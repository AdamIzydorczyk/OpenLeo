
undefined4 * __regparm3 FUN_00049700(undefined4 *param_1,char param_2)

{
  char cVar1;
  int iVar2;
  char *unaff_EBX;
  char *pcVar3;
  char *pcVar4;
  
  *param_1 = &PTR_LAB_0009632c;
  DAT_000a90e0 = 0;
  *param_1 = &PTR_LAB_000a11f8;
  pcVar4 = &DAT_000a90e0;
  pcVar3 = (&PTR_s_Nie_mozna_znalezc_pliku_000a94c8)[param_2];
  do {
    cVar1 = *pcVar3;
    *pcVar4 = cVar1;
    if (cVar1 == '\0') break;
    cVar1 = pcVar3[1];
    pcVar3 = pcVar3 + 2;
    pcVar4[1] = cVar1;
    pcVar4 = pcVar4 + 2;
  } while (cVar1 != '\0');
  iVar2 = -1;
  pcVar3 = &DAT_000a90e0;
  do {
    pcVar4 = pcVar3;
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    pcVar4 = pcVar3 + 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar4;
  } while (cVar1 != '\0');
  pcVar4 = pcVar4 + -1;
  do {
    cVar1 = *unaff_EBX;
    *pcVar4 = cVar1;
    if (cVar1 == '\0') {
      return param_1;
    }
    cVar1 = unaff_EBX[1];
    unaff_EBX = unaff_EBX + 2;
    pcVar4[1] = cVar1;
    pcVar4 = pcVar4 + 2;
  } while (cVar1 != '\0');
  return param_1;
}

