
void __regparm3 FUN_0001e0a0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_000500b0(param_2);
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_00097970;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00097968;
  *(undefined4 *)(iVar1 + 0x12a) = 0;
  *(byte *)(iVar1 + 0xb6) = *(byte *)(iVar1 + 0xb6) & 0xfb;
  *(undefined4 *)(iVar1 + 0xad) = 500;
  DAT_000cef5c = 1;
  DAT_000cef60 = 1;
  return;
}

