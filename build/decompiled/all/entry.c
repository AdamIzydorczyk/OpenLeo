
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 entry(undefined2 param_1,undefined1 param_2,undefined1 param_3)

{
  code *pcVar1;
  char cVar2;
  short sVar3;
  undefined4 uVar4;
  short extraout_CX;
  uint uVar5;
  int iVar6;
  short extraout_DX;
  char *pcVar7;
  undefined1 *puVar8;
  int iVar9;
  uint *puVar10;
  uint *puVar11;
  char *pcVar12;
  char *pcVar13;
  undefined4 *puVar14;
  undefined2 in_ES;
  undefined2 in_DS;
  short in_GS;
  bool bVar15;
  undefined8 uVar16;
  
  DAT_000a9bcc = 0x24;
  pcVar1 = (code *)swi(0x21);
  DAT_000a9bc0 = (undefined1 *)register0x00000010;
  DAT_000a9bd4 = (undefined1 *)register0x00000010;
  uVar16 = (*pcVar1)();
  iVar6 = (int)((ulonglong)uVar16 >> 0x20);
  DAT_000a9bf7 = (undefined1)uVar16;
  DAT_000a9bf8 = (undefined1)((ulonglong)uVar16 >> 8);
  iVar9 = 0;
  pcVar12 = &DAT_00000081;
  sVar3 = (short)((ulonglong)uVar16 >> 0x10);
  if (sVar3 == 0x4458) {
    DAT_000a9bc0 = (undefined1 *)(_DAT_0000005c + 0xfffU & 0xfffff000);
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    DAT_000a9bf5 = 0x2c;
  }
  else {
    if (sVar3 != 0x4243) {
      pcVar1 = (code *)swi(0x21);
      cVar2 = (*pcVar1)();
      puVar8 = &param_2;
      if (cVar2 == '\0') {
        DAT_000a9bf5 = 0x2c;
        param_1 = 0;
      }
      else {
        if (in_GS != 0) {
          DAT_000a9ac8 = in_GS;
        }
        pcVar1 = (code *)swi(0x31);
        (*pcVar1)();
        puVar8 = &param_3;
        param_1 = 1;
        if (extraout_DX != 0 || extraout_CX != 0) {
          param_1 = 0x101;
        }
        DAT_000a9bcc = in_ES;
        DAT_000a9bf5 = _DAT_0000002c;
      }
      goto LAB_00074e79;
    }
    pcVar1 = (code *)swi(0x21);
    DAT_000a9ac4 = iVar6;
    (*pcVar1)();
    iVar6 = *(int *)(iVar6 + 0x10);
    pcVar12 = (char *)(iVar6 + 0x81);
    iVar9 = (uint)*(ushort *)(iVar6 + 0x2c) << 4;
    param_1 = 9;
    DAT_000a9bcc = in_DS;
    DAT_000a9bf5 = in_DS;
  }
  puVar8 = &param_2;
LAB_00074e79:
  DAT_000a9bee = (char)param_1;
  DAT_000a9bef = (undefined1)((ushort)param_1 >> 8);
  *(int *)(puVar8 + -4) = iVar9;
  pcVar7 = &DAT_000cfed0;
  bVar15 = true;
  uVar5 = (uint)(byte)pcVar12[-1];
  do {
    pcVar13 = pcVar12;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar13 = pcVar12 + 1;
    bVar15 = *pcVar12 == ' ';
    pcVar12 = pcVar13;
  } while (bVar15);
  DAT_00074fc1 = in_DS;
  _DAT_000a9bf1 = iVar9;
  if (!bVar15) {
    pcVar12 = pcVar13 + -1;
    for (iVar6 = uVar5 + 1; iVar6 != 0; iVar6 = iVar6 + -1) {
      *pcVar7 = *pcVar12;
      pcVar12 = pcVar12 + 1;
      pcVar7 = pcVar7 + 1;
    }
  }
  *pcVar7 = '\0';
  pcVar7[1] = '\0';
  puVar11 = *(uint **)(puVar8 + -4);
  *(char **)(puVar8 + -4) = pcVar7 + 1;
  *(uint *)(puVar8 + -8) = CONCAT22(0xc,in_DS);
  _DAT_000a9bec = 0;
  do {
    if (((*puVar11 | 0x20202020) == 0x37386f6e) && ((char)puVar11[1] == '=')) {
      _DAT_000a9bec = _DAT_000a9bec + 1;
    }
    do {
      puVar10 = puVar11;
      puVar11 = (uint *)((int)puVar10 + 1);
    } while ((char)*puVar10 != '\0');
  } while (*(char *)puVar11 != '\0');
  puVar11 = puVar10 + 1;
  pcVar12 = pcVar7 + 1;
  do {
    uVar5 = *puVar11;
    DAT_000a9bd0 = pcVar12 + 1;
    *pcVar12 = (char)*puVar11;
    puVar11 = (uint *)((int)puVar11 + 1);
    pcVar12 = DAT_000a9bd0;
  } while ((char)uVar5 != '\0');
  _DAT_000a9bc8 = *(undefined4 *)(puVar8 + -4);
  uVar5 = 0x22464;
  if (DAT_000a9bee == '\x01') {
    uVar5 = 0x1000;
  }
  puVar14 = &DAT_000ada6c;
  _DAT_000a9bbc = puVar8;
  for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
    *puVar14 = 0;
    puVar14 = puVar14 + 1;
  }
  for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
    *(undefined1 *)puVar14 = 0;
    puVar14 = (undefined4 *)((int)puVar14 + 1);
  }
  _DAT_000a9bc4 = &DAT_000cfed0;
  *(undefined4 *)(puVar8 + -4) = 0x74f74;
  FUN_00078b0a();
  *(undefined4 *)(puVar8 + -4) = 0x74f7b;
  uVar4 = FUN_00078aba();
  *(undefined4 *)(puVar8 + -4) = uVar4;
  *(undefined4 *)(puVar8 + -8) = 0x74fb0;
  FUN_00078b55();
  pcVar1 = (code *)swi(0x21);
  uVar4 = (*pcVar1)();
  return uVar4;
}

