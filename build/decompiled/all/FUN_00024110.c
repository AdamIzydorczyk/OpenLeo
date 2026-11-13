
undefined8 __regparm3 FUN_00024110(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_000500b0();
  *(undefined4 *)(iVar1 + 0x12a) = 0;
  *(undefined4 *)(iVar1 + 0x12e) = 0;
  *(undefined4 *)(iVar1 + 0x132) = 0;
  iVar1 = FUN_00025c90();
  *(undefined ***)(iVar1 + -0x12e) = &PTR_LAB_000988ec;
  *(undefined ***)(iVar1 + -0x12a) = &PTR_LAB_000988e4;
  *(undefined4 *)(iVar1 + -0x89) = 500;
  return CONCAT44(param_2,iVar1 + -0x136);
}

