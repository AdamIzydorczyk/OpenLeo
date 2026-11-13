
void __regparm3 FUN_000563a0(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = *(int **)(param_1 + 0x5a);
  if (piVar2 == (int *)0x0) {
    return;
  }
  do {
    iVar1 = (**(code **)(piVar2[2] + 0x34))(param_2,param_3);
    if (iVar1 != 0) {
      (**(code **)(piVar2[2] + 0x24))();
    }
    piVar2 = (int *)*piVar2;
  } while (piVar2 != (int *)0x0);
  return;
}

