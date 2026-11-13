
void __regparm3 FUN_00054c00(int param_1)

{
  int iVar1;
  
  if (*(char *)(param_1 + 0xb5) == '\0') {
    if (((*(byte *)(param_1 + 0xb6) & 2) == 0) &&
       ((iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xbc))(), iVar1 != 0 ||
        ((*(byte *)(param_1 + 0xb6) & 1) != 0)))) {
      (**(code **)(*(int *)(param_1 + 8) + 0x50))();
      FUN_00057530();
      FUN_00057500();
      (**(code **)(*(int *)(param_1 + 8) + 0x48))();
      FUN_000425f0();
      return;
    }
    (**(code **)(*(int *)(param_1 + 8) + 0x50))();
    FUN_00057530();
    FUN_00057500();
    (**(code **)(*(int *)(param_1 + 8) + 0x48))();
    FUN_00042580();
    return;
  }
  if (((*(byte *)(param_1 + 0xb6) & 2) == 0) &&
     ((iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xbc))(), iVar1 != 0 ||
      ((*(byte *)(param_1 + 0xb6) & 1) != 0)))) {
    (**(code **)(*(int *)(param_1 + 8) + 0x50))((int)*(char *)(param_1 + 0xb5));
    FUN_00057530();
    FUN_00057500();
    (**(code **)(*(int *)(param_1 + 8) + 0x48))();
    FUN_00042620();
    return;
  }
  (**(code **)(*(int *)(param_1 + 8) + 0x50))((int)*(char *)(param_1 + 0xb5));
  FUN_00057530();
  FUN_00057500();
  (**(code **)(*(int *)(param_1 + 8) + 0x48))();
  FUN_000425b0();
  return;
}

