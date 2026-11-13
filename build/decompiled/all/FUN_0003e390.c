
void __regparm3 FUN_0003e390(int param_1)

{
  int iVar1;
  int iVar2;
  
  if (((*(byte *)(param_1 + 0x67) & 8) != 0) && (*(int *)(param_1 + 0x11a) != 0)) {
    if ((*(int *)(param_1 + 0x11e) != 0) || (*(int *)(param_1 + 0x122) != 0)) {
      iVar1 = (**(code **)(*(int *)(*(int *)(param_1 + 0x11a) + 8) + 0x3c))();
      if (iVar1 != 0) {
        if (*(int *)(param_1 + 0x102) - *(int *)(param_1 + 0x10e) < 0) {
          iVar1 = -1;
        }
        else {
          iVar1 = 1;
        }
        iVar2 = (**(code **)(*(int *)(*(int *)(param_1 + 0x11a) + 8) + 0x70))();
        FUN_00052b10(iVar1 * iVar2);
        FUN_00052b10();
      }
    }
    *(undefined4 *)(param_1 + 0x11a) = 0;
  }
  FUN_00054450();
  return;
}

