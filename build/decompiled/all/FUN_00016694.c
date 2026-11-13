
undefined8 __regparm3 FUN_00016694(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = ll_alloc_try(param_2,param_3);
  iVar2 = FUN_000165d8();
  if (iVar2 == 0) {
    DAT_000cf2cc = DAT_000cf2cc + 1;
    *(undefined4 *)(&DAT_000cf2d0 + DAT_000cf2cc * 4) = uVar1;
    FUN_0005c370();
    uVar1 = 0;
  }
  return CONCAT44(param_2,uVar1);
}

