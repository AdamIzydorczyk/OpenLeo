
undefined4 __regparm3 FUN_00055770(int param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 extraout_ECX;
  int extraout_ECX_00;
  undefined4 extraout_ECX_01;
  int iVar3;
  int iVar4;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_FUN_000981e8;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_000981e0;
    iVar4 = 0;
    if (0 < *(int *)(param_1 + 0x3c)) {
      iVar3 = 0;
      do {
        iVar1 = *(int *)(*(int *)(param_1 + 0x40) + iVar3);
        if (iVar1 != 0) {
          *(undefined4 *)(iVar1 + 0x30) = 0;
          iVar1 = *(int *)(iVar3 + *(int *)(param_1 + 0x40));
          if (iVar1 != 0) {
            (*(code *)**(undefined4 **)(iVar1 + 8))();
            param_1 = extraout_ECX_00;
          }
        }
        iVar4 = iVar4 + 1;
        iVar3 = iVar3 + 4;
      } while (iVar4 < *(int *)(param_1 + 0x3c));
    }
    thunk_FUN_0005c370();
    uVar2 = FUN_00052960();
    if ((param_2 & 2) != 0) {
      FUN_0005c370();
      uVar2 = extraout_ECX_01;
    }
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    uVar2 = extraout_ECX;
  }
  return uVar2;
}

