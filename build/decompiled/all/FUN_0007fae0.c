
int __regparm3 FUN_0007fae0(int param_1,undefined4 *param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint unaff_EBX;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  int local_18;
  undefined4 *local_14;
  
  bVar6 = 0;
  local_18 = 0;
  local_14 = param_2;
  do {
    if ((int)unaff_EBX < 1) {
      return local_18;
    }
    if (*(int *)(param_1 + 0x18) == 0) {
      iVar1 = (**(code **)(*(int *)(param_1 + 0x28) + 0xc))();
      if (iVar1 == -1) {
        return local_18;
      }
      uVar3 = 1;
    }
    else {
      uVar3 = *(int *)(param_1 + 0x1c) - *(int *)(param_1 + 0x20);
      if ((int)uVar3 < 1) {
        iVar1 = (**(code **)(*(int *)(param_1 + 0x28) + 0xc))();
        if (iVar1 == -1) {
          return local_18;
        }
        uVar3 = *(int *)(param_1 + 0x1c) - *(int *)(param_1 + 0x20);
      }
      if ((int)unaff_EBX < (int)uVar3) {
        uVar3 = unaff_EBX;
      }
      puVar4 = local_14;
      puVar5 = *(undefined4 **)(param_1 + 0x20);
      for (uVar2 = uVar3 >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
        puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
      }
      for (uVar2 = uVar3 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
        *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
        puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
        puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
      }
      *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x20) + uVar3;
    }
    local_18 = local_18 + uVar3;
    local_14 = (undefined4 *)((int)local_14 + uVar3);
    unaff_EBX = unaff_EBX - uVar3;
  } while( true );
}

