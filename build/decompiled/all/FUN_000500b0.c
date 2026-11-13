
void __regparm3 FUN_000500b0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_00053ff0(param_2);
  *(undefined4 *)(iVar1 + 0xee) = 0;
  *(undefined4 *)(iVar1 + 0xf2) = 1;
  *(undefined4 *)(iVar1 + 0xf6) = 2000;
  *(undefined4 *)(iVar1 + 0xfa) = 0;
  *(undefined4 *)(iVar1 + 0xfe) = 0;
  *(undefined4 *)(iVar1 + 0x102) = 0;
  *(undefined4 *)(iVar1 + 0x106) = 1;
  *(undefined4 *)(iVar1 + 0x112) = 0;
  *(undefined4 *)(iVar1 + 0x11a) = 0;
  *(undefined4 *)(iVar1 + 0x11e) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_FUN_000a2218;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_000a2210;
  *(undefined4 *)(iVar1 + 0x10a) = 0;
  *(undefined4 *)(iVar1 + 0x10e) = 0;
  *(undefined4 *)(iVar1 + 0x126) = 0;
  *(undefined4 *)(iVar1 + 0x116) = 0;
  *(undefined1 *)(iVar1 + 0xb5) = 0;
  DAT_000cef5c = 1;
  DAT_000a94f4 = 3;
  return;
}

