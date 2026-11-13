
undefined8 __regparm3 FUN_00059430(int param_1,undefined4 param_2)

{
  if (*(int *)(param_1 + 0x24) != 0) {
    *(undefined4 *)(*(int *)(param_1 + 0x24) + 4) = *(undefined4 *)(*(int *)(param_1 + 0x20) + 4);
    *(undefined4 *)(*(int *)(param_1 + 0x20) + 4) = *(undefined4 *)(param_1 + 0x1c);
    *(undefined4 *)(param_1 + 0x1c) = *(undefined4 *)(param_1 + 0x20);
  }
  return CONCAT44(param_2,**(undefined4 **)(param_1 + 0x1c));
}

