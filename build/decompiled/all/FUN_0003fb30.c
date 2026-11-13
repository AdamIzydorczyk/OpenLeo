
void __regparm3 FUN_0003fb30(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_000216c0();
  *(undefined4 *)(iVar1 + 0x16) = 0xffffffff;
  *(undefined4 *)(iVar1 + 0x1e) = 0;
  *(undefined ***)(iVar1 + 0x22) = &PTR_FUN_00096200;
  FUN_00057990();
  *(undefined ***)(iVar1 + 0x22) = &PTR_FUN_000961fc;
  *(undefined4 *)(iVar1 + 0x2a) = 0;
  *(undefined ***)(iVar1 + 0x2e) = &PTR_FUN_000a09f0;
  *(undefined4 *)(iVar1 + 0x26) = param_2;
  return;
}

