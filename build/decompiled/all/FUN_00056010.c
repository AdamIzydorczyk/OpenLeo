
void __regparm3 FUN_00056010(int *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *extraout_ECX;
  int extraout_ECX_00;
  
  if (*param_1 != 0) {
    FUN_00056170();
    FUN_00057a70();
    if (*extraout_ECX != 0) {
      iVar1 = FUN_00057c60();
      if (iVar1 == -1) {
        uVar2 = FUN_0001369c();
        *(undefined4 *)(extraout_ECX_00 + 0x5e) = uVar2;
        FUN_000150e8();
        FUN_00056080();
      }
    }
    FUN_00057a00();
  }
  return;
}

