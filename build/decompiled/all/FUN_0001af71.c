
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0001af71(undefined1 param_1)

{
  code *pcVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  
  _DAT_000a894c = DAT_000c41c8 >> 2;
  DAT_000a896c = 0;
  DAT_000a8974 = 0;
  DAT_000a8964 = 0;
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  puVar4 = &param_1;
  do {
    DAT_000a8954 = DAT_000a8958 * DAT_000a896c + _DAT_000c41cc;
    if (DAT_000a8974 != DAT_000a8954 >> 0x10) {
      pcVar1 = (code *)swi(0x10);
      DAT_000a8974 = DAT_000a8954 >> 0x10;
      (*pcVar1)();
      puVar4 = puVar4 + 4;
    }
    iVar2 = DAT_000cde24 * DAT_000cde3c;
    uVar3 = DAT_000a8954 & 0xffff;
    if ((int)(uVar3 + iVar2 * 4) < 0x10000) {
      DAT_000a893c = uVar3 + DAT_000c41bc;
    }
    else {
      DAT_000a893c = DAT_000a8958 * DAT_000a8974 + DAT_000c41c0;
      DAT_000a8964 = uVar3;
      DAT_000a8968 = DAT_000a893c;
    }
    DAT_000a8940 = DAT_000a893c + iVar2;
    DAT_000a8944 = DAT_000a8940 + iVar2;
    DAT_000a8948 = DAT_000a8944 + iVar2;
    DAT_000a8970 = 1;
    *(undefined4 *)(puVar4 + -4) = 0x1b07f;
    FUN_0001adb8();
    if (DAT_000a8964 != 0) {
      *(undefined4 *)(puVar4 + -4) = unaff_ESI;
      *(undefined4 *)(puVar4 + -8) = unaff_EDI;
      DAT_000a8964 = 0;
      *(undefined4 *)(puVar4 + -0xc) = 0x1b0af;
      FUN_0001aeda();
      *(undefined4 *)(puVar4 + -0xc) = 0x1b0b4;
      FUN_0001aeda();
      *(undefined4 *)(puVar4 + -0xc) = 0x1b0b9;
      FUN_0001aeda();
      *(undefined4 *)(puVar4 + -0xc) = 0x1b0be;
      FUN_0001aeda();
      unaff_EDI = *(undefined4 *)(puVar4 + -8);
      unaff_ESI = *(undefined4 *)(puVar4 + -4);
    }
    DAT_000a896c = DAT_000a896c + DAT_000a8970;
    _DAT_000a894c = _DAT_000a894c - DAT_000a8970;
  } while (_DAT_000a894c != 0);
  return;
}

