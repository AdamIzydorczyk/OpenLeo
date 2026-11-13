
undefined8 __regparm3 FUN_000751da(char *param_1,undefined4 param_2)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  uint extraout_ECX;
  uint uVar4;
  char *pcVar5;
  byte bVar6;
  
  bVar6 = 0;
  iVar3 = -1;
  pcVar2 = param_1;
  do {
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    cVar1 = *pcVar2;
    pcVar2 = pcVar2 + 1;
  } while (cVar1 != '\0');
  pcVar2 = (char *)crt_malloc();
  if (pcVar2 != (char *)0x0) {
    pcVar5 = pcVar2;
    for (uVar4 = extraout_ECX >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined4 *)pcVar5 = *(undefined4 *)param_1;
      param_1 = param_1 + (uint)bVar6 * -8 + 4;
      pcVar5 = pcVar5 + (uint)bVar6 * -8 + 4;
    }
    for (uVar4 = extraout_ECX & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pcVar5 = *param_1;
      param_1 = param_1 + (uint)bVar6 * -2 + 1;
      pcVar5 = pcVar5 + (uint)bVar6 * -2 + 1;
    }
  }
  return CONCAT44(param_2,pcVar2);
}

