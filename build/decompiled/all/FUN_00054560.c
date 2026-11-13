
void __regparm3 FUN_00054560(int param_1,int param_2)

{
  uint uVar1;
  undefined4 *puVar2;
  int extraout_ECX;
  
  uVar1 = *(uint *)(param_2 + 4);
  if (uVar1 < 6) {
    if (uVar1 < 3) {
      if (uVar1 != 2) {
        FUN_00053770();
        return;
      }
      *(undefined4 *)(param_1 + 0xc2) = *(undefined4 *)(param_2 + 8);
      *(undefined4 *)(param_1 + 0xc6) = *(undefined4 *)(param_2 + 0xc);
    }
    else if (uVar1 < 4) {
      *(byte *)(param_1 + 0xb6) = *(byte *)(param_1 + 0xb6) | 4;
    }
    else {
      if (uVar1 != 4) {
        FUN_00053770();
        return;
      }
      *(byte *)(param_1 + 0xb6) = *(byte *)(param_1 + 0xb6) | 8;
    }
  }
  else {
    if (uVar1 < 7) {
      if (*(int *)(param_2 + 8) != 0) {
        *(int *)(param_1 + 0x6d) = *(int *)(param_2 + 8);
      }
      *(undefined4 *)(param_1 + 0x71) = *(undefined4 *)(param_2 + 0xc);
      return;
    }
    if (uVar1 < 0xd) {
      if (uVar1 == 8) {
        *(undefined4 *)(param_1 + 0xc2) = 0;
        *(undefined4 *)(param_1 + 0xc6) = 0;
        return;
      }
      FUN_00053770();
      return;
    }
    if (uVar1 < 0xe) {
      *(int *)(param_1 + 0xc2) = *(int *)(param_1 + 0xc2) + *(int *)(param_2 + 8);
      *(int *)(param_1 + 0xc6) = *(int *)(param_1 + 0xc6) + *(int *)(param_2 + 0xc);
    }
    else {
      if (uVar1 != 0x1b) {
        FUN_00053770();
        return;
      }
      if (*(int **)(param_1 + 0xa9) != (int *)0x0) {
        (**(code **)(**(int **)(param_1 + 0xa9) + 4))();
        param_2 = extraout_ECX;
      }
      puVar2 = *(undefined4 **)(param_2 + 8);
      *(undefined4 **)(param_1 + 0xa9) = puVar2;
      if (puVar2 != (undefined4 *)0x0) {
        (**(code **)*puVar2)();
        return;
      }
    }
  }
  return;
}

