
void __regparm3 FUN_00061670(int param_1,undefined4 *param_2,undefined4 param_3)

{
  undefined4 *unaff_EBX;
  
  if ((*(byte *)(param_1 + 0x60) & 0x40) != 0) {
    *param_2 = 2;
    *unaff_EBX = 0;
    *(undefined1 *)(param_1 + 0xb5) = 0;
  }
  FUN_00050970(param_3);
  return;
}

