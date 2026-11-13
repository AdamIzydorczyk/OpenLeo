
int __regparm3 FUN_000370a0(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  uint extraout_ECX;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_FUN_0009f380;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_0009f378;
    iVar1 = 0;
    for (iVar2 = DAT_000cedf0; iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x288)) {
      if (iVar2 == param_1) {
        if (iVar1 == 0) {
          DAT_000cedf0 = *(int *)(param_1 + 0x288);
        }
        else {
          *(undefined4 *)(iVar1 + 0x288) = *(undefined4 *)(param_1 + 0x288);
        }
        break;
      }
      iVar1 = iVar2;
    }
    if (*(int *)(param_1 + 300) != 0) {
      (*(code *)**(undefined4 **)(*(int *)(param_1 + 300) + 8))();
    }
    FUN_000215e0();
    param_1 = FUN_00053a30();
    if ((extraout_ECX & 2) != 0) {
      FUN_0005c370();
    }
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

