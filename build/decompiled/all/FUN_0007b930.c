
uint FUN_0007b930(void)

{
  code *pcVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  uint extraout_EDX;
  uint unaff_EBX;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  undefined1 auStack_14 [4];
  
  uVar3 = FUN_0007f975();
  puVar6 = (undefined4 *)auStack_14;
  if ((uVar3 & 0x80) != 0) {
    bVar7 = 0;
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    puVar5 = (undefined4 *)(auStack_14 + 4);
    puVar6 = (undefined4 *)(auStack_14 + 4);
    if ((bVar7 & 1) != 0) goto LAB_0007b968;
  }
  bVar7 = 0;
  pcVar1 = (code *)swi(0x21);
  iVar4 = (*pcVar1)();
  puVar5 = puVar6 + 1;
  bVar2 = (bVar7 & 1) != 0;
  uVar3 = (iVar4 << 1 | (uint)bVar7) >> 1 | (uint)bVar2 << 0x1f;
  puVar6[1] = uVar3;
  if (!bVar2) {
    if (uVar3 != unaff_EBX) {
      *puVar6 = 0x7b99a;
      FUN_00078c12();
      uVar3 = extraout_EDX;
    }
    return uVar3;
  }
LAB_0007b968:
  *(undefined4 *)((int)puVar5 + -4) = 0x7b973;
  uVar3 = FUN_0007ba5f();
  return uVar3;
}

