
void __regparm3 FUN_00054eb0(int param_1,int param_2)

{
  int iVar1;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar2;
  int unaff_EBX;
  
  (**(code **)(*(int *)(param_1 + 8) + 0x10))(0,0x1b,0,0);
  piVar2 = extraout_ECX;
  if ((*(byte *)((int)extraout_ECX + 0xb6) & 2) == 0) {
    iVar1 = (**(code **)(extraout_ECX[2] + 0xbc))();
    piVar2 = extraout_ECX_00;
    if ((iVar1 != 0) || ((*(byte *)((int)extraout_ECX_00 + 0xb6) & 1) != 0)) {
      *extraout_ECX_00 =
           (param_2 - *(int *)((int)extraout_ECX_00 + 0xd6)) + *(int *)((int)extraout_ECX_00 + 0x89)
      ;
      unaff_EBX = (unaff_EBX - *(int *)((int)extraout_ECX_00 + 0xda)) +
                  *(int *)((int)extraout_ECX_00 + 0x8d);
      goto LAB_00054f20;
    }
  }
  *piVar2 = param_2;
LAB_00054f20:
  piVar2[1] = unaff_EBX;
  (**(code **)(piVar2[2] + 0x54))();
  return;
}

