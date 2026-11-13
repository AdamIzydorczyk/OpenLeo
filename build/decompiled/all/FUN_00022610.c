
void __regparm3 FUN_00022610(int *param_1)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  
  if (*param_1 != 0) {
    if (0 < param_1[2]) {
      iVar2 = 0;
      do {
        iVar1 = *param_1;
        (**(code **)(param_1[4] + 4))
                  (*(undefined4 *)(iVar1 + iVar2),*(undefined4 *)(iVar1 + 4 + iVar2),
                   *(undefined4 *)(iVar1 + 8 + iVar2),*(undefined4 *)(iVar1 + 0xc + iVar2));
        iVar2 = iVar2 + 0x10;
      } while (extraout_ECX < param_1[2]);
    }
    thunk_FUN_0005c370();
    *param_1 = 0;
    param_1[2] = 0;
    param_1[3] = 0;
  }
  return;
}

