
int __regparm3 FUN_00020c10(int param_1,uint param_2,undefined4 param_3)

{
  int extraout_ECX;
  uint uVar1;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_LAB_00097bc0;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_00097bb8;
    uVar1 = 0;
    if (*(int *)(param_1 + 0x120) != 0) {
      (*(code *)**(undefined4 **)(*(int *)(param_1 + 0x120) + 8))(1,param_2,param_3);
    }
    FUN_000215e0();
    param_1 = FUN_00053a30();
    if ((uVar1 & 2) != 0) {
      FUN_0005c370();
      param_1 = extraout_ECX;
    }
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

