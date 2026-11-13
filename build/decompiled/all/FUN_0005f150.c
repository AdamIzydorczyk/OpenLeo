
void __regparm3 FUN_0005f150(undefined4 param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 unaff_retaddr;
  
  iVar2 = FUN_0005eeb0(0);
  *(undefined4 *)(iVar2 + 0x30) = *param_2;
  uVar1 = param_2[1];
  *(undefined ***)(iVar2 + 0x38) = &PTR_LAB_00098278;
  *(undefined4 *)(iVar2 + 0x34) = uVar1;
  *(undefined ***)(iVar2 + 0x10) = &PTR_LAB_000a2dd8;
  *(undefined4 *)(iVar2 + 0x3c) = unaff_retaddr;
  return;
}

