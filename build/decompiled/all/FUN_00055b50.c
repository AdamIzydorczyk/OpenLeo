
void __regparm3 FUN_00055b50(int param_1,undefined4 param_2,undefined4 param_3)

{
  ushort uVar1;
  ushort uVar2;
  int unaff_EBX;
  
  if (*(int *)(param_1 + 4) != 0) {
    FUN_00012ae8(param_2);
    uVar1 = (**(code **)(*(int *)(param_1 + 0x18) + 4))();
    if (unaff_EBX == 0) {
      uVar2 = 0;
    }
    else {
      uVar2 = 0x10;
    }
    FUN_00012b14(param_3,*(undefined4 *)(param_1 + 0xc),uVar2 | uVar1);
  }
  return;
}

