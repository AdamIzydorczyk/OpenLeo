
void __regparm3 FUN_00050ed0(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  int unaff_EBX;
  
  if ((((*(int *)(param_1 + 0x11a) != 6) && (*(int *)(param_1 + 0x11a) != 7)) && (unaff_EBX != 0))
     && ((*(int *)(param_1 + 0x126) == 0 &&
         ((iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x40))(), iVar1 == 0 ||
          (iVar1 = (**(code **)(*(int *)(param_2 + 8) + 0x40))(), iVar1 != 0)))))) {
    uVar2 = (**(code **)(*(int *)(param_2 + 8) + 0xc))();
    *(undefined4 *)(param_1 + 0x10a) = uVar2;
    uVar2 = (**(code **)(*(int *)(param_2 + 8) + 0x70))();
    iVar1 = *(int *)(param_1 + 0xd2);
    *(undefined4 *)(param_1 + 0x10e) = uVar2;
    while (iVar1 != 0) {
      FUN_000551f0();
      iVar1 = *(int *)(param_1 + 0xd2);
    }
    iVar1 = FUN_000502f0();
    if (iVar1 == 0) {
      (**(code **)(*(int *)(param_1 + 8) + 0xe4))();
      FUN_00054550();
    }
  }
  return;
}

