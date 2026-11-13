
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_00010010(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int extraout_ECX;
  undefined2 *extraout_ECX_00;
  int extraout_ECX_01;
  undefined4 *puVar5;
  byte *pbVar6;
  byte *pbVar7;
  bool bVar8;
  bool bVar9;
  byte bVar10;
  undefined4 uStack_1c;
  
  bVar10 = 0;
  uStack_1c = 0x10021;
  FUN_000169b4();
  uStack_1c = 0x10032;
  FUN_00016a9c();
  uStack_1c = 0x1003e;
  _DAT_000ada70 = FUN_00016a78();
  uStack_1c = 0x1004f;
  FUN_00016a9c();
  uStack_1c = 0x10060;
  FUN_00016a9c();
  uStack_1c = 0x10067;
  DAT_000ada7c = FUN_00016a78();
  uStack_1c = 0x10073;
  _DAT_000ada80 = FUN_00016a54();
  uStack_1c = 0x10080;
  DAT_000ada82 = FUN_00016a54();
  uStack_1c = 0x1008d;
  _DAT_000ada84 = FUN_00016a78();
  uStack_1c = 0x10099;
  _DAT_000ada88 = FUN_00016a78();
  uStack_1c = 0x100a5;
  DAT_000ada8c = FUN_00016a54();
  uStack_1c = 0x100b2;
  _DAT_000ada8e = FUN_00016a54();
  if (*(int **)(param_1 + 1) == (int *)0x0) {
LAB_000100d3:
    bVar8 = true;
  }
  else {
    uStack_1c = 0x100c6;
    iVar2 = (**(code **)(**(int **)(param_1 + 1) + 0x18))();
    bVar8 = false;
    if (iVar2 == -1) goto LAB_000100d3;
  }
  if (!bVar8) {
    iVar4 = 4;
    bVar8 = false;
    iVar2 = 0;
    bVar9 = true;
    pbVar6 = (byte *)&DAT_000ada6c;
    pbVar7 = &DAT_00090004;
    do {
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      bVar8 = *pbVar6 < *pbVar7;
      bVar9 = *pbVar6 == *pbVar7;
      pbVar6 = pbVar6 + (uint)bVar10 * -2 + 1;
      pbVar7 = pbVar7 + (uint)bVar10 * -2 + 1;
    } while (bVar9);
    if (!bVar9) {
      iVar2 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
    }
    if (iVar2 == 0) {
      iVar4 = 4;
      bVar8 = false;
      iVar2 = 0;
      bVar9 = true;
      pbVar6 = &DAT_000ada74;
      pbVar7 = &DAT_00090009;
      do {
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        bVar8 = *pbVar6 < *pbVar7;
        bVar9 = *pbVar6 == *pbVar7;
        pbVar6 = pbVar6 + (uint)bVar10 * -2 + 1;
        pbVar7 = pbVar7 + (uint)bVar10 * -2 + 1;
      } while (bVar9);
      if (!bVar9) {
        iVar2 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
      }
      if (iVar2 == 0) {
        iVar4 = 4;
        bVar8 = false;
        iVar2 = 0;
        bVar9 = true;
        pbVar6 = &DAT_000ada78;
        pbVar7 = &DAT_0009000e;
        do {
          if (iVar4 == 0) break;
          iVar4 = iVar4 + -1;
          bVar8 = *pbVar6 < *pbVar7;
          bVar9 = *pbVar6 == *pbVar7;
          pbVar6 = pbVar6 + (uint)bVar10 * -2 + 1;
          pbVar7 = pbVar7 + (uint)bVar10 * -2 + 1;
        } while (bVar9);
        if (!bVar9) {
          iVar2 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
        }
        if (iVar2 == 0) {
          uStack_1c = 0x10167;
          FUN_000169b4();
          uStack_1c = 0x1017d;
          FUN_00016a9c();
          puVar5 = (undefined4 *)((int)register0x00000010 + -0x18);
          bVar8 = false;
          iVar4 = 0;
          bVar9 = true;
          iVar2 = extraout_ECX;
          pbVar6 = &DAT_000ada90;
          pbVar7 = &DAT_00090022;
          do {
            if (iVar2 == 0) break;
            iVar2 = iVar2 + -1;
            bVar8 = *pbVar6 < *pbVar7;
            bVar9 = *pbVar6 == *pbVar7;
            pbVar6 = pbVar6 + (uint)bVar10 * -2 + 1;
            pbVar7 = pbVar7 + (uint)bVar10 * -2 + 1;
          } while (bVar9);
          if (!bVar9) {
            iVar4 = (1 - (uint)bVar8) - (uint)(bVar8 != 0);
          }
          if (iVar4 == 0) {
            if (DAT_000ada82 < 2) {
              uStack_1c = 65999;
              iVar2 = ll_alloc_try();
              uStack_1c = 0x101da;
              FUN_000749a0();
              *extraout_ECX_00 = 0x2000;
              *(undefined1 *)((int)extraout_ECX_00 + 3) = 0x40;
              uStack_1c = 0x101ea;
              uVar3 = FUN_00016a78();
              *(undefined4 *)(extraout_ECX_01 + 5) = uVar3;
              if (DAT_000ada8c == 2) {
                *(undefined2 *)(extraout_ECX_01 + 0x11) = 3;
                *(uint *)(extraout_ECX_01 + 5) = *(uint *)(extraout_ECX_01 + 5) >> 1;
              }
              uStack_1c = (uint)*(ushort *)(iVar2 + 0x11);
              uVar1 = FUN_00012980();
              puVar5 = &uStack_1c;
              *(undefined2 *)(iVar2 + 0x1b) = uVar1;
              if (*(int *)(iVar2 + 0x19) >> 0x10 < 0) {
                DAT_000cf2cc = DAT_000cf2cc + 1;
                *(int *)(&DAT_000cf2d0 + DAT_000cf2cc * 4) = iVar2;
                FUN_0005c370();
                puVar5 = &uStack_1c;
                iVar2 = 0;
              }
            }
            else {
              iVar2 = 0;
              _DAT_000c3fb0 = s_Only_mono_WAV_s_are_supported_00090036;
            }
          }
          else {
            iVar2 = 0;
            _DAT_000c3fb0 = s_Not_a_WAV_file_00090027;
            puVar5 = (undefined4 *)((int)register0x00000010 + -0x18);
          }
          goto LAB_0001024c;
        }
      }
    }
  }
  iVar2 = 0;
  _DAT_000c3fb0 = s_Not_a_WAV_file_00090013;
  puVar5 = (undefined4 *)((int)register0x00000010 + -0x18);
LAB_0001024c:
  return CONCAT44(*(undefined4 *)((int)puVar5 + 0xc),iVar2);
}

