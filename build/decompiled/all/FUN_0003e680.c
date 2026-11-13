
int __regparm3 FUN_0003e680(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int unaff_EBX;
  char *pcVar3;
  byte bVar4;
  
  bVar4 = 0;
  if ((((*(int *)(param_1 + 0x5c) == 1) &&
       (iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))(), iVar2 != 0)) &&
      (iVar2 = FUN_00052eb0(), iVar2 != 0)) && (*(int *)(param_1 + 0xf2) != 0)) {
    FUN_00076bcc();
    if (*(char **)(param_1 + 0xfe) != (char *)0x0) {
      iVar2 = -1;
      pcVar3 = *(char **)(param_1 + 0xfe);
      do {
        if (iVar2 == 0) break;
        iVar2 = iVar2 + -1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar3 + (uint)bVar4 * -2 + 1;
      } while (cVar1 != '\0');
    }
    FUN_00052b10();
    FUN_00052cc0();
    *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
    iVar2 = *(int *)(param_1 + 0xf2) + -1;
    *(int *)(param_1 + 0xf2) = iVar2;
    if (iVar2 == 0) {
      *(undefined4 *)(param_1 + 0xad) = 0xffffffff;
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

