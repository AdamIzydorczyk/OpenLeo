
void FUN_0003ffb0(void)

{
  undefined4 in_ECX;
  int extraout_ECX;
  int iVar1;
  int extraout_EDX;
  
  FUN_000537f0(in_ECX);
  iVar1 = 0;
  if (*(int *)(extraout_ECX + 0x10e) < 1) {
    return;
  }
  do {
    (**(code **)(*(int *)(*(int *)(*(int *)(extraout_ECX + 0x106) + iVar1 * 4) + 0x2e) + 0xc))();
    iVar1 = extraout_EDX;
  } while (extraout_EDX < *(int *)(extraout_ECX + 0x10e));
  return;
}

