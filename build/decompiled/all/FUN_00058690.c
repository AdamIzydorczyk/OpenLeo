
void __regparm3 FUN_00058690(int param_1,undefined4 *param_2)

{
  int *piVar1;
  int iVar2;
  int extraout_ECX;
  undefined4 *puVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  int local_1c;
  undefined4 *local_18;
  
  bVar7 = 0;
  piVar1 = (int *)(param_1 + 0xf);
  if (*(int *)(param_1 + 0xb) == 2) {
    puVar3 = (undefined4 *)(*piVar1 + *(int *)(param_1 + 0x2b) * 0x10);
    *param_2 = *puVar3;
    param_2[1] = puVar3[1];
    param_2[2] = puVar3[2];
    iVar4 = *(int *)(param_1 + 0x2f) + 1;
    *(int *)(param_1 + 0x2f) = iVar4;
    if (iVar4 == *(int *)(*piVar1 + *(int *)(param_1 + 0x2b) * 0x10 + 0xc)) {
      *(undefined4 *)(param_1 + 0x2f) = 0;
      *(int *)(param_1 + 0x2b) = *(int *)(param_1 + 0x2b) + 1;
    }
  }
  else {
    param_2[2] = 0;
    FUN_000582f0();
    FUN_00058010();
    iVar4 = *(int *)(extraout_ECX + 0xb);
    if (iVar4 == 1) {
      iVar2 = FUN_00057fe0();
      if (iVar2 == 0) {
        local_28 = *param_2;
        local_24 = param_2[1];
        local_20 = param_2[2];
        local_1c = iVar4;
        local_18 = &local_28;
        if (*(int *)(param_1 + 0x17) == *(int *)(param_1 + 0x1b)) {
          local_18 = &local_28;
          FUN_000225a0();
        }
        puVar5 = (undefined4 *)(*piVar1 + *(int *)(param_1 + 0x17) * 0x10);
        *(int *)(param_1 + 0x17) = *(int *)(param_1 + 0x17) + 1;
        puVar6 = puVar5 + (uint)bVar7 * -2 + 1;
        puVar3 = local_18 + (uint)bVar7 * -2 + 1;
        *puVar5 = *local_18;
        *puVar6 = *puVar3;
        puVar6[(uint)bVar7 * -2 + 1] = puVar3[(uint)bVar7 * -2 + 1];
        (puVar6 + (uint)bVar7 * -2 + 1)[(uint)bVar7 * -2 + 1] =
             (puVar3 + (uint)bVar7 * -2 + 1)[(uint)bVar7 * -2 + 1];
      }
      else {
        piVar1 = (int *)(*piVar1 + 0xc + (*(int *)(param_1 + 0x17) + -1) * 0x10);
        *piVar1 = *piVar1 + 1;
      }
    }
  }
  iVar4 = FUN_00058550();
  if (iVar4 != 0) {
    *(ushort *)(param_2 + 2) = *(ushort *)(param_2 + 2) | 0x808;
  }
  iVar4 = FUN_0001c530();
  if (iVar4 != 0) {
    *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 8;
  }
  return;
}

