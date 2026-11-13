
void __regparm3 FUN_00041180(int param_1)

{
  int unaff_EBX;
  
  if (*(int *)(param_1 + 0xee) == 0) {
    *(undefined4 *)(param_1 + 0xee) = 0x4b;
    *(int *)(param_1 + 0xad) = *(int *)(param_1 + 0xad) - unaff_EBX;
  }
  return;
}

