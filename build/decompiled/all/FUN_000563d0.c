
undefined8 __regparm3 FUN_000563d0(int param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_EDX;
  int iVar2;
  undefined8 uVar3;
  
  if (0 < *(int *)(param_1 + 0x24)) {
    iVar2 = 0;
    do {
      (*(code *)**(undefined4 **)(*(int *)(iVar2 + *(int *)(param_1 + 0x40)) + 8))();
      iVar2 = iVar2 + 4;
      param_1 = extraout_ECX;
    } while (extraout_EDX < *(int *)(extraout_ECX + 0x24));
  }
  iVar2 = (DAT_000cf064 + 1) / 10;
  DAT_000cf064 = (DAT_000cf064 + 1) % 10;
  if (DAT_000cf064 == 0) {
    iVar1 = *(int *)(param_1 + 0x5a);
    while (iVar1 != 0) {
      uVar3 = (**(code **)(*(int *)(iVar1 + 8) + 4))();
      iVar2 = (int)uVar3;
      iVar1 = *(int *)((ulonglong)uVar3 >> 0x20);
    }
  }
  return CONCAT44(param_2,iVar2);
}

