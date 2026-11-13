
undefined8 __regparm3 FUN_0003b180(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_000500b0();
  *(undefined4 *)(iVar1 + 0x12e) = 0;
  *(undefined4 *)(iVar1 + 0x132) = 0;
  *(undefined4 *)(iVar1 + 0x136) = 0;
  *(undefined4 *)(iVar1 + 0x13a) = 0;
  *(undefined ***)(iVar1 + 0x13e) = &PTR_FUN_00098274;
  *(undefined4 *)(iVar1 + 0x146) = 0;
  *(undefined4 *)(iVar1 + 0x14a) = 0;
  *(undefined4 *)(iVar1 + 0x14e) = 0;
  *(undefined ***)(iVar1 + 0x152) = &PTR_FUN_00098274;
  *(undefined4 *)(iVar1 + 0x142) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_000a0624;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_000a061c;
  *(undefined4 *)(iVar1 + 0x12a) = 0;
  DAT_000cef5c = 1;
  *(undefined4 *)(iVar1 + 0x156) = 0;
  return CONCAT44(param_2,iVar1);
}

