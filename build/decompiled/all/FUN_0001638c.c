
undefined8 FUN_0001638c(void)

{
  code *pcVar1;
  short sVar2;
  int iVar4;
  int iVar3;
  undefined4 uVar5;
  undefined1 uVar6;
  undefined1 *puVar7;
  uint uVar8;
  bool bVar9;
  undefined1 auStack_94 [128];
  
  puVar7 = auStack_94 + 0x80;
  uVar8 = 0;
  iVar4 = 0;
  do {
    *(undefined4 *)((int)&DAT_000b33ec + iVar4) = 0;
    *(undefined2 *)((int)&DAT_000b33d2 + iVar4) = 0;
    *(undefined2 *)((int)&DAT_000b33d4 + iVar4) = 0;
    (&DAT_000b33d0)[iVar4] = 0;
    (&DAT_000b33d1)[iVar4] = 0;
    (&DAT_000b33ea)[iVar4] = 0;
    *(undefined4 *)((int)&DAT_000b33e6 + iVar4) = 10000;
    if ((uVar8 & 1) == 0) {
      uVar6 = 0xff;
    }
    else {
      uVar6 = 0;
    }
    uVar8 = uVar8 + 1;
    (&DAT_000b33eb)[iVar4] = uVar6;
    iVar4 = iVar4 + 0x28;
  } while ((int)uVar8 < 0x20);
  if ((DAT_000ae672 & 4) != 0) {
    DAT_000ae672 = DAT_000ae672 & 0xfb;
  }
  iVar4 = 0;
  do {
    iVar3 = iVar4 + 2;
    *(undefined2 *)((int)&DAT_000c3cf0 + iVar4) = 0;
    iVar4 = iVar3;
  } while (iVar3 != 0x82);
  uVar8 = 0;
  do {
    bVar9 = uVar8 < 0x82;
    if (0x81 < (int)uVar8) {
      uVar5 = 1;
LAB_00016463:
      return CONCAT44(*(undefined4 *)(puVar7 + 8),uVar5);
    }
    pcVar1 = (code *)swi(0x31);
    sVar2 = (*pcVar1)();
    if (bVar9) {
      sVar2 = 0;
    }
    *(short *)((int)&DAT_000c3cf0 + uVar8) = sVar2;
    if (sVar2 == 0) {
      uVar5 = 0;
      puVar7 = puVar7 + 4;
      goto LAB_00016463;
    }
    pcVar1 = (code *)swi(0x31);
    (*pcVar1)();
    puVar7 = puVar7 + 8;
    uVar8 = uVar8 + 2;
  } while( true );
}

