
int __regparm3 FUN_00057870(int param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  
  if (*(int *)(param_1 + 0x44) != 0) {
    iVar3 = 0;
    if (0 < *(int *)(param_1 + 0x18)) {
      iVar2 = 0;
      do {
        iVar1 = *(int *)(iVar2 + *(int *)(param_1 + 0x44));
        if (iVar1 != 0) {
          (**(code **)(*(int *)(iVar1 + 8) + 4))();
          iVar2 = extraout_ECX;
        }
        iVar3 = iVar3 + 1;
        iVar2 = iVar2 + 4;
      } while (iVar3 < *(int *)(param_1 + 0x18));
    }
    thunk_FUN_0005c370();
  }
  thunk_FUN_0005c370();
  thunk_FUN_0005c370();
  thunk_FUN_0005c370();
  return param_1;
}

