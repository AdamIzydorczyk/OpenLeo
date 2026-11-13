
void __regparm3 FUN_0004b970(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  int iVar3;
  int local_24;
  int local_20;
  
  iVar2 = *(int *)(*(int *)(param_1 + 0x390) + 0x18);
  iVar3 = 0;
  thunk_FUN_0005c320();
  FUN_00076ab0();
  if (0 < extraout_ECX) {
    do {
      FUN_00057530();
      FUN_00057500();
      iVar3 = iVar3 + 1;
      FUN_0004b940();
    } while (iVar3 < iVar2);
  }
  local_24 = 0;
  iVar3 = 0;
  iVar2 = 0;
  local_20 = 0;
  while( true ) {
    for (; iVar3 < *(int *)(param_1 + local_20 + 0x44); iVar3 = iVar3 + 1) {
      for (iVar1 = *(int *)(*(int *)(param_1 + local_20 + 0x58) + iVar2); iVar1 != 0;
          iVar1 = *(int *)(iVar1 + 0x34)) {
        (**(code **)(*(int *)(iVar1 + 8) + 100))();
      }
      iVar2 = iVar2 + 4;
    }
    local_24 = local_24 + 4;
    if (local_24 == 0xc) break;
    iVar3 = 0;
    iVar2 = 0;
    local_20 = local_24;
  }
  for (iVar2 = *(int *)(param_1 + 100); iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x34)) {
    (**(code **)(*(int *)(iVar2 + 8) + 100))();
  }
  thunk_FUN_0005c370();
  return;
}

