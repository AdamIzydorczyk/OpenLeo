
void __regparm3 FUN_00058ef0(undefined4 *param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 unaff_EBX;
  
  uVar1 = *(undefined4 *)(param_2 + 0x12);
  param_1[2] = 0;
  param_1[1] = uVar1;
  uVar1 = *(undefined4 *)(param_2 + 0x16);
  *param_1 = &PTR_LAB_000a28e8;
  param_1[3] = uVar1;
  param_1[4] = unaff_EBX;
  return;
}

