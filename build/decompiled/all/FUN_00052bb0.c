
void __regparm3 FUN_00052bb0(int *param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = *(uint *)(param_2 + 4);
  if (uVar1 < 0xe) {
    if (uVar1 == 1) {
      iVar2 = *(int *)(param_2 + 0xc);
      *param_1 = *param_1 + *(int *)(param_2 + 8);
      param_1[1] = param_1[1] + iVar2;
      (**(code **)(param_1[2] + 0x54))();
      return;
    }
    return;
  }
  if (uVar1 < 0xf) {
    param_1[0xc] = *(int *)(param_2 + 8);
    return;
  }
  if (uVar1 == 0x21) {
    param_1[10] = 1;
    return;
  }
  return;
}

