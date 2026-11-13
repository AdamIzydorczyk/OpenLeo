
undefined8 FUN_0007ffab(void)

{
  code *pcVar1;
  short sVar2;
  undefined2 uVar3;
  undefined1 extraout_CL;
  undefined2 extraout_DX;
  undefined2 unaff_BX;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 in_DS;
  undefined1 auStack_18 [4];
  undefined1 local_14 [4];
  undefined4 auStack_c [2];
  
  puVar4 = auStack_18;
  puVar5 = auStack_18;
  if (DAT_000ad5d0 != 0) goto LAB_00080032;
  if (DAT_000a9bee == '\x01') {
LAB_0007ffd6:
    *puVar4 = 0;
    puVar4[1] = 0;
    *(undefined2 *)(puVar4 + 2) = 0;
    puVar4[4] = 0;
    *(undefined2 *)(puVar4 + -4) = in_DS;
    pcVar1 = (code *)swi(0x31);
    uVar3 = (*pcVar1)();
    *puVar4 = (char)((ushort)uVar3 >> 8);
    puVar4[1] = (char)uVar3;
    *(undefined2 *)(puVar4 + 2) = unaff_BX;
    puVar4[4] = extraout_CL;
    puVar4[5] = (char)((ushort)extraout_DX >> 8);
    puVar4[6] = (char)extraout_DX;
    puVar5 = puVar4 + 4;
    if ((uint)(byte)puVar4[4] + (uint)(byte)puVar4[5] != 0) {
      DAT_000ad5d0 = 1;
      puVar5 = puVar4 + 4;
      goto LAB_00080032;
    }
  }
  else {
    pcVar1 = (code *)swi(0x2f);
    sVar2 = (*pcVar1)();
    puVar4 = local_14;
    puVar5 = local_14;
    if (sVar2 == 0) goto LAB_0007ffd6;
  }
  DAT_000ad5d0 = -1;
LAB_00080032:
  return CONCAT44(*(undefined4 *)(puVar5 + 0xc),DAT_000ad5d0);
}

