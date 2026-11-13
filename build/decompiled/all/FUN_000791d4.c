
undefined8 __regparm3 FUN_000791d4(uint param_1)

{
  code *pcVar1;
  undefined4 uVar2;
  int iVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  byte bVar7;
  undefined8 uVar8;
  undefined4 local_18;
  
  local_18 = param_1;
  puVar6 = &local_18;
  if ((DAT_000a9fa8 != 0) && (puVar6 = &local_18, DAT_000a9bc0 != -2)) {
    iVar3 = FUN_000793d0();
    puVar6 = &local_18;
    uVar2 = 0;
    if (iVar3 == 0) goto LAB_000791cb;
    bVar7 = DAT_000a9bee == '\0';
    if (DAT_000a9bee == '\x01') {
      puVar4 = (uint *)FUN_00079103();
      puVar6 = &local_18;
      if (puVar4 != (uint *)0x0) {
        local_18 = *puVar4;
        puVar6 = &local_18;
      }
    }
    else {
      pcVar1 = (code *)swi(0x21);
      uVar5 = (*pcVar1)();
      puVar6 = (uint *)((int)register0x00000010 + -0x14);
      puVar4 = (uint *)(uVar5 & ~-(uint)bVar7);
    }
    if (((puVar4 != (uint *)0x0) && (uVar5 = *puVar6 - 4, uVar5 <= *puVar6)) &&
       (*puVar6 = uVar5, 0x37 < uVar5)) {
      *puVar4 = uVar5;
      puVar6[-1] = 0x79259;
      uVar8 = FUN_0007903f();
      iVar3 = (int)((ulonglong)uVar8 >> 0x20);
      uVar5 = *(uint *)uVar8;
      *puVar6 = uVar5;
      *(uint *)uVar8 = uVar5 | 1;
      *(undefined4 *)(iVar3 + 0x14) = 0xffffffff;
      *(int *)(iVar3 + 0x18) = *(int *)(iVar3 + 0x18) + 1;
      puVar6[-1] = 0x7927a;
      FUN_00075302();
      uVar2 = 1;
      goto LAB_000791cb;
    }
  }
  uVar2 = 0;
LAB_000791cb:
  return CONCAT44(*(undefined4 *)((int)puVar6 + 0xc),uVar2);
}

