
void __regparm3 FUN_0007571d(int *param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int extraout_ECX;
  undefined1 local_18;
  
  piVar1 = (int *)param_1[2];
  iVar2 = piVar1[2];
  iVar3 = *param_1;
  if (iVar2 != 0) {
    local_18 = *(undefined1 *)(iVar2 + 0x1c);
    *(undefined1 *)(iVar2 + 0x1c) = 0;
  }
  while (iVar3 != *piVar1) {
    FUN_00074b7c();
    *piVar1 = *(int *)*piVar1;
    iVar3 = extraout_ECX;
  }
  if (iVar3 != 0) {
    FUN_00074b7c();
  }
  if (iVar2 != 0) {
    *(undefined1 *)(iVar2 + 0x1c) = local_18;
  }
  return;
}

