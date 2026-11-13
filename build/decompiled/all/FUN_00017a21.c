
undefined8 __regparm3 FUN_00017a21(uint param_1,int param_2,uint param_3)

{
  uint uVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint unaff_EBX;
  byte *pbVar6;
  uint uVar7;
  uint local_14;
  
  iVar3 = (param_3 - 1) * 0x4e;
  pbVar6 = &DAT_000a85a8 + iVar3;
  if ((*pbVar6 & 2) != 0) {
    return CONCAT44(iVar3,6);
  }
  uVar1 = param_1 >> 0x10;
  uVar4 = (param_1 + unaff_EBX) - 1;
  uVar5 = uVar4 >> 0x10;
  *pbVar6 = *pbVar6 | 0x12;
  local_14 = param_1;
  if (3 < param_3) {
    local_14 = param_1 >> 1;
    uVar4 = uVar4 >> 1;
    unaff_EBX = unaff_EBX >> 1;
  }
  uVar7 = uVar4 & 0xffff;
  if (uVar1 == uVar5) {
    *pbVar6 = *pbVar6 & 0xfb;
    uVar5 = uVar1;
  }
  else {
    *pbVar6 = *pbVar6 | 4;
    *(uint *)(&DAT_000a85e9 + iVar3) = uVar5;
    if (param_3 < 4) {
      *(undefined4 *)(&DAT_000a85ed + iVar3) = 0;
    }
    else {
      if (((&DAT_000a85e9)[iVar3] & 1) == 0) {
        *(undefined4 *)(&DAT_000a85ed + iVar3) = 0;
      }
      else {
        *(undefined4 *)(&DAT_000a85ed + iVar3) = 0x8000;
      }
      uVar7 = uVar4 & 0x7fff;
    }
    *(uint *)(&DAT_000a85f1 + iVar3) = uVar7;
    unaff_EBX = (unaff_EBX - uVar7) - 1;
  }
  if ((param_2 == 3) && ((*pbVar6 & 4) != 0)) {
    return CONCAT44(uVar5,7);
  }
  *(undefined4 *)(&DAT_000a85e1 + iVar3) = 0;
  *(uint *)(&DAT_000a85e5 + iVar3) = unaff_EBX;
  *(uint *)(&DAT_000a85dd + iVar3) = local_14 & 0xffff;
  *(uint *)(&DAT_000a85d9 + iVar3) = uVar1;
  switch(param_2) {
  case 1:
    bVar2 = (&DAT_000a85d4)[iVar3];
    break;
  case 2:
    bVar2 = (&DAT_000a85d0)[iVar3];
    break;
  case 3:
    bVar2 = (&DAT_000a85d4)[iVar3] | 0x10;
    break;
  case 4:
    bVar2 = (&DAT_000a85d0)[iVar3] | 0x10;
    break;
  default:
    goto switchD_00017b01_default;
  }
  (&DAT_000a85d8)[iVar3] = bVar2;
switchD_00017b01_default:
  out((short)*(undefined4 *)(&DAT_000a85c4 + iVar3),(char)*(undefined4 *)(&DAT_000a85b0 + iVar3));
  out((short)*(undefined4 *)(&DAT_000a85c8 + iVar3),(&DAT_000a85d8)[iVar3]);
  out((short)*(undefined4 *)(&DAT_000a85cc + iVar3),0);
  out((short)*(undefined4 *)(&DAT_000a85bc + iVar3),(char)*(undefined4 *)(&DAT_000a85dd + iVar3));
  out((short)*(undefined4 *)(&DAT_000a85bc + iVar3),
      (char)((uint)*(undefined4 *)(&DAT_000a85dd + iVar3) >> 8));
  out((short)*(undefined4 *)(&DAT_000a85b8 + iVar3),(char)*(undefined4 *)(&DAT_000a85d9 + iVar3));
  out((short)*(undefined4 *)(&DAT_000a85cc + iVar3),0);
  out((short)*(undefined4 *)(&DAT_000a85c0 + iVar3),(char)(unaff_EBX - 1));
  out((short)*(undefined4 *)(&DAT_000a85c0 + iVar3),(char)(unaff_EBX - 1 >> 8));
  out((short)*(undefined4 *)(&DAT_000a85c4 + iVar3),(char)*(undefined4 *)(&DAT_000a85b4 + iVar3));
  return CONCAT44(*(undefined4 *)(&DAT_000a85c4 + iVar3),1);
}

