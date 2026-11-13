
uint __regparm3 FUN_00047e40(byte *param_1,byte *param_2)

{
  byte *pbVar1;
  undefined4 *puVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  int iVar3;
  uint extraout_ECX_01;
  uint uVar4;
  int iVar5;
  byte *pbVar6;
  undefined4 *puVar7;
  bool bVar8;
  bool bVar9;
  byte bVar10;
  byte abStack_7c [12];
  byte local_70;
  undefined4 local_6c [3];
  undefined1 local_60;
  byte local_5c [16];
  undefined4 local_4c [3];
  undefined1 local_40;
  byte local_3c [12];
  byte local_30;
  byte local_2c [12];
  byte local_20;
  byte *local_1c;
  undefined4 *local_18;
  undefined4 *local_14;
  
  bVar10 = 0;
  switch(param_1[0xc]) {
  case 0:
    local_70 = param_2[0xc];
    pbVar1 = abStack_7c;
    for (iVar3 = 3; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined4 *)pbVar1 = *(undefined4 *)param_2;
      param_2 = param_2 + 4;
      pbVar1 = pbVar1 + 4;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pbVar1 = *param_2;
      param_2 = param_2 + 1;
      pbVar1 = pbVar1 + 1;
    }
    local_1c = local_5c;
    pbVar1 = (byte *)FUN_00049550();
    pbVar6 = local_5c;
    for (uVar4 = extraout_ECX >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined4 *)pbVar6 = *(undefined4 *)pbVar1;
      pbVar1 = pbVar1 + ((uint)bVar10 * -2 + 1) * 4;
      pbVar6 = pbVar6 + ((uint)bVar10 * -2 + 1) * 4;
    }
    for (uVar4 = extraout_ECX & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pbVar6 = *pbVar1;
      pbVar1 = pbVar1 + (uint)bVar10 * -2 + 1;
      pbVar6 = pbVar6 + (uint)bVar10 * -2 + 1;
    }
    iVar5 = 0xc;
    bVar8 = false;
    iVar3 = 0;
    bVar9 = true;
    break;
  case 1:
    local_30 = param_2[0xc];
    pbVar1 = local_3c;
    for (iVar3 = 3; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined4 *)pbVar1 = *(undefined4 *)param_2;
      param_2 = param_2 + 4;
      pbVar1 = pbVar1 + 4;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pbVar1 = *param_2;
      param_2 = param_2 + 1;
      pbVar1 = pbVar1 + 1;
    }
    local_18 = local_6c;
    puVar2 = (undefined4 *)FUN_00049550();
    local_60 = *(undefined1 *)(puVar2 + 3);
    puVar7 = local_6c;
    for (uVar4 = extraout_ECX_00 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar7 = *puVar2;
      puVar2 = puVar2 + (uint)bVar10 * -2 + 1;
      puVar7 = puVar7 + (uint)bVar10 * -2 + 1;
    }
    for (uVar4 = extraout_ECX_00 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined1 *)puVar7 = *(undefined1 *)puVar2;
      puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar10 * -2 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + (uint)bVar10 * -2 + 1);
    }
    uVar4 = FUN_000495b0();
    return uVar4;
  case 2:
    iVar5 = 0xc;
    bVar8 = false;
    iVar3 = 0;
    bVar9 = true;
    do {
      if (iVar5 == 0) break;
      iVar5 = iVar5 + -1;
      bVar8 = *param_1 < *param_2;
      bVar9 = *param_1 == *param_2;
      param_1 = param_1 + 1;
      param_2 = param_2 + 1;
    } while (bVar9);
    if (!bVar9) {
      iVar3 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
    }
    return (uint)(iVar3 == 0);
  case 3:
    local_20 = param_2[0xc];
    pbVar1 = local_2c;
    for (iVar3 = 3; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined4 *)pbVar1 = *(undefined4 *)param_2;
      param_2 = param_2 + 4;
      pbVar1 = pbVar1 + 4;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pbVar1 = *param_2;
      param_2 = param_2 + 1;
      pbVar1 = pbVar1 + 1;
    }
    local_14 = local_4c;
    puVar2 = (undefined4 *)FUN_00049550();
    local_40 = *(undefined1 *)(puVar2 + 3);
    puVar7 = local_4c;
    for (uVar4 = extraout_ECX_01 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar7 = *puVar2;
      puVar2 = puVar2 + (uint)bVar10 * -2 + 1;
      puVar7 = puVar7 + (uint)bVar10 * -2 + 1;
    }
    for (uVar4 = extraout_ECX_01 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined1 *)puVar7 = *(undefined1 *)puVar2;
      puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar10 * -2 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + (uint)bVar10 * -2 + 1);
    }
    iVar3 = FUN_000495b0();
    return (uint)(iVar3 == 0);
  default:
    ll_printf(&DAT_000a92d4,s_TEventSet__match__Bledna_operacj_000919ca);
    ll_rethrow_report();
    return 0;
  }
  do {
    if (iVar5 == 0) break;
    iVar5 = iVar5 + -1;
    bVar8 = *local_1c < *param_1;
    bVar9 = *local_1c == *param_1;
    local_1c = local_1c + (uint)bVar10 * -2 + 1;
    param_1 = param_1 + (uint)bVar10 * -2 + 1;
  } while (bVar9);
  if (!bVar9) {
    iVar3 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
  }
  return (uint)(iVar3 == 0);
}

