
void __regparm3 FUN_00022010(int *param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  
  bVar7 = 0;
  iVar2 = param_1[3];
  param_1[3] = iVar2 + param_2;
  if (iVar2 + param_2 == 0) {
    thunk_FUN_0005c370();
    *param_1 = 0;
  }
  else {
    thunk_FUN_0005c320();
    puVar1 = (undefined4 *)FUN_00076ab0();
    iVar2 = 0;
    puVar4 = puVar1;
    if (0 < param_1[2]) {
      do {
        puVar5 = (undefined4 *)(iVar2 * 0x15 + *param_1);
        puVar6 = puVar4;
        for (iVar3 = 5; iVar3 != 0; iVar3 = iVar3 + -1) {
          *puVar6 = *puVar5;
          puVar5 = puVar5 + (uint)bVar7 * -2 + 1;
          puVar6 = puVar6 + (uint)bVar7 * -2 + 1;
        }
        *(undefined1 *)puVar6 = *(undefined1 *)puVar5;
        iVar2 = iVar2 + 1;
        puVar4 = (undefined4 *)((int)puVar4 + 0x15);
      } while (iVar2 < param_1[2]);
    }
    thunk_FUN_0005c370();
    *param_1 = (int)puVar1;
  }
  return;
}

