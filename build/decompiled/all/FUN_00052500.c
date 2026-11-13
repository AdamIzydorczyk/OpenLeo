
void __regparm3 FUN_00052500(undefined4 param_1,int param_2)

{
  int iVar1;
  
  iVar1 = FUN_000517e0();
  *(undefined ***)(iVar1 + 0x44) = &PTR_LAB_000a25d4;
  *(undefined4 *)(iVar1 + 0x48) = *(undefined4 *)(param_2 + 0x48);
  *(undefined4 *)(iVar1 + 0x4c) = *(undefined4 *)(param_2 + 0x4c);
  *(undefined4 *)(iVar1 + 0x50) = *(undefined4 *)(param_2 + 0x50);
  *(undefined4 *)(iVar1 + 0x54) = *(undefined4 *)(param_2 + 0x54);
  *(undefined4 *)(iVar1 + 0x58) = *(undefined4 *)(param_2 + 0x58);
  *(undefined4 *)(iVar1 + 0x5c) = *(undefined4 *)(param_2 + 0x5c);
  return;
}

