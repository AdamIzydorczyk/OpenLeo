
void __regparm3 FUN_00025b40(int *param_1)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  
  if (*param_1 != 0) {
    iVar2 = 0;
    if (0 < param_1[2]) {
      iVar1 = 0;
      do {
        iVar2 = iVar2 + 1;
        (**(code **)(param_1[4] + 4))
                  (*(undefined4 *)(iVar1 + *param_1),*(undefined4 *)(iVar1 + 4 + *param_1),
                   &PTR_LAB_00098278);
        iVar1 = extraout_ECX + 0xc;
      } while (iVar2 < param_1[2]);
    }
    if (*param_1 != 0) {
      (*(code *)**(undefined4 **)(*param_1 + 8))();
    }
    *param_1 = 0;
    param_1[2] = 0;
    param_1[3] = 0;
  }
  return;
}

