
void __regparm3 FUN_00054f40(int param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xbc))();
  iVar2 = extraout_ECX;
  if ((iVar1 == 0) && ((*(byte *)(extraout_ECX + 0xb6) & 1) == 0)) {
    iVar2 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x48))();
    iVar2 = *(int *)(iVar2 + 0x33);
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x50))();
    (**(code **)(iVar2 + 4))();
    iVar2 = extraout_ECX_01;
  }
  *(byte *)(iVar2 + 0xb6) = *(byte *)(iVar2 + 0xb6) | 1;
  return;
}

