
void __regparm3 FUN_000594a0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  if (*(int *)(param_1 + 0x1c) != 0) {
    *(undefined4 *)(param_1 + 0x24) = 0;
    uVar2 = *(undefined4 *)(param_1 + 0x1c);
    while( true ) {
      *(undefined4 *)(param_1 + 0x20) = uVar2;
      iVar1 = *(int *)(param_1 + 0x20);
      if (*(int *)(iVar1 + 4) == 0) break;
      *(int *)(param_1 + 0x24) = iVar1;
      uVar2 = *(undefined4 *)(iVar1 + 4);
    }
    if (*(int *)(param_1 + 0x24) == 0) {
      *(undefined4 *)(param_1 + 0x1c) = 0;
    }
    else {
      *(undefined4 *)(*(int *)(param_1 + 0x24) + 4) = 0;
    }
    if (*(int *)(param_1 + 0x20) != 0) {
      FUN_00059460();
      FUN_0005c370();
    }
    *(int *)(param_1 + 0x18) = *(int *)(param_1 + 0x18) + -1;
  }
  return;
}

