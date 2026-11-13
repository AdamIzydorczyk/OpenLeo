
void __regparm3 FUN_0001274c(byte *param_1,uint param_2)

{
  undefined2 *puVar1;
  undefined4 *puVar2;
  byte *pbVar3;
  short *psVar4;
  int iVar5;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint uVar6;
  char *pcVar7;
  int iVar8;
  byte *pbVar9;
  uint local_20;
  byte local_1c;
  int local_18;
  
  pbVar9 = param_1;
  local_20 = param_2;
  if ((DAT_000ae680 & 1) != 0) {
    local_20 = param_2 >> 1;
  }
  do {
    while( true ) {
      if (local_20 == 0) {
        return;
      }
      if ((DAT_000ae67e & 1) == 0) break;
      FUN_00016a9c();
      uVar6 = extraout_ECX;
      if ((DAT_000ae67e & 8) != 0) {
        FUN_0007519c();
        uVar6 = extraout_ECX_00;
      }
LAB_00012884:
      if ((DAT_000ae67e & 4) != 0) {
        iVar5 = 0;
        for (iVar8 = 0; iVar8 < (int)(uVar6 & 0xffff); iVar8 = iVar8 + 1) {
          *(short *)(&DAT_000ae682 + iVar5) = *(short *)(&DAT_000ae682 + iVar5) + DAT_000ae67c;
          DAT_000ae67c = *(short *)(&DAT_000ae682 + iVar5);
          iVar5 = iVar5 + 2;
        }
      }
      if (((DAT_000ae680 ^ DAT_000ae67e) & 2) != 0) {
        iVar5 = 0;
        for (iVar8 = 0; iVar8 < (int)(uVar6 & 0xffff); iVar8 = iVar8 + 1) {
          (&DAT_000ae683)[iVar5] = (&DAT_000ae683)[iVar5] ^ 0x80;
          iVar5 = iVar5 + 2;
        }
      }
      if ((DAT_000ae680 & 1) == 0) {
        iVar5 = 0;
        for (iVar8 = 0; iVar8 < (int)(uVar6 & 0xffff); iVar8 = iVar8 + 1) {
          puVar2 = (undefined4 *)((int)&DAT_000ae680 + iVar5);
          iVar5 = iVar5 + 2;
          local_1c = (byte)((uint)*puVar2 >> 0x18);
          *param_1 = local_1c;
          param_1 = param_1 + 1;
        }
      }
      else {
        iVar5 = 0;
        for (iVar8 = 0; iVar8 < (int)(uVar6 & 0xffff); iVar8 = iVar8 + 1) {
          puVar1 = (undefined2 *)(&DAT_000ae682 + iVar5);
          iVar5 = iVar5 + 2;
          *(undefined2 *)pbVar9 = *puVar1;
          pbVar9 = pbVar9 + 2;
        }
      }
      local_20 = local_20 - (uVar6 & 0xffff);
    }
    if (((DAT_000ae67e & 4) != 0) || ((DAT_000ae680 & 1) != 0)) {
      FUN_00016a9c();
      pcVar7 = &DAT_000ae682 + (extraout_ECX_02 & 0xffff);
      psVar4 = (short *)(&DAT_000ae682 + (extraout_ECX_02 & 0xffff) * 2);
      for (local_18 = 0; uVar6 = extraout_ECX_02, local_18 < (int)(extraout_ECX_02 & 0xffff);
          local_18 = local_18 + 1) {
        pcVar7 = pcVar7 + -1;
        psVar4 = psVar4 + -1;
        *psVar4 = (short)*pcVar7 << 8;
      }
      goto LAB_00012884;
    }
    FUN_00016a9c();
    if (((DAT_000ae680 ^ DAT_000ae67e) & 2) != 0) {
      pbVar3 = param_1;
      for (iVar5 = 0; iVar5 < (int)(extraout_ECX_01 & 0xffff); iVar5 = iVar5 + 1) {
        *pbVar3 = *pbVar3 ^ 0x80;
        pbVar3 = pbVar3 + 1;
      }
    }
    local_20 = local_20 - (extraout_ECX_01 & 0xffff);
    param_1 = param_1 + (extraout_ECX_01 & 0xffff);
  } while( true );
}

