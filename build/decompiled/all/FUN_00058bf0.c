
uint __regparm3 FUN_00058bf0(int param_1,undefined4 *param_2)

{
  byte bVar1;
  byte *pbVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = (uint)*(byte *)*param_2;
  uVar3 = 0xffffffff;
  pbVar2 = (byte *)*param_2;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    bVar1 = *pbVar2;
    pbVar2 = pbVar2 + 1;
  } while (bVar1 != 0);
  iVar4 = 1;
  if (1 < (int)(~uVar3 - 1)) {
    pbVar2 = (byte *)*param_2;
    do {
      pbVar2 = pbVar2 + 1;
      uVar5 = uVar5 * *pbVar2;
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(~uVar3 - 1));
  }
  return uVar5 % *(uint *)(param_1 + 0x10);
}

