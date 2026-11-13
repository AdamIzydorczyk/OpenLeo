
int __regparm3 FUN_00077e98(int param_1,byte *param_2)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  int iVar4;
  char *pcVar5;
  uint uVar6;
  int local_14;
  
  local_14 = 0;
  *(byte *)(param_1 + 0x10) = *(byte *)(param_1 + 0x10) & 0xfd;
  do {
    pbVar3 = param_2 + 1;
    uVar6 = (uint)*param_2;
    if (uVar6 == 0) goto LAB_000780bf;
    if (((&DAT_000a68b4)[(byte)(*param_2 + 1)] & 2) == 0) {
      if (uVar6 == 0x25) {
        pbVar3 = (byte *)FUN_000780df();
        bVar1 = *pbVar3;
        if (bVar1 != 0) {
          pbVar3 = pbVar3 + 1;
        }
        if (bVar1 < 0x65) {
          if (bVar1 < 0x58) {
            if (bVar1 < 0x45) {
              if ((bVar1 == 0x25) && (iVar4 = FUN_00077e89(), iVar4 != 0x25)) goto LAB_00077f08;
            }
            else if ((bVar1 < 0x46) || (bVar1 == 0x47)) goto LAB_0007801c;
          }
          else {
            if (bVar1 < 0x59) goto LAB_00077fe3;
            if (bVar1 < 99) {
              if (bVar1 != 0x5b) goto LAB_0007807f;
              iVar4 = FUN_0007846c();
            }
            else {
              if (99 < bVar1) goto LAB_00077fe3;
              iVar4 = FUN_00078213();
            }
LAB_00078048:
            if (iVar4 < 1) goto LAB_000780bf;
            if ((*(byte *)(param_1 + 0x10) & 1) != 0) {
              local_14 = local_14 + 1;
            }
          }
        }
        else {
          if (bVar1 < 0x68) {
LAB_0007801c:
            iVar4 = FUN_00078566();
            goto LAB_00078048;
          }
          if (bVar1 < 0x70) {
            if (bVar1 < 0x6e) {
              if (bVar1 == 0x69) goto LAB_00077fe3;
            }
            else {
              if (0x6e < bVar1) {
LAB_00077fe3:
                iVar4 = FUN_0007887d();
                goto LAB_00078048;
              }
              FUN_000783cb();
            }
          }
          else {
            if (bVar1 < 0x71) goto LAB_00077fe3;
            if (bVar1 < 0x75) {
              if (bVar1 == 0x73) {
                iVar4 = FUN_000782a8();
                goto LAB_00078048;
              }
            }
            else if ((bVar1 < 0x76) || (bVar1 == 0x78)) goto LAB_00077fe3;
          }
        }
      }
      else {
        uVar2 = FUN_00077e89();
        if (uVar2 != uVar6) {
LAB_00077f08:
          if ((*(byte *)(param_1 + 0x10) & 2) == 0) {
            FUN_00077e90();
          }
          goto LAB_000780bf;
        }
      }
    }
    else {
      FUN_000781d4();
    }
LAB_0007807f:
    param_2 = pbVar3;
  } while ((*(byte *)(param_1 + 0x10) & 2) == 0);
  if ((*pbVar3 == 0x25) && (pcVar5 = (char *)FUN_000780df(pbVar3 + 1), *pcVar5 == 'n')) {
    FUN_000783cb(pcVar5);
  }
LAB_000780bf:
  if ((local_14 == 0) && ((*(byte *)(param_1 + 0x10) & 2) != 0)) {
    local_14 = -1;
  }
  return local_14;
}

