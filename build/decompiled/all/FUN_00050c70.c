
void __regparm3 FUN_00050c70(int param_1)

{
  int iVar1;
  int unaff_EBX;
  
  if ((unaff_EBX == 0) && (*(int *)(param_1 + 0x112) != 0)) {
    unaff_EBX = (**(code **)(*(int *)(*(int *)(param_1 + 0x112) + 8) + 0xdc))();
  }
  if (unaff_EBX != 0) {
    (**(code **)(*(int *)(param_1 + 8) + 0x118))();
    if (*(int *)(unaff_EBX + 0xb1) == 0x14) {
      *(undefined4 *)(unaff_EBX + 0xb1) = *(undefined4 *)(param_1 + 0xb1);
    }
    (**(code **)(*(int *)(unaff_EBX + 8) + 4))();
    *(int *)(unaff_EBX + 0xee) = param_1;
    *(undefined1 *)(unaff_EBX + 0xb5) = *(undefined1 *)(param_1 + 0xb5);
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0xe0))();
    if (iVar1 == 0) {
      (**(code **)(*(int *)(param_1 + 8) + 0x80))();
    }
    else {
      FUN_000531b0();
    }
  }
  return;
}

