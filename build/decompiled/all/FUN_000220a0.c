
void __regparm3 FUN_000220a0(int *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined4 auStack_38 [3];
  undefined1 local_2c;
  undefined4 local_2b;
  undefined4 local_27;
  int local_20;
  int local_1c;
  
  bVar6 = 0;
  if (*param_1 != 0) {
    iVar3 = 0;
    if (0 < param_1[2]) {
      local_1c = 0;
      do {
        local_20 = param_1[4];
        puVar2 = (undefined4 *)(*param_1 + local_1c);
        local_2c = *(undefined1 *)(puVar2 + 3);
        puVar4 = puVar2;
        puVar5 = auStack_38;
        for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
          *puVar5 = *puVar4;
          puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
          puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
        }
        for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
          *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
          puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
          puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
        }
        local_2b = *(undefined4 *)((int)puVar2 + 0xd);
        local_27 = *(undefined4 *)((int)puVar2 + 0x11);
        (**(code **)(local_20 + 4))();
        iVar3 = iVar3 + 1;
        local_1c = local_1c + 0x15;
      } while (iVar3 < param_1[2]);
    }
    thunk_FUN_0005c370();
    *param_1 = 0;
    param_1[2] = 0;
    param_1[3] = 0;
  }
  return;
}

