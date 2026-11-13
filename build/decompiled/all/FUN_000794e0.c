
uint * __regparm3 FUN_000794e0(undefined4 *param_1,uint param_2)

{
  uint *puVar1;
  int iVar2;
  uint *puVar3;
  uint *extraout_ECX;
  uint uVar4;
  int unaff_EBX;
  uint *puVar5;
  char *pcVar6;
  uint uVar7;
  undefined4 *puVar8;
  byte bVar9;
  
  bVar9 = 0;
  if (DAT_000a9fb4 == (undefined4 *)0x0) {
    DAT_000a9fb8 = 0;
    DAT_000a9fbc = DAT_000a9fb0 + -8;
    DAT_000a9fb4 = &DAT_000a9fb8;
  }
  pcVar6 = *(char **)(param_2 + 4);
  if (*pcVar6 == '\x02') {
    pcVar6 = *(char **)(pcVar6 + 1);
  }
  iVar2 = FUN_0007b7e8();
  uVar7 = iVar2 + 0x22;
  if (uVar7 < 8) {
    uVar7 = 8;
  }
  uVar7 = uVar7 + 3 & 0xfffffffc;
  puVar3 = (uint *)&DAT_000a9fb4;
  while( true ) {
    puVar5 = (uint *)*puVar3;
    if (puVar5 == (uint *)0x0) {
      FUN_00077bdc();
      puVar3 = extraout_ECX;
    }
    while (puVar1 = (uint *)*puVar5, (uint *)(puVar5[1] + (int)puVar5) == puVar1) {
      puVar5[1] = puVar5[1] + puVar1[1];
      *puVar5 = *puVar1;
    }
    if (uVar7 == puVar5[1]) {
      *puVar3 = (uint)puVar1;
      goto LAB_0007958b;
    }
    if (uVar7 + 8 < puVar5[1]) break;
    puVar3 = puVar5;
  }
  puVar5[1] = puVar5[1] - uVar7;
  puVar5 = (uint *)(puVar5[1] + (int)puVar5);
LAB_0007958b:
  *puVar5 = uVar7;
  puVar5[5] = (uint)&DAT_000a9fb0;
  puVar3 = puVar5 + 1;
  *puVar3 = *(uint *)(unaff_EBX + 8);
  *(uint **)(unaff_EBX + 8) = puVar3;
  uVar7 = *(uint *)(param_2 + 4);
  *(undefined1 *)(puVar5 + 8) = 5;
  puVar5[3] = uVar7;
  *(byte *)((int)puVar5 + 0x21) = *(byte *)((int)puVar5 + 0x21) & 0xfe;
  puVar5[2] = param_2;
  switch(*pcVar6) {
  case '\0':
    break;
  case '\x01':
  case '\x06':
  case '\a':
    uVar7 = 4;
    goto LAB_000795dc;
  default:
    goto switchD_000795c9_caseD_2;
  case '\x03':
  case '\b':
    uVar7 = (uint)(byte)pcVar6[1];
LAB_000795dc:
    puVar8 = (undefined4 *)((int)puVar5 + 0x22);
    for (uVar4 = uVar7 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar8 = *param_1;
      param_1 = param_1 + (uint)bVar9 * -2 + 1;
      puVar8 = puVar8 + (uint)bVar9 * -2 + 1;
    }
    for (uVar7 = uVar7 & 0xffffff03; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined1 *)puVar8 = *(undefined1 *)param_1;
      param_1 = (undefined4 *)((int)param_1 + (uint)bVar9 * -2 + 1);
      puVar8 = (undefined4 *)((int)puVar8 + (uint)bVar9 * -2 + 1);
    }
    return puVar3;
  case '\x04':
  }
  (**(code **)(pcVar6 + 5))();
switchD_000795c9_caseD_2:
  return puVar3;
}

