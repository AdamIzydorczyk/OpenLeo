
void __regparm3 FUN_00054450(int param_1,undefined4 *param_2)

{
  undefined4 *unaff_EBX;
  
  if (*(short *)(*(int *)(*(int *)(*(int *)(DAT_000cef40 + 0x390) + 0x44) +
                         *(int *)(*(int *)(DAT_000cef40 + 0x390) + 0x1c) * 4) + 2) + 500 <
      *(int *)(param_1 + 4)) {
    *(undefined4 *)(param_1 + 0xad) = 0xffffffff;
  }
  if ((*(int *)(param_1 + 0x5c) == 2) && ((*(byte *)(param_1 + 0x60) & 4) != 0)) {
    (**(code **)(*(int *)(param_1 + 8) + 0xd4))();
    return;
  }
  if ((*(int *)(param_1 + 0xad) < 0) && (*(int *)(param_1 + 0x5c) != 2)) {
    *param_2 = 2;
    *unaff_EBX = 0;
    return;
  }
  FUN_00053410();
  return;
}

