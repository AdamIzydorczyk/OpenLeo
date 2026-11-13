
uint __regparm3 FUN_0007612f(char *param_1,uint param_2,int *param_3)

{
  char cVar1;
  int iVar2;
  int extraout_ECX;
  uint uVar3;
  uint uVar4;
  int unaff_EBX;
  char *pcVar5;
  char *pcVar6;
  byte bVar7;
  undefined8 uVar8;
  char *local_18;
  uint local_14;
  uint local_10;
  
  bVar7 = 0;
  if ((*(byte *)(param_3 + 3) & 1) == 0) {
    FUN_00078c12();
    *(byte *)(extraout_ECX + 0xc) = *(byte *)(extraout_ECX + 0xc) | 0x20;
    return 0;
  }
  local_10 = unaff_EBX * param_2;
  if (local_10 == 0) {
    return 0;
  }
  if (param_3[2] == 0) {
    FUN_0007a448();
  }
  local_14 = 0;
  if ((*(byte *)(param_3 + 3) & 0x40) == 0) {
    pcVar5 = param_1 + local_10;
    do {
      if (param_3[1] == 0) {
        uVar8 = FUN_000770f1();
        param_1 = (char *)((ulonglong)uVar8 >> 0x20);
        if ((int)uVar8 == 0) break;
      }
      pcVar6 = (char *)*param_3;
      param_3[1] = param_3[1] + -1;
      *param_3 = (int)(pcVar6 + 1);
      cVar1 = *pcVar6;
      if (cVar1 == '\r') {
        if (param_3[1] == 0) {
          uVar8 = FUN_000770f1();
          param_1 = (char *)((ulonglong)uVar8 >> 0x20);
          if ((int)uVar8 == 0) break;
        }
        pcVar6 = (char *)*param_3;
        param_3[1] = param_3[1] + -1;
        *param_3 = (int)(pcVar6 + 1);
        cVar1 = *pcVar6;
      }
      if (cVar1 == '\x1a') goto LAB_00076244;
      *param_1 = cVar1;
      local_14 = local_14 + 1;
      param_1 = param_1 + 1;
    } while (param_1 != pcVar5);
  }
  else {
    local_18 = param_1;
    do {
      while( true ) {
        uVar4 = param_3[1];
        if (uVar4 != 0) {
          if (local_10 < uVar4) {
            uVar4 = local_10;
          }
          pcVar5 = (char *)*param_3;
          pcVar6 = local_18;
          for (uVar3 = uVar4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
            *(undefined4 *)pcVar6 = *(undefined4 *)pcVar5;
            pcVar5 = pcVar5 + (uint)bVar7 * -8 + 4;
            pcVar6 = pcVar6 + (uint)bVar7 * -8 + 4;
          }
          for (uVar3 = uVar4 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
            *pcVar6 = *pcVar5;
            pcVar5 = pcVar5 + (uint)bVar7 * -2 + 1;
            pcVar6 = pcVar6 + (uint)bVar7 * -2 + 1;
          }
          local_18 = local_18 + uVar4;
          local_14 = local_14 + uVar4;
          local_10 = local_10 - uVar4;
          *param_3 = *param_3 + uVar4;
          param_3[1] = param_3[1] - uVar4;
        }
        if (local_10 == 0) goto LAB_000762f8;
        if ((local_10 < (uint)param_3[5]) && ((*(byte *)((int)param_3 + 0xd) & 4) == 0)) break;
        param_3[1] = 0;
        *param_3 = param_3[2];
        iVar2 = FUN_0007a4bf();
        if (iVar2 == -1) {
          *(byte *)(param_3 + 3) = *(byte *)(param_3 + 3) | 0x20;
          goto LAB_000762f8;
        }
        if (iVar2 == 0) goto LAB_00076244;
        local_18 = local_18 + iVar2;
        local_10 = local_10 - iVar2;
        local_14 = local_14 + iVar2;
      }
      iVar2 = FUN_000770f1();
    } while (iVar2 != 0);
  }
LAB_000762f8:
  return local_14 / param_2;
LAB_00076244:
  *(byte *)(param_3 + 3) = *(byte *)(param_3 + 3) | 0x10;
  goto LAB_000762f8;
}

