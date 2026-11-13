
void __regparm3 FUN_00044f10(undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 extraout_ECX;
  int iVar2;
  
  FUN_00049920(param_2,&DAT_00091828,param_1 + 1);
  if (param_1[1] != 0) {
    thunk_FUN_0005c320();
    uVar1 = FUN_00076bb7();
    iVar2 = 0;
    *param_1 = uVar1;
    if (0 < (int)param_1[1]) {
      do {
        FUN_0004a0b0();
        iVar2 = iVar2 + 1;
        FUN_00049920(extraout_ECX,&DAT_0009182b);
      } while (iVar2 < (int)param_1[1]);
    }
  }
  return;
}

