
undefined4 __regparm3 FUN_0007773c(int param_1)

{
  code *pcVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  byte bVar7;
  byte bVar8;
  undefined4 uStack_a0;
  char local_9c [136];
  
  bVar8 = 0;
  bVar7 = 0;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar5 = (char *)&uStack_a0;
  if ((bVar7 & 1) == 0) {
    pcVar1 = (code *)swi(0x21);
    cVar2 = (*pcVar1)();
    local_9c[0] = cVar2 + 'A';
    local_9c[2] = 0x5c;
    local_9c[1] = 0x3a;
    iVar4 = -1;
    pcVar6 = local_9c;
    do {
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      cVar2 = *pcVar6;
      pcVar6 = pcVar6 + (uint)bVar8 * -2 + 1;
    } while (cVar2 != '\0');
    if (param_1 == 0) {
      uStack_a0 = 0x777a0;
      iVar4 = crt_malloc();
      pcVar5 = local_9c;
      if (iVar4 == 0) goto LAB_00077762;
    }
    uStack_a0 = 0x777b8;
    uVar3 = FUN_00076f64();
  }
  else {
LAB_00077762:
    *(undefined4 *)(pcVar5 + -4) = 0x77767;
    FUN_00078c12();
    uVar3 = 0;
  }
  return uVar3;
}

