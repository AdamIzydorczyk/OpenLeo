
/* WARNING: Instruction at (ram,0x00076f2c) overlaps instruction at (ram,0x00076f2a)
    */

undefined8 __regparm3 FUN_00076ea6(byte *param_1,byte *param_2)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  uint uVar4;
  byte *pbVar5;
  byte *pbVar6;
  byte *pbVar7;
  byte *pbVar8;
  byte *pbVar9;
  byte *pbVar10;
  ushort in_DS;
  bool bVar11;
  bool bVar12;
  
  pbVar7 = param_1;
  if (*param_2 != 0) {
    if (param_2[1] != 0) {
      pbVar3 = (byte *)0xffffffff;
      bVar11 = true;
      pbVar7 = (byte *)(uint)in_DS;
      pbVar5 = param_1;
      do {
        pbVar9 = pbVar5;
        if (pbVar3 == (byte *)0x0) break;
        pbVar3 = pbVar3 + -1;
        pbVar9 = pbVar5 + 1;
        bVar11 = *pbVar5 == 0;
        pbVar5 = pbVar9;
      } while (!bVar11);
      if (!bVar11) {
        pbVar9 = pbVar3;
      }
      uVar4 = 0xffffffff;
      pbVar5 = param_2;
      do {
        if (uVar4 == 0) break;
        uVar4 = uVar4 - 1;
        bVar1 = *pbVar5;
        pbVar5 = pbVar5 + 1;
      } while (bVar1 != 0);
      pbVar5 = (byte *)(~uVar4 - 1);
      do {
        pbVar6 = pbVar9 + (-1 - (int)param_1);
        bVar11 = pbVar6 == pbVar5;
        pbVar3 = pbVar7;
        if (pbVar6 < pbVar5) {
LAB_00076f59:
          pbVar7 = (byte *)0x0;
          break;
        }
        if (pbVar6 == (byte *)0x0) {
LAB_00076f2a_2:
          param_1 = pbVar6;
        }
        else {
          do {
            pbVar7 = param_1;
            if (pbVar6 == (byte *)0x0) break;
            pbVar6 = pbVar6 + -1;
            pbVar7 = param_1 + 1;
            bVar11 = *param_2 == *param_1;
            param_1 = pbVar7;
          } while (!bVar11);
          param_1 = pbVar7;
          if (!bVar11) goto LAB_00076f2a_2;
        }
        pbVar7 = param_1 + -1;
        pbVar3 = pbVar7;
        if (pbVar7 == (byte *)0x0) goto LAB_00076f59;
        bVar11 = false;
        iVar2 = 0;
        bVar12 = true;
        pbVar6 = pbVar5;
        pbVar8 = pbVar7;
        pbVar10 = param_2;
        do {
          if (pbVar6 == (byte *)0x0) break;
          pbVar6 = pbVar6 + -1;
          bVar11 = *pbVar8 < *pbVar10;
          bVar12 = *pbVar8 == *pbVar10;
          pbVar8 = pbVar8 + 1;
          pbVar10 = pbVar10 + 1;
        } while (bVar12);
        if (!bVar12) {
          iVar2 = (1 - (uint)bVar11) - (uint)(bVar11 != 0);
        }
      } while (iVar2 != 0);
      goto LAB_00076f5b;
    }
    bVar1 = *param_2;
    param_2 = (byte *)CONCAT31((int3)((uint)param_2 >> 8),bVar1);
    do {
      pbVar7 = param_1;
      if (*param_1 == bVar1) goto LAB_00076eb7;
      if (*param_1 == 0) break;
      pbVar7 = param_1 + 1;
      if (*pbVar7 == bVar1) goto LAB_00076eb7;
      param_1 = param_1 + 2;
    } while (*pbVar7 != 0);
    pbVar7 = (byte *)0x0;
  }
LAB_00076eb7:
  pbVar3 = param_2;
LAB_00076f5b:
  return CONCAT44(pbVar3,pbVar7);
}

