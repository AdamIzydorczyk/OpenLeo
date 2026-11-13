
void __regparm3 FUN_000561a0(int param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  
  iVar3 = 0;
  for (iVar2 = 0; iVar2 < *(int *)(param_1 + 0x4c); iVar2 = iVar2 + 1) {
    iVar1 = *(int *)(iVar3 + *(int *)(param_1 + 0x44));
    if (iVar1 != 0) {
      (*(code *)**(undefined4 **)(iVar1 + 0x18))();
      iVar2 = extraout_ECX;
    }
    iVar3 = iVar3 + 4;
  }
  FUN_0001cd10();
  return;
}

