
undefined8 __regparm3 FUN_0003f560(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = FUN_00052860();
  *(undefined4 *)(iVar2 + 0x40) = 0;
  *(undefined ***)(iVar2 + 0x44) = &PTR_FUN_00097f50;
  *(undefined4 *)(iVar2 + 0x3c) = 0;
  *(undefined ***)(iVar2 + 8) = &PTR_FUN_000a0780;
  *(undefined ***)(iVar2 + 0x44) = &PTR_LAB_000a0774;
  uVar1 = DAT_000cf0e4;
  *(undefined ***)(iVar2 + 0xc) = &PTR_LAB_000a076c;
  *(undefined4 *)(iVar2 + 0x48) = uVar1;
  return CONCAT44(param_2,iVar2);
}

