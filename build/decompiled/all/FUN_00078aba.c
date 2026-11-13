
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00078aba(void)

{
  int iVar1;
  uint uVar2;
  undefined1 *puVar3;
  undefined8 uVar4;
  undefined1 auStack_88 [128];
  
  uVar4 = FUN_0007bfac();
  uVar2 = (uint)((ulonglong)uVar4 >> 0x20);
  puVar3 = auStack_88 + 0x80;
  if (uVar2 < (uint)uVar4) {
    iVar1 = -uVar2;
    puVar3 = auStack_88 + iVar1 + 0x80;
    _DAT_000a9bdc = auStack_88 + iVar1 + 0x80;
  }
  else {
    _DAT_000a9bdc = (undefined1 *)0x0;
  }
  _DAT_000a9bdc = _DAT_000a9bdc + DAT_000a9bd8;
  *(undefined4 *)(puVar3 + -4) = 0x78aef;
  FUN_0007bfb5();
  *(undefined4 *)(puVar3 + -4) = 0x78aff;
  ll_main();
  *(undefined4 *)(puVar3 + -4) = 0x78b04;
  FUN_000769ab();
  return;
}

