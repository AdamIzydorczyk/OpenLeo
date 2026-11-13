
void __regparm3 FUN_00015e98(char *param_1,ushort param_2)

{
  byte bVar1;
  int extraout_ECX;
  int iVar2;
  int extraout_ECX_00;
  uint uVar3;
  int iVar4;
  int *piVar5;
  
  FUN_000749a0();
  iVar2 = extraout_ECX;
  for (iVar4 = 0; iVar4 < (int)(uint)DAT_000ae674; iVar4 = iVar4 + 1) {
    DAT_000b38d0 = iVar2;
    if (*(char *)(iVar2 + 1) != '\0') {
      DAT_000b38d8 = *(int *)(iVar2 + 10) * 0x800 + -1;
      DAT_000b38dc = *(int *)(iVar2 + 0xe) << 0xb;
      DAT_000b38e0 = *(int *)(iVar2 + 0x12) * 0x800 + -1;
      DAT_000a7c40 = *(undefined2 *)(iVar2 + 0x24);
      DAT_000c3d72 = *(undefined2 *)(iVar2 + 0x26);
      FUN_00015ca4();
      iVar2 = extraout_ECX_00;
    }
    iVar2 = iVar2 + 0x28;
  }
  if ((DAT_000ae672 & 2) == 0) {
    bVar1 = 0x18 - DAT_000b38ec;
    if ((DAT_000ae672 & 1) == 0) {
      uVar3 = (uint)param_2;
    }
    else {
      uVar3 = (uint)param_2 * 2;
    }
    piVar5 = &DAT_000afb90;
    do {
      iVar2 = *piVar5 >> (bVar1 & 0x1f);
      if (iVar2 < 0x80) {
        if (iVar2 < -0x80) {
          iVar2 = -0x80;
        }
      }
      else {
        iVar2 = 0x7f;
      }
      *param_1 = (char)iVar2 + -0x80;
      piVar5 = piVar5 + 1;
      param_1 = param_1 + 1;
      uVar3 = uVar3 - 1;
    } while (uVar3 != 0);
  }
  else {
    bVar1 = 0x10 - DAT_000b38ec;
    if ((DAT_000ae672 & 1) == 0) {
      uVar3 = (uint)param_2;
    }
    else {
      uVar3 = (uint)param_2 * 2;
    }
    piVar5 = &DAT_000afb90;
    do {
      iVar2 = *piVar5 >> (bVar1 & 0x1f);
      if (iVar2 < 0x8000) {
        if (iVar2 < -0x8000) {
          iVar2 = -0x8000;
        }
      }
      else {
        iVar2 = 0x7fff;
      }
      *(short *)param_1 = (short)iVar2;
      piVar5 = piVar5 + 1;
      param_1 = param_1 + 2;
      uVar3 = uVar3 - 1;
    } while (uVar3 != 0);
  }
  return;
}

