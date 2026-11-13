
undefined4 __regparm3 FUN_00045620(int param_1,int param_2)

{
  uint uVar1;
  int *unaff_EBX;
  int iVar2;
  
  if ((*unaff_EBX <= *(int *)(param_1 + 4)) && (*(int *)(param_1 + 4) <= unaff_EBX[2])) {
    if ((*(int *)(param_1 + 8) <= unaff_EBX[3]) ||
       (*(int *)(param_1 + 8) + *(int *)(param_1 + 0x10) <= unaff_EBX[3])) {
      if ((unaff_EBX[1] <= *(int *)(param_1 + 8)) ||
         (unaff_EBX[1] <= *(int *)(param_1 + 8) + *(int *)(param_1 + 0x10))) {
        if (*(int *)(param_1 + 4) - *unaff_EBX < unaff_EBX[2] - *(int *)(param_1 + 4)) {
          iVar2 = *unaff_EBX + -1;
        }
        else {
          iVar2 = unaff_EBX[2] + 1;
        }
        uVar1 = *(int *)(param_1 + 4) - iVar2;
        if (1 < (int)((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f))) {
          *(uint *)(param_2 + 0xca) = *(int *)(param_2 + 0xca) + uVar1;
          return 1;
        }
      }
    }
  }
  return 0;
}

