
undefined8 __regparm3 FUN_0002df60(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  FUN_000500b0();
  iVar1 = FUN_00076ab0();
  *(undefined4 *)(iVar1 + 100) = 0;
  *(undefined4 *)(iVar1 + 0x68) = 0;
  *(undefined4 *)(iVar1 + 0x6c) = 0;
  *(undefined ***)(iVar1 + 0x70) = &PTR_FUN_00098274;
  *(undefined4 *)(iVar1 + 0x60) = 0;
  *(undefined ***)(iVar1 + -0x14e) = &PTR_LAB_0009c958;
  *(undefined ***)(iVar1 + -0x14a) = &PTR_LAB_0009c950;
  *(undefined4 *)(iVar1 + -0x10) = 0;
  *(undefined4 *)(iVar1 + -0xc) = 0;
  *(undefined4 *)(iVar1 + -4) = 0;
  *(undefined4 *)(iVar1 + -0x24) = 0;
  *(undefined4 *)(iVar1 + -0x28) = 0;
  *(undefined4 *)(iVar1 + -0x20) = 0;
  DAT_000cef5c = 1;
  *(undefined4 *)(iVar1 + -0x2c) = 0;
  return CONCAT44(param_2,iVar1 + -0x156);
}

