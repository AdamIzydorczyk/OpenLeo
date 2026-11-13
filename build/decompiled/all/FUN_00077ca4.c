
int __regparm3 FUN_00077ca4(int *param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int extraout_ECX;
  
  if (param_2 == 0) {
    return 0;
  }
  iVar1 = (*(code *)**(undefined4 **)(&DAT_000cf518 + *param_1 * 4))(param_3);
  if (*param_1 != 2) {
    iVar1 = extraout_ECX * 8 + -8 + iVar1;
  }
  return iVar1;
}

