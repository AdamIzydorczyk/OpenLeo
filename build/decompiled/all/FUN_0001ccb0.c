
void __regparm3 FUN_0001ccb0(int *param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int extraout_ECX;
  undefined4 *puVar3;
  
  iVar2 = param_1[3];
  param_1[3] = iVar2 + param_2;
  if (iVar2 + param_2 == 0) {
    thunk_FUN_0005c370();
    *param_1 = extraout_ECX;
    return;
  }
  puVar1 = (undefined4 *)thunk_FUN_0005c320();
  iVar2 = 0;
  puVar3 = puVar1;
  if (0 < param_1[2]) {
    do {
      *puVar3 = *(undefined4 *)(*param_1 + iVar2 * 4);
      iVar2 = iVar2 + 1;
      puVar3 = puVar3 + 1;
    } while (iVar2 < param_1[2]);
  }
  thunk_FUN_0005c370();
  *param_1 = (int)puVar1;
  return;
}

