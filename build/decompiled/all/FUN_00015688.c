
void __regparm3 FUN_00015688(undefined4 param_1,uint param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  int unaff_EBX;
  int iVar3;
  int unaff_ESI;
  int *unaff_EDI;
  int in_FS_OFFSET;
  
  uVar2 = param_2 >> 4;
  if (uVar2 != 0) {
    if ((short)(DAT_000a7c40 + DAT_000c3d72) == 0) {
      uRam00000000 = 0;
    }
    do {
      uVar1 = (uint)*(byte *)(unaff_ESI + (unaff_EBX >> 0xb));
      *unaff_EDI = *unaff_EDI + *(int *)(uVar1 * 4);
      unaff_EDI[1] = unaff_EDI[1] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (unaff_EBX + param_3 >> 0xb));
      iVar3 = unaff_EBX + param_3 + param_3;
      unaff_EDI[2] = unaff_EDI[2] + *(int *)(uVar1 * 4);
      unaff_EDI[3] = unaff_EDI[3] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[4] = unaff_EDI[4] + *(int *)(uVar1 * 4);
      unaff_EDI[5] = unaff_EDI[5] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[6] = unaff_EDI[6] + *(int *)(uVar1 * 4);
      unaff_EDI[7] = unaff_EDI[7] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[8] = unaff_EDI[8] + *(int *)(uVar1 * 4);
      unaff_EDI[9] = unaff_EDI[9] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[10] = unaff_EDI[10] + *(int *)(uVar1 * 4);
      unaff_EDI[0xb] = unaff_EDI[0xb] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0xc] = unaff_EDI[0xc] + *(int *)(uVar1 * 4);
      unaff_EDI[0xd] = unaff_EDI[0xd] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0xe] = unaff_EDI[0xe] + *(int *)(uVar1 * 4);
      unaff_EDI[0xf] = unaff_EDI[0xf] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0x10] = unaff_EDI[0x10] + *(int *)(uVar1 * 4);
      unaff_EDI[0x11] = unaff_EDI[0x11] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0x12] = unaff_EDI[0x12] + *(int *)(uVar1 * 4);
      unaff_EDI[0x13] = unaff_EDI[0x13] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0x14] = unaff_EDI[0x14] + *(int *)(uVar1 * 4);
      unaff_EDI[0x15] = unaff_EDI[0x15] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0x16] = unaff_EDI[0x16] + *(int *)(uVar1 * 4);
      unaff_EDI[0x17] = unaff_EDI[0x17] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0x18] = unaff_EDI[0x18] + *(int *)(uVar1 * 4);
      unaff_EDI[0x19] = unaff_EDI[0x19] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0x1a] = unaff_EDI[0x1a] + *(int *)(uVar1 * 4);
      unaff_EDI[0x1b] = unaff_EDI[0x1b] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      iVar3 = iVar3 + param_3;
      unaff_EDI[0x1c] = unaff_EDI[0x1c] + *(int *)(uVar1 * 4);
      unaff_EDI[0x1d] = unaff_EDI[0x1d] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      uVar1 = (uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb));
      unaff_EBX = iVar3 + param_3;
      unaff_EDI[0x1e] = unaff_EDI[0x1e] + *(int *)(uVar1 * 4);
      unaff_EDI[0x1f] = unaff_EDI[0x1f] + *(int *)(in_FS_OFFSET + uVar1 * 4);
      unaff_EDI = unaff_EDI + 0x20;
      uVar2 = uVar2 - 1;
    } while (uVar2 != 0);
  }
  param_2 = param_2 & 0xf;
  if (param_2 != 0) {
    do {
      uVar2 = (uint)*(byte *)(unaff_ESI + (unaff_EBX >> 0xb));
      unaff_EBX = unaff_EBX + param_3;
      *unaff_EDI = *unaff_EDI + *(int *)(uVar2 * 4);
      unaff_EDI[1] = unaff_EDI[1] + *(int *)(in_FS_OFFSET + uVar2 * 4);
      unaff_EDI = unaff_EDI + 2;
      param_2 = param_2 - 1;
    } while (param_2 != 0);
  }
  return;
}

