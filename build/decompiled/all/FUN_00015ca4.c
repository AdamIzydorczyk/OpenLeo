
void __regparm3 FUN_00015ca4(int param_1,int param_2)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  bool bVar5;
  
  do {
    if ((short)param_2 == 0) {
      return;
    }
    bVar1 = *(byte *)(DAT_000b38d0 + 2);
    if ((bVar1 & 0x80) == 0) {
      if ((bVar1 & 0x10) == 0) {
        bVar5 = SBORROW4(DAT_000b38d8,*(int *)(DAT_000b38d0 + 0x1c));
        iVar3 = DAT_000b38d8 - *(int *)(DAT_000b38d0 + 0x1c);
        goto LAB_00015d35;
      }
      if (DAT_000b38e0 < *(int *)(DAT_000b38d0 + 0x1c)) {
        if ((*(byte *)(DAT_000b38d0 + 2) & 0x20) != 0) {
          *(byte *)(DAT_000b38d0 + 2) = *(byte *)(DAT_000b38d0 + 2) | 0x80;
          *(int *)(DAT_000b38d0 + 0x20) = -*(int *)(DAT_000b38d0 + 0x20);
          iVar3 = *(int *)(DAT_000b38d0 + 0x1c) - DAT_000b38e0;
          goto LAB_00015d27;
        }
        iVar3 = DAT_000b38dc + (*(int *)(DAT_000b38d0 + 0x1c) - DAT_000b38e0);
LAB_00015d29:
        *(int *)(DAT_000b38d0 + 0x1c) = iVar3;
      }
    }
    else if ((bVar1 & 0x10) == 0) {
      iVar3 = *(int *)(DAT_000b38d0 + 0x1c);
      bVar5 = false;
LAB_00015d35:
      if (bVar5 != iVar3 < 0) {
        *(undefined4 *)(DAT_000b38d0 + 0x1c) = 0;
        goto LAB_00015d42;
      }
    }
    else {
      iVar3 = *(int *)(DAT_000b38d0 + 0x1c);
      if (iVar3 < DAT_000b38dc) {
        if ((*(byte *)(DAT_000b38d0 + 2) & 0x20) == 0) {
          iVar3 = DAT_000b38dc - iVar3;
LAB_00015d27:
          iVar3 = DAT_000b38e0 - iVar3;
          goto LAB_00015d29;
        }
        *(int *)(DAT_000b38d0 + 0x1c) = DAT_000b38dc * 2 - iVar3;
        *(byte *)(DAT_000b38d0 + 2) = *(byte *)(DAT_000b38d0 + 2) & 0x7f;
        *(int *)(DAT_000b38d0 + 0x20) = -*(int *)(DAT_000b38d0 + 0x20);
      }
    }
    if ((&DAT_000c3d74)[*(int *)(DAT_000b38d0 + 2) >> 0x10] == 0) {
      *(undefined4 *)(DAT_000b38d0 + 0x1c) = 0;
LAB_00015d42:
      *(undefined1 *)(DAT_000b38d0 + 1) = 0;
      return;
    }
    uVar2 = FUN_00015c68(param_2,param_1);
    if (uVar2 == 0) goto LAB_00015d42;
    if (*(char *)(DAT_000b38d0 + 0x1a) != '\0') {
      if ((DAT_000ae672 & 1) == 0) {
        FUN_000158ff();
      }
      else {
        FUN_00015688();
      }
    }
    uVar4 = (uint)uVar2;
    *(uint *)(DAT_000b38d0 + 0x1c) =
         *(int *)(DAT_000b38d0 + 0x1c) + *(int *)(DAT_000b38d0 + 0x20) * uVar4;
    param_2 = param_2 - uVar4;
    if ((DAT_000ae672 & 1) != 0) {
      uVar4 = uVar4 * 2;
    }
    param_1 = param_1 + uVar4 * 4;
  } while( true );
}

