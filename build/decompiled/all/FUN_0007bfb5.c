
undefined8 FUN_0007bfb5(void)

{
  code *pcVar1;
  bool bVar2;
  int iVar3;
  undefined2 extraout_var;
  undefined1 *puVar4;
  byte bVar5;
  undefined1 auStack_94 [128];
  undefined1 auStack_a [6];
  
  puVar4 = auStack_94 + 0x80;
  DAT_000a9fac = 0x8000;
  if ((DAT_000a9bee < 2) || (8 < DAT_000a9bee)) {
    bVar2 = true;
  }
  else {
    bVar2 = false;
  }
  iVar3 = 0;
  if (bVar2) {
    iVar3 = FUN_0007ffab();
    puVar4 = auStack_94 + 0x80;
    if (iVar3 == 1) {
      bVar5 = 0;
      pcVar1 = (code *)swi(0x31);
      (*pcVar1)();
      puVar4 = auStack_94 + 0x84;
      iVar3 = CONCAT22(extraout_var,-(ushort)bVar5);
    }
  }
  return CONCAT44(*(undefined4 *)(puVar4 + 8),iVar3);
}

