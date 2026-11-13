
void __regparm3 FUN_0007519c(undefined1 *param_1,undefined1 *param_2)

{
  undefined1 uVar1;
  int unaff_EBX;
  int iVar2;
  
  for (iVar2 = unaff_EBX >> 1; iVar2 != 0; iVar2 = iVar2 + -1) {
    uVar1 = *param_1;
    *param_2 = param_1[1];
    param_1 = param_1 + 2;
    param_2[1] = uVar1;
    param_2 = param_2 + 2;
  }
  return;
}

