
int * __regparm3 FUN_00022190(int *param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  
  bVar6 = 0;
  param_1[4] = (int)&PTR_FUN_00097684;
  param_1[1] = param_2[1];
  param_1[2] = param_2[2];
  iVar1 = param_2[3];
  param_1[3] = iVar1;
  if (iVar1 == 0) {
    *param_1 = 0;
  }
  else {
    thunk_FUN_0005c320();
    iVar1 = FUN_00076ab0();
    iVar3 = 0;
    *param_1 = iVar1;
    if (0 < param_1[2]) {
      iVar1 = 0;
      do {
        puVar4 = (undefined4 *)(*param_2 + iVar1);
        puVar5 = (undefined4 *)(*param_1 + iVar1);
        for (iVar2 = 5; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar5 = *puVar4;
          puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
          puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
        }
        *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
        iVar3 = iVar3 + 1;
        iVar1 = iVar1 + 0x15;
      } while (iVar3 < param_1[2]);
    }
  }
  return param_1;
}

