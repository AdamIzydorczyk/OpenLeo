
undefined8 __regparm3 FUN_00051320(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  int iVar4;
  
  iVar1 = FUN_0001c4d0();
  uVar3 = 0;
  if ((iVar1 != 0) && (iVar1 < 0x100)) {
    iVar4 = 0;
    do {
      iVar4 = iVar4 + 4;
      iVar2 = FUN_000510c0(iVar1,param_2,param_3);
    } while (iVar4 != 0x38);
    uVar3 = 0;
    if (extraout_ECX != 0 || iVar2 != 0) {
      iVar1 = FUN_00076e98();
      uVar3 = extraout_ECX_00;
      if (iVar1 == 0x50) {
        FUN_0001c580();
        uVar3 = extraout_ECX_01;
      }
    }
  }
  return CONCAT44(param_2,uVar3);
}

