
void __regparm3 FUN_0004a350(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  undefined **ppuVar2;
  
  ppuVar2 = &PTR_FUN_000a0000;
  for (iVar1 = 16000; iVar1 != 0; iVar1 = iVar1 + -1) {
    *ppuVar2 = (undefined *)*param_2;
    param_2 = param_2 + 1;
    ppuVar2 = ppuVar2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)ppuVar2 = *(undefined1 *)param_2;
    param_2 = (undefined4 *)((int)param_2 + 1);
    ppuVar2 = (undefined **)((int)ppuVar2 + 1);
  }
  return;
}

