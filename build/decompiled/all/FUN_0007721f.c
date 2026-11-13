
uint __regparm3 FUN_0007721f(undefined4 *param_1,uint param_2,int *param_3)

{
  byte bVar1;
  uint uVar2;
  bool bVar3;
  uint uVar4;
  int extraout_ECX;
  uint uVar5;
  uint extraout_EDX;
  int unaff_EBX;
  undefined4 *puVar6;
  undefined4 *puVar7;
  byte bVar8;
  undefined8 uVar9;
  uint local_14;
  uint local_10;
  
  bVar8 = 0;
  if ((*(byte *)(param_3 + 3) & 2) == 0) {
    FUN_00078c12();
    *(byte *)(extraout_ECX + 0xc) = *(byte *)(extraout_ECX + 0xc) | 0x20;
    local_14 = 0;
  }
  else {
    local_10 = unaff_EBX * param_2;
    if (local_10 == 0) {
      local_14 = 0;
    }
    else {
      if (param_3[2] == 0) {
        FUN_0007a448();
      }
      uVar2 = param_3[3];
      bVar1 = *(byte *)(param_3 + 3);
      local_14 = 0;
      *(byte *)(param_3 + 3) = bVar1 & 0xcf;
      if ((bVar1 & 0x40) == 0) {
        bVar3 = false;
        if ((*(byte *)((int)param_3 + 0xd) & 4) != 0) {
          bVar3 = true;
          *(byte *)((int)param_3 + 0xd) = *(byte *)((int)param_3 + 0xd) & 0xfa | 1;
        }
        do {
          param_1 = (undefined4 *)((int)param_1 + 1);
          FUN_0007b9b0(param_2,param_1);
          if ((*(byte *)(param_3 + 3) & 0x30) != 0) break;
          local_14 = local_14 + 1;
        } while (local_10 != local_14);
        if (bVar3) {
          *(byte *)((int)param_3 + 0xd) = *(byte *)((int)param_3 + 0xd) & 0xfa | 4;
          FUN_0007a4e4();
        }
      }
      else {
        do {
          if ((param_3[1] == 0) && ((uint)param_3[5] <= local_10)) {
            uVar4 = FUN_0007b930();
            if (uVar4 != 0xffffffff) {
              if (uVar4 != 0) goto LAB_0007732e;
              uVar9 = FUN_0007b9a4();
              uVar4 = (uint)((ulonglong)uVar9 >> 0x20);
              *(undefined4 *)uVar9 = 0xc;
            }
            *(byte *)(param_3 + 3) = *(byte *)(param_3 + 3) | 0x20;
          }
          else {
            uVar4 = param_3[5] - param_3[1];
            if (local_10 < (uint)(param_3[5] - param_3[1])) {
              uVar4 = local_10;
            }
            puVar6 = param_1;
            puVar7 = (undefined4 *)*param_3;
            for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
              *puVar7 = *puVar6;
              puVar6 = puVar6 + (uint)bVar8 * -2 + 1;
              puVar7 = puVar7 + (uint)bVar8 * -2 + 1;
            }
            for (uVar5 = uVar4 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
              *(undefined1 *)puVar7 = *(undefined1 *)puVar6;
              puVar6 = (undefined4 *)((int)puVar6 + (uint)bVar8 * -2 + 1);
              puVar7 = (undefined4 *)((int)puVar7 + (uint)bVar8 * -2 + 1);
            }
            param_3[1] = param_3[1] + uVar4;
            *(byte *)((int)param_3 + 0xd) = *(byte *)((int)param_3 + 0xd) | 0x10;
            *param_3 = *param_3 + uVar4;
            if ((param_3[1] == param_3[5]) || ((*(byte *)((int)param_3 + 0xd) & 4) != 0)) {
              FUN_0007a4e4();
              uVar4 = extraout_EDX;
            }
          }
LAB_0007732e:
          param_1 = (undefined4 *)((int)param_1 + uVar4);
          local_14 = local_14 + uVar4;
          local_10 = local_10 - uVar4;
        } while ((local_10 != 0) && ((*(byte *)(param_3 + 3) & 0x20) == 0));
      }
      if ((*(byte *)(param_3 + 3) & 0x20) != 0) {
        local_14 = 0;
      }
      param_3[3] = param_3[3] | uVar2 & 0x30;
      local_14 = local_14 / param_2;
    }
  }
  return local_14;
}

