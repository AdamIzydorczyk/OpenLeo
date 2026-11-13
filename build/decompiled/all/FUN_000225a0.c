
void __regparm3 FUN_000225a0(int param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int *extraout_ECX;
  undefined4 *extraout_ECX_00;
  undefined4 *extraout_ECX_01;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  
  bVar7 = 0;
  param_2 = *(int *)(param_1 + 0xc) + param_2;
  *(int *)(param_1 + 0xc) = param_2;
  if (param_2 == 0) {
    thunk_FUN_0005c370();
    *extraout_ECX_01 = 0;
  }
  else {
    thunk_FUN_0005c320();
    puVar1 = (undefined4 *)FUN_00076ab0();
    iVar2 = 0;
    puVar3 = puVar1;
    if (0 < extraout_ECX[2]) {
      do {
        puVar4 = (undefined4 *)(*extraout_ECX + iVar2 * 0x10);
        puVar6 = puVar3 + (uint)bVar7 * -2 + 1;
        puVar5 = puVar4 + (uint)bVar7 * -2 + 1;
        *puVar3 = *puVar4;
        *puVar6 = *puVar5;
        puVar6[(uint)bVar7 * -2 + 1] = puVar5[(uint)bVar7 * -2 + 1];
        (puVar6 + (uint)bVar7 * -2 + 1)[(uint)bVar7 * -2 + 1] =
             (puVar5 + (uint)bVar7 * -2 + 1)[(uint)bVar7 * -2 + 1];
        iVar2 = iVar2 + 1;
        puVar3 = puVar3 + 4;
      } while (iVar2 < extraout_ECX[2]);
    }
    thunk_FUN_0005c370();
    *extraout_ECX_00 = puVar1;
  }
  return;
}

