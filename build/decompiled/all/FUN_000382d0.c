
int __regparm3 FUN_000382d0(int param_1,int param_2)

{
  int iVar1;
  int unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if ((iVar1 != 0) && (*(int *)(param_1 + 0x5c) == 1)) {
    iVar1 = FUN_00052eb0();
    if ((iVar1 != 0) &&
       ((*(int *)(unaff_EBX + 0x5c) == 0x1f || (*(int *)(unaff_EBX + 0x5c) == 0x20)))) {
      *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
      (**(code **)(*(int *)(unaff_EBX + 8) + 0xd0))();
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

