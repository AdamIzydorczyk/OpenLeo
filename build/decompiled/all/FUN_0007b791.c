
void __regparm3 FUN_0007b791(undefined4 param_1,int param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int unaff_EBX;
  
  *(undefined4 *)(param_2 + 0x10) = 2;
  *(int *)(param_2 + 0x14) = unaff_EBX;
  uVar1 = *(undefined4 *)(unaff_EBX + 9);
  *(undefined4 *)(param_2 + 0x1c) = 0;
  *(undefined4 *)(param_2 + 0x18) = uVar1;
  *(undefined4 *)(param_2 + 0xc) = param_1;
  *(int *)(param_2 + 4) = param_2 + 0x10;
  FUN_00077e10(param_3);
  return;
}

