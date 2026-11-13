
undefined8 __regparm3 FUN_0007a2b5(undefined4 param_1,uint param_2,undefined4 param_3)

{
  code *pcVar1;
  bool bVar2;
  uint uVar3;
  undefined4 uVar4;
  int extraout_EDX;
  uint *puVar5;
  uint *puVar6;
  byte bVar7;
  undefined1 auStack_14 [4];
  uint local_c;
  undefined4 uStack_8;
  
  local_c = param_2;
  uStack_8 = param_3;
  uVar3 = FUN_0007f975();
  FUN_0007f9ca();
  puVar6 = (uint *)auStack_14;
  bVar7 = 0;
  if ((uVar3 >> 8 & 0x20) == 0) {
LAB_0007a310:
    uVar4 = 0;
  }
  else {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    puVar5 = (uint *)(auStack_14 + 4);
    if ((bVar7 & 1) == 0) {
      bVar7 = 0;
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      puVar6 = &local_c;
      puVar5 = &local_c;
      bVar2 = (bVar7 & 1) != 0;
      local_c = (extraout_EDX << 1 | (uint)bVar7) >> 1 | (uint)bVar2 << 0x1f;
      if (!bVar2) goto LAB_0007a310;
    }
    *(undefined4 *)((int)puVar5 + -4) = 0x7a2f3;
    uVar4 = FUN_0007ba5f();
    puVar6 = puVar5;
  }
  return CONCAT44(*(undefined4 *)((int)puVar6 + 8),uVar4);
}

