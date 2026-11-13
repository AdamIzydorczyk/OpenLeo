
void __regparm3 FUN_00054860(int param_1,byte *param_2)

{
  int iVar1;
  
  if (((param_2[2] & 4) == 0) && (0 < *(int *)(param_1 + 0xc6))) {
    if (((*param_2 & 0x10) == 0) ||
       ((*(uint *)(*(int *)(*(int *)(param_1 + 0x50) + *(int *)(param_1 + 0x5c) * 4) + 0x12) &
        0x1000) == 0)) {
      iVar1 = 0;
    }
    else {
      iVar1 = (*(int *)(param_1 + 0xc6) * -2) / 3;
    }
    *(int *)(param_1 + 0xc6) = iVar1;
  }
  if ((((param_2[3] & 2) != 0) && (*(int *)(param_1 + 0xc6) < 0)) || ((*param_2 & 2) != 0)) {
    *(undefined4 *)(param_1 + 0xc6) = 0;
  }
  if ((*param_2 & 1) != 0) {
    *(undefined4 *)(param_1 + 0xc2) = 0;
  }
  return;
}

