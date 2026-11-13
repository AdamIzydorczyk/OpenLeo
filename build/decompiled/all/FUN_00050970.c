
void __regparm3 FUN_00050970(int param_1,undefined4 *param_2)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 *unaff_EBX;
  undefined4 *puVar3;
  undefined4 uStack_2c;
  undefined1 auStack_28 [2];
  byte bStack_26;
  undefined1 uStack_1c;
  
  if ((*(byte *)(param_1 + 0x67) & 0x80) != 0) {
    *(undefined4 *)(DAT_000cef40 + 0x20) = 1;
  }
  if ((*(int *)(param_1 + 0x5c) == 2) && (*(int *)(param_1 + 0x9d) < 0x32)) {
    *param_2 = 2;
    *unaff_EBX = *(undefined4 *)(*(int *)(*(int *)(param_1 + 0x50) + 8) + 4);
  }
  if (*(int *)(param_1 + 0x11e) != 0) {
    iVar2 = *(int *)(param_1 + 0x11e) + -1;
    *(int *)(param_1 + 0x11e) = iVar2;
    if (iVar2 < 0x32) {
      if (*(int *)(param_1 + 0x11a) == 7) {
        if ((*(int *)(param_1 + 0x11e) / 5) % 2 == 0) {
          uStack_2c = 0x50a27;
          uVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xdc))();
          *(undefined1 *)(param_1 + 0xb5) = uVar1;
        }
        else {
          *(undefined1 *)(param_1 + 0xb5) = 0;
        }
      }
      if (*(int *)(param_1 + 0x11a) == 6) {
        if ((*(int *)(param_1 + 0x11e) / 5) % 2 == 0) {
          *(undefined1 *)(param_1 + 0xb5) = 1;
        }
        else {
          *(undefined1 *)(param_1 + 0xb5) = 0;
        }
      }
    }
    if (*(int *)(param_1 + 0x11e) == 0) {
      *(undefined1 *)(param_1 + 0xb5) = 0;
      *(undefined4 *)(param_1 + 0x11a) = 0;
      *(undefined4 *)(param_1 + 0xb1) = *(undefined4 *)(param_1 + 0x122);
    }
    if (*(int *)(param_1 + 0x11a) == 8) {
      uStack_1c = 0;
      uStack_2c = 0x50aa8;
      FUN_000749a0();
      bStack_26 = bStack_26 | 0x20;
      uStack_2c = 0x50abc;
      FUN_00049580();
    }
  }
  if (*(int *)(param_1 + 0x126) != 0) {
    *param_2 = 2;
    *unaff_EBX = 0;
    if ((*(int *)(param_1 + 0x126) == 1) && (*(int *)(param_1 + 0xd2) == 0)) {
      *(undefined4 *)(param_1 + 0x116) = 1;
    }
    *(byte *)(param_1 + 0x60) = *(byte *)(param_1 + 0x60) & 0xfb;
  }
  if ((*(byte *)(param_1 + 0x67) & 0x20) != 0) {
    *(undefined4 *)(param_1 + 0x116) = 1;
  }
  if ((*(byte *)(param_1 + 0x60) & 4) != 0) {
    DAT_000cef4c = 0;
  }
  uStack_2c = 0x50b3a;
  iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x104))();
  if (iVar2 != 0) {
    uStack_2c = 0x50b4f;
    (**(code **)(*(int *)(param_1 + 8) + 0x11c))();
    uStack_2c = 0x50b5d;
    iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x104))();
    if ((iVar2 == 0) && ((*(int *)(param_1 + 0x112) == 0 || (DAT_000a94f4 == 0)))) {
      *param_2 = 0;
      *unaff_EBX = 0;
    }
  }
  uStack_2c = 0x50b93;
  iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x104))();
  puVar3 = (undefined4 *)auStack_28;
  if (iVar2 != 0) {
    if (*(int *)(param_1 + 0x9d) == 1) {
      uStack_2c = 0x50bb4;
      (**(code **)(*(int *)(param_1 + 8) + 0x114))();
    }
    uStack_2c = 0x50bc2;
    iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x108))();
    if ((((-1 < iVar2) && (iVar2 == *(int *)(param_1 + 0x9d))) ||
        ((puVar3 = (undefined4 *)auStack_28, iVar2 < 1 &&
         (puVar3 = (undefined4 *)auStack_28,
         *(int *)(*(int *)(*(int *)(param_1 + 0x50) + *(int *)(param_1 + 0x5c) * 4) + 4) == -iVar2))
        )) && ((puVar3 = (undefined4 *)auStack_28, *(int *)(param_1 + 0x112) != 0 &&
               (puVar3 = (undefined4 *)auStack_28, DAT_000cef4c == 0)))) {
      iVar2 = *(int *)(param_1 + 8);
      DAT_000cef4c = 1;
      uStack_2c = 0x50c12;
      uStack_2c = (**(code **)(iVar2 + 0x110))();
      (**(code **)(*(int *)(param_1 + 8) + 0x10c))();
      (**(code **)(iVar2 + 0x100))();
      puVar3 = &uStack_2c;
    }
  }
  *(undefined4 *)((int)puVar3 + -4) = 0x50c3d;
  FUN_00054450();
  return;
}

