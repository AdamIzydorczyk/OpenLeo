
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00016248(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iStack_1c;
  undefined *puStack_18;
  
  iVar1 = (int)(0x1000000 / (ulonglong)(longlong)(int)(uint)DAT_000ae674);
  puStack_18 = (undefined *)0x0;
  iStack_1c = 0;
  _DAT_000b38e4 = iVar1;
  do {
    iVar2 = (int)((iStack_1c + (iStack_1c >> 0x1f) * -0x40) - (uint)((iStack_1c >> 0x1f) << 5 < 0))
            >> 6;
    uVar3 = 0xffffff80;
    iVar4 = iVar2 * -0x80;
    do {
      uVar5 = uVar3 & 0xff;
      uVar3 = uVar3 + 1;
      *(int *)(&DAT_000b38f0 + (int)(puStack_18 + uVar5 * 4)) = iVar4;
      iVar4 = iVar4 + iVar2;
    } while ((int)uVar3 < 0x80);
    puStack_18 = puStack_18 + 0x400;
    iStack_1c = iStack_1c + iVar1;
  } while (puStack_18 != &DAT_00010400);
  _DAT_000b38ec = (int)(uint)DAT_000ae674 >> 3;
  if ((DAT_000ae672 & 1) != 0) {
    _DAT_000b38ec = _DAT_000b38ec + 1;
  }
  DAT_000b38d4 = 0xe10;
  if ((DAT_000ae672 & 1) != 0) {
    DAT_000b38d4 = 0x708;
  }
  DAT_000c3f74 = 0;
  return;
}

