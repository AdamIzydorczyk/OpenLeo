
void __regparm3 FUN_00057990(undefined4 *param_1)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  char *extraout_EDX;
  char *pcVar4;
  char *pcVar5;
  byte bVar6;
  undefined8 uVar7;
  
  bVar6 = 0;
  FUN_000579e0();
  if (extraout_EDX == (char *)0x0) {
    iVar3 = 0;
  }
  else {
    uVar2 = 0xffffffff;
    pcVar4 = extraout_EDX;
    do {
      if (uVar2 == 0) break;
      uVar2 = uVar2 - 1;
      cVar1 = *pcVar4;
      pcVar4 = pcVar4 + (uint)bVar6 * -2 + 1;
    } while (cVar1 != '\0');
    iVar3 = ~uVar2 - 1;
  }
  if (iVar3 != 0) {
    uVar7 = thunk_FUN_0005c320();
    *param_1 = (int)uVar7;
    do {
      pcVar4 = (char *)((ulonglong)uVar7 >> 0x20);
      pcVar5 = (char *)uVar7;
      cVar1 = *pcVar4;
      *pcVar5 = cVar1;
      if (cVar1 == '\0') {
        return;
      }
      cVar1 = pcVar4[1];
      pcVar5[1] = cVar1;
      uVar7 = CONCAT44(pcVar4 + 2,pcVar5 + 2);
    } while (cVar1 != '\0');
  }
  return;
}

