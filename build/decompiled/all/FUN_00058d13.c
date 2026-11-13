
undefined4 __regparm3 FUN_00058d13(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_00058980();
  iVar1 = FUN_00058c50();
  FUN_00057a00();
  if ((iVar1 == 0) || ((*(byte *)(iVar1 + 0xe) & 2) == 0)) {
    uVar2 = 0;
  }
  else {
    iVar1 = FUN_0005c320();
    uVar2 = 0;
    if (iVar1 != 0) {
      uVar2 = FUN_00058b40(param_2);
    }
  }
  return uVar2;
}

