
void FUN_0005be50(void)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  
  pcVar2 = s_BLASTER__00092449;
  pcVar3 = &DAT_000cf28c;
  do {
    cVar1 = *pcVar2;
    *pcVar3 = cVar1;
    if (cVar1 == '\0') break;
    cVar1 = pcVar2[1];
    pcVar2 = pcVar2 + 2;
    pcVar3[1] = cVar1;
    pcVar3 = pcVar3 + 2;
  } while (cVar1 != '\0');
  pcVar2 = s_ULTRASND__00092452;
  pcVar3 = &DAT_000cf258;
  do {
    cVar1 = *pcVar2;
    *pcVar3 = cVar1;
    if (cVar1 == '\0') break;
    cVar1 = pcVar2[1];
    pcVar2 = pcVar2 + 2;
    pcVar3[1] = cVar1;
    pcVar3 = pcVar3 + 2;
  } while (cVar1 != '\0');
  FUN_000777c4();
  FUN_000777c4();
  return;
}

