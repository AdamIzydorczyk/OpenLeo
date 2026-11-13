
undefined4 __regparm3 FUN_0007a69c(int *param_1,undefined4 *param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint unaff_EBX;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined4 local_1c;
  undefined4 *local_18;
  uint local_14;
  uint local_10;
  
  bVar6 = 0;
  local_1c = 0;
  local_10 = *(int *)((int)param_1 + *(int *)(*param_1 + 4) + 0x1c) - unaff_EBX;
  local_18 = (undefined4 *)0x0;
  if (0 < (int)local_10) {
    local_18 = (undefined4 *)thunk_FUN_0005c320();
    if (local_18 == (undefined4 *)0x0) {
      local_10 = 0;
    }
    else {
      FUN_000749a0();
    }
  }
  iVar1 = *(int *)((int)param_1 + *(int *)(*param_1 + 4) + 4);
  uVar3 = *(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc);
  if ((uVar3 & 2) == 0) {
    uVar2 = local_14;
    if ((uVar3 & 8) == 0) {
      if (0 < (int)local_10) {
        local_14 = local_10;
        if ((int)local_10 < *(int *)(iVar1 + 0x1c) - *(int *)(iVar1 + 0x20)) {
          puVar4 = local_18;
          puVar5 = *(undefined4 **)(iVar1 + 0x20);
          for (uVar3 = local_10 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
            *puVar5 = *puVar4;
            puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
            puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
          }
          for (uVar3 = local_10 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
            *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
            puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
            puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
          }
          *(int *)(iVar1 + 0x20) = *(int *)(iVar1 + 0x20) + local_10;
          uVar2 = local_10;
        }
        else {
          uVar2 = (**(code **)(*(int *)(iVar1 + 0x28) + 4))();
        }
      }
      if (uVar2 != local_14) goto LAB_0007a983;
      if (*(int *)(iVar1 + 0x1c) - *(int *)(iVar1 + 0x20) <= (int)unaff_EBX) {
        uVar2 = (**(code **)(*(int *)(iVar1 + 0x28) + 4))();
        local_14 = unaff_EBX;
        goto LAB_0007a983;
      }
      puVar4 = *(undefined4 **)(iVar1 + 0x20);
      for (uVar3 = unaff_EBX >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar4 = *param_2;
        param_2 = param_2 + (uint)bVar6 * -2 + 1;
        puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
      }
      for (uVar3 = unaff_EBX & 3; local_14 = unaff_EBX, uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined1 *)puVar4 = *(undefined1 *)param_2;
        param_2 = (undefined4 *)((int)param_2 + (uint)bVar6 * -2 + 1);
        puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
      }
    }
    else {
      if (0 < (int)param_3) {
        local_14 = param_3;
        if ((int)param_3 < *(int *)(iVar1 + 0x1c) - *(int *)(iVar1 + 0x20)) {
          puVar4 = param_2;
          puVar5 = *(undefined4 **)(iVar1 + 0x20);
          for (uVar3 = param_3 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
            *puVar5 = *puVar4;
            puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
            puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
          }
          for (uVar3 = param_3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
            *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
            puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
            puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
          }
          *(int *)(iVar1 + 0x20) = *(int *)(iVar1 + 0x20) + param_3;
          uVar2 = param_3;
        }
        else {
          uVar2 = (**(code **)(*(int *)(iVar1 + 0x28) + 4))();
        }
      }
      if ((uVar2 == local_14) && (0 < (int)local_10)) {
        local_14 = local_10;
        if ((int)local_10 < *(int *)(iVar1 + 0x1c) - *(int *)(iVar1 + 0x20)) {
          puVar4 = local_18;
          puVar5 = *(undefined4 **)(iVar1 + 0x20);
          for (uVar3 = local_10 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
            *puVar5 = *puVar4;
            puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
            puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
          }
          for (uVar3 = local_10 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
            *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
            puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
            puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
          }
          *(int *)(iVar1 + 0x20) = *(int *)(iVar1 + 0x20) + local_10;
          uVar2 = local_10;
        }
        else {
          uVar2 = (**(code **)(*(int *)(iVar1 + 0x28) + 4))();
        }
      }
      if (uVar2 != local_14) goto LAB_0007a983;
      local_14 = unaff_EBX - param_3;
      if (*(int *)(iVar1 + 0x1c) - *(int *)(iVar1 + 0x20) <= (int)local_14) {
        uVar2 = (**(code **)(*(int *)(iVar1 + 0x28) + 4))();
        goto LAB_0007a983;
      }
      puVar4 = (undefined4 *)((int)param_2 + param_3);
      puVar5 = *(undefined4 **)(iVar1 + 0x20);
      for (uVar3 = local_14 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
        puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
      }
      for (uVar3 = local_14 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
        puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
        puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
      }
    }
  }
  else {
    if ((int)unaff_EBX < *(int *)(iVar1 + 0x1c) - *(int *)(iVar1 + 0x20)) {
      puVar4 = *(undefined4 **)(iVar1 + 0x20);
      for (uVar3 = unaff_EBX >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar4 = *param_2;
        param_2 = param_2 + (uint)bVar6 * -2 + 1;
        puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
      }
      for (uVar3 = unaff_EBX & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined1 *)puVar4 = *(undefined1 *)param_2;
        param_2 = (undefined4 *)((int)param_2 + (uint)bVar6 * -2 + 1);
        puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
      }
      *(int *)(iVar1 + 0x20) = *(int *)(iVar1 + 0x20) + unaff_EBX;
      uVar2 = unaff_EBX;
    }
    else {
      uVar2 = (**(code **)(*(int *)(iVar1 + 0x28) + 4))();
    }
    local_14 = unaff_EBX;
    if ((uVar2 != unaff_EBX) || ((int)local_10 < 1)) goto LAB_0007a983;
    local_14 = local_10;
    if (*(int *)(iVar1 + 0x1c) - *(int *)(iVar1 + 0x20) <= (int)local_10) {
      uVar2 = (**(code **)(*(int *)(iVar1 + 0x28) + 4))();
      goto LAB_0007a983;
    }
    puVar4 = local_18;
    puVar5 = *(undefined4 **)(iVar1 + 0x20);
    for (uVar3 = local_10 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
      puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
    }
    for (local_10 = local_10 & 3; local_10 != 0; local_10 = local_10 - 1) {
      *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
      puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
      puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
    }
  }
  *(int *)(iVar1 + 0x20) = *(int *)(iVar1 + 0x20) + local_14;
  uVar2 = local_14;
LAB_0007a983:
  if (uVar2 != local_14) {
    local_1c = 2;
  }
  if (local_18 != (undefined4 *)0x0) {
    thunk_FUN_0005c370();
  }
  *(undefined4 *)((int)param_1 + *(int *)(*param_1 + 4) + 0x1c) = 0;
  return local_1c;
}

