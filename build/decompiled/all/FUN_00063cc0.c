
undefined4 __regparm3 FUN_00063cc0(int param_1,uint param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  uint uVar2;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_LAB_00097e74;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_00097e6c;
    uVar2 = 0;
    if (*(int *)(param_1 + 0x120) != 0) {
      (*(code *)**(undefined4 **)(*(int *)(param_1 + 0x120) + 8))(1,param_2,param_3);
    }
    *(undefined4 *)(param_1 + 0x120) = 0;
    FUN_000215e0();
    uVar1 = FUN_00053a30();
    if ((uVar2 & 2) != 0) {
      FUN_0005c370();
    }
    if ((param_2 & 2) != 0) {
      FUN_0005c370();
      uVar1 = extraout_ECX_00;
    }
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    uVar1 = extraout_ECX;
  }
  return uVar1;
}

