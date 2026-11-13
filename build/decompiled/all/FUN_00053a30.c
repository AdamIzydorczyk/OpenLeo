
int __regparm3 FUN_00053a30(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_FUN_000a2498;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_000a2490;
    (**(code **)(*(int *)(param_1 + 8) + 0x94))();
    iVar3 = 1;
    if (0 < *(int *)(param_1 + 0x58)) {
      iVar2 = 4;
      do {
        iVar1 = *(int *)(iVar2 + *(int *)(param_1 + 0x50));
        if (iVar1 != 0) {
          (**(code **)(*(int *)(iVar1 + 0x44) + 0x24))();
          iVar2 = extraout_ECX;
        }
        iVar3 = iVar3 + 1;
        iVar2 = iVar2 + 4;
      } while (iVar3 <= *(int *)(param_1 + 0x58));
    }
    thunk_FUN_0005c370();
    FUN_00021d70();
    param_1 = FUN_00052960();
    if ((param_2 & 2) != 0) {
      FUN_0005c370();
    }
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

