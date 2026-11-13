
int __regparm3 FUN_000349d0(int *param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  int *unaff_EBX;
  
  iVar1 = (**(code **)(unaff_EBX[2] + 0xc))();
  if ((((iVar1 == DAT_000ced00) &&
       (uVar2 = *param_1 - *unaff_EBX >> 0x1f,
       (int)((*param_1 - *unaff_EBX ^ uVar2) - uVar2) < *(int *)((int)param_1 + 0xf2))) &&
      (uVar2 = param_1[1] - unaff_EBX[1] >> 0x1f,
      (int)((param_1[1] - unaff_EBX[1] ^ uVar2) - uVar2) < 0x1e)) && (unaff_EBX[0x17] == 2)) {
    (**(code **)(param_1[2] + 0xc))(*(undefined4 *)((int)param_1 + 0xee));
    FUN_0004bc40();
    (**(code **)(param_1[2] + 0xc))(*(undefined4 *)((int)param_1 + 0xee));
    FUN_0004bc40();
    iVar1 = *(int *)((int)param_1 + 0xee);
    (**(code **)(param_1[2] + 0xc))();
    FUN_0004bc40();
    *(undefined1 *)(iVar1 + 0xc) = 0;
    FUN_000749a0();
    return iVar1;
  }
  FUN_000552e0();
  return param_2;
}

