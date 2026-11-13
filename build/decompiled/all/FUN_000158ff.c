
void __regparm3 FUN_000158ff(undefined4 param_1,uint param_2,int param_3)

{
  uint uVar1;
  int unaff_EBX;
  int iVar2;
  int iVar3;
  int unaff_ESI;
  int *unaff_EDI;
  
  uVar1 = param_2 >> 4;
  if (uVar1 != 0) {
    if (DAT_000a7c40 == 0) {
      uRam00000000 = 0;
    }
    do {
      *unaff_EDI = *unaff_EDI + *(int *)((uint)*(byte *)(unaff_ESI + (unaff_EBX >> 0xb)) * 4);
      iVar2 = unaff_EBX + param_3 + param_3;
      unaff_EDI[1] = unaff_EDI[1] +
                     *(int *)((uint)*(byte *)(unaff_ESI + (unaff_EBX + param_3 >> 0xb)) * 4);
      iVar3 = iVar2 + param_3;
      unaff_EDI[2] = unaff_EDI[2] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar2 >> 0xb)) * 4);
      iVar2 = iVar3 + param_3;
      unaff_EDI[3] = unaff_EDI[3] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb)) * 4);
      iVar3 = iVar2 + param_3;
      unaff_EDI[4] = unaff_EDI[4] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar2 >> 0xb)) * 4);
      iVar2 = iVar3 + param_3;
      unaff_EDI[5] = unaff_EDI[5] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb)) * 4);
      iVar3 = iVar2 + param_3;
      unaff_EDI[6] = unaff_EDI[6] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar2 >> 0xb)) * 4);
      iVar2 = iVar3 + param_3;
      unaff_EDI[7] = unaff_EDI[7] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb)) * 4);
      iVar3 = iVar2 + param_3;
      unaff_EDI[8] = unaff_EDI[8] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar2 >> 0xb)) * 4);
      iVar2 = iVar3 + param_3;
      unaff_EDI[9] = unaff_EDI[9] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb)) * 4);
      iVar3 = iVar2 + param_3;
      unaff_EDI[10] = unaff_EDI[10] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar2 >> 0xb)) * 4);
      iVar2 = iVar3 + param_3;
      unaff_EDI[0xb] = unaff_EDI[0xb] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb)) * 4);
      iVar3 = iVar2 + param_3;
      unaff_EDI[0xc] = unaff_EDI[0xc] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar2 >> 0xb)) * 4);
      iVar2 = iVar3 + param_3;
      unaff_EDI[0xd] = unaff_EDI[0xd] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb)) * 4);
      iVar3 = iVar2 + param_3;
      unaff_EDI[0xe] = unaff_EDI[0xe] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar2 >> 0xb)) * 4);
      unaff_EBX = iVar3 + param_3;
      unaff_EDI[0xf] = unaff_EDI[0xf] + *(int *)((uint)*(byte *)(unaff_ESI + (iVar3 >> 0xb)) * 4);
      unaff_EDI = unaff_EDI + 0x10;
      uVar1 = uVar1 - 1;
    } while (uVar1 != 0);
  }
  param_2 = param_2 & 0xf;
  if (param_2 != 0) {
    do {
      iVar2 = unaff_EBX >> 0xb;
      unaff_EBX = unaff_EBX + param_3;
      *unaff_EDI = *unaff_EDI + *(int *)((uint)*(byte *)(unaff_ESI + iVar2) * 4);
      unaff_EDI = unaff_EDI + 1;
      param_2 = param_2 - 1;
    } while (param_2 != 0);
  }
  return;
}

