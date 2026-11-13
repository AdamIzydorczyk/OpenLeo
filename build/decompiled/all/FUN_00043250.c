
void __regparm3 FUN_00043250(int param_1,byte *param_2,int param_3)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  byte *pbVar5;
  int *unaff_EBX;
  uint uVar6;
  byte *pbVar7;
  byte bVar8;
  byte *pbVar9;
  byte *local_14;
  
  bVar8 = 0;
  pbVar9 = param_2 + param_3;
  iVar2 = *unaff_EBX;
  pbVar5 = (byte *)(iVar2 + 1);
  *unaff_EBX = (int)pbVar5;
  uVar6 = (uint)*pbVar5;
  *unaff_EBX = iVar2 + 2;
  local_14 = param_2;
  if (*(char *)(param_1 + 0x14) != '\0') {
LAB_0004327d:
    FUN_000749a0(pbVar9);
    local_14 = local_14 + uVar6;
    uVar6 = (uint)*(byte *)*unaff_EBX;
    *unaff_EBX = (int)((byte *)*unaff_EBX + 1);
    if (0x7f < uVar6) goto LAB_000432f6;
    do {
      pbVar5 = (byte *)*unaff_EBX;
      pbVar7 = local_14;
      for (uVar3 = uVar6 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined4 *)pbVar7 = *(undefined4 *)pbVar5;
        pbVar5 = pbVar5 + (uint)bVar8 * -8 + 4;
        pbVar7 = pbVar7 + (uint)bVar8 * -8 + 4;
      }
      for (uVar3 = uVar6 & 0xffffff03; uVar3 != 0; uVar3 = uVar3 - 1) {
        *pbVar7 = *pbVar5;
        pbVar5 = pbVar5 + (uint)bVar8 * -2 + 1;
        pbVar7 = pbVar7 + (uint)bVar8 * -2 + 1;
      }
      local_14 = local_14 + uVar6;
      pbVar5 = (byte *)(*unaff_EBX + uVar6);
      *unaff_EBX = (int)pbVar5;
      uVar6 = (uint)*pbVar5;
      *unaff_EBX = (int)(pbVar5 + 1);
      if (0x7f < uVar6) {
LAB_00043344:
        FUN_000749a0(pbVar9);
        *unaff_EBX = *unaff_EBX + -1;
        return;
      }
      if (uVar6 < 0x40) goto LAB_0004327d;
      uVar6 = uVar6 & 0x3f;
LAB_000432f6:
      *unaff_EBX = *unaff_EBX + 1;
      FUN_000749a0(pbVar9);
      local_14 = local_14 + (uVar6 & 0x7f);
      uVar6 = (uint)*(byte *)*unaff_EBX;
      *unaff_EBX = (int)((byte *)*unaff_EBX + 1);
      if (0x7f < uVar6) goto LAB_00043344;
      if (uVar6 < 0x40) goto LAB_0004327d;
      uVar6 = uVar6 & 0x3f;
    } while( true );
  }
  do {
    FUN_000749a0(pbVar9);
    pbVar5 = (byte *)*unaff_EBX + 1;
    bVar1 = *(byte *)*unaff_EBX;
    uVar3 = (uint)bVar1;
    *unaff_EBX = (int)pbVar5;
    if (0x7f < uVar3) break;
    pbVar7 = local_14 + uVar6;
    for (uVar4 = (uint)(bVar1 >> 2); uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined4 *)pbVar7 = *(undefined4 *)pbVar5;
      pbVar5 = pbVar5 + (uint)bVar8 * -8 + 4;
      pbVar7 = pbVar7 + (uint)bVar8 * -8 + 4;
    }
    for (uVar4 = bVar1 & 0xffffff03; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pbVar7 = *pbVar5;
      pbVar5 = pbVar5 + (uint)bVar8 * -2 + 1;
      pbVar7 = pbVar7 + (uint)bVar8 * -2 + 1;
    }
    local_14 = local_14 + uVar6 + uVar3;
    pbVar5 = (byte *)(*unaff_EBX + uVar3);
    *unaff_EBX = (int)pbVar5;
    uVar6 = (uint)*pbVar5;
    *unaff_EBX = (int)(pbVar5 + 1);
  } while (uVar6 < 0x80);
  *unaff_EBX = *unaff_EBX + -1;
  return;
}

