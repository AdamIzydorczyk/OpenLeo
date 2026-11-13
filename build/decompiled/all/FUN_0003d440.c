
undefined4 __regparm3 FUN_0003d440(int param_1,undefined4 param_2)

{
  code *pcVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  undefined4 uVar5;
  int unaff_EBX;
  undefined4 *puVar6;
  undefined4 *puVar7;
  byte bVar8;
  undefined8 uVar9;
  int aiStack_60 [4];
  int local_50;
  int local_4c;
  int local_48;
  undefined **local_44;
  byte local_2f;
  undefined1 auStack_2c [12];
  int local_20;
  int local_1c;
  undefined **local_18;
  undefined4 local_14;
  
  bVar8 = 0;
  auStack_2c[8] = 0;
  aiStack_60[2] = 0x3d465;
  local_14 = param_2;
  FUN_000749a0();
  if (*(int *)(param_1 + 0x5c) == 1) {
    aiStack_60[2] = 0x3d485;
    iVar3 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
    piVar4 = aiStack_60 + 3;
    if (iVar3 != 0) {
      aiStack_60[2] = 0x3d495;
      piVar4 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
      local_18 = &PTR_LAB_00098278;
      local_20 = *piVar4 + *(int *)(param_1 + 0xf2);
      local_1c = piVar4[1] + *(int *)(param_1 + 0xf6);
      aiStack_60[2] = 0x3d4de;
      iVar3 = (**(code **)(*(int *)(param_1 + 8) + 0xdc))();
      if (iVar3 != 0) {
        *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
      }
      aiStack_60[2] = 0x3d4f4;
      iVar3 = (**(code **)(*(int *)(param_1 + 8) + 0xf0))();
      piVar4 = aiStack_60 + 3;
      if (*(int *)(*(int *)(DAT_000cef40 + 0x394) + 0xee) == iVar3) {
        aiStack_60[2] = 0x3d51f;
        iVar3 = (**(code **)(*(int *)(param_1 + 8) + 0xd8))();
        if (iVar3 == 0) {
          aiStack_60[2] = 0x3d535;
          piVar4 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
          aiStack_60[3] = *piVar4;
          local_50 = piVar4[1];
          local_4c = piVar4[2];
          local_48 = piVar4[3];
          local_44 = &PTR_FUN_00098274;
          aiStack_60[2] = 0x3d566;
          FUN_0004a050();
          aiStack_60[2] = 0x3d572;
          FUN_0004a080();
          if ((((local_20 < aiStack_60[3]) || (local_4c < local_20)) || (local_1c < local_50)) ||
             (local_48 < local_1c)) {
            bVar2 = false;
          }
          else {
            bVar2 = true;
          }
          piVar4 = aiStack_60 + 3;
          if ((bVar2) && (piVar4 = aiStack_60 + 3, (*(byte *)(unaff_EBX + 100) & 0x80) != 0)) {
            *(byte *)(unaff_EBX + 100) = *(byte *)(unaff_EBX + 100) & 0x7f;
            aiStack_60[2] = 0x3d5e2;
            uVar9 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
            if (*(int *)uVar9 < (int)((ulonglong)uVar9 >> 0x20)) {
              uVar5 = 1;
            }
            else {
              uVar5 = 0xffffffff;
            }
            *(undefined4 *)(param_1 + 0x106) = uVar5;
            aiStack_60[2] = 0x3d606;
            iVar3 = FUN_0005c320();
            piVar4 = aiStack_60 + 3;
            if (iVar3 != 0) {
              aiStack_60[2] = unaff_EBX + 0xd2;
              FUN_00053f00();
              piVar4 = aiStack_60 + 2;
            }
            pcVar1 = *(code **)(*(int *)(param_1 + 8) + 0xe4);
            *(undefined4 *)((int)piVar4 + -4) = 0x3d62a;
            (*pcVar1)();
            *(undefined1 *)((int)piVar4 + 0x20) = 0;
            *(undefined4 *)((int)piVar4 + -4) = 0x3d640;
            FUN_000749a0();
            *(byte *)((int)piVar4 + 0x18) = *(byte *)((int)piVar4 + 0x18) | 0x80;
            *(undefined4 *)((int)piVar4 + -4) = 0x3d659;
            iVar3 = FUN_0005c320();
            if (iVar3 != 0) {
              *(int *)((int)piVar4 + -4) = unaff_EBX + 0xd2;
              *(undefined4 *)((int)piVar4 + -8) = 0x3d672;
              FUN_00053f50();
              piVar4 = (int *)((int)piVar4 + -4);
            }
            *(undefined4 *)(*(int *)(DAT_000cef40 + 0x394) + 0x106) = 0;
          }
        }
        else {
          local_2f = local_2f | 0x40;
          piVar4 = aiStack_60 + 3;
        }
      }
    }
    puVar7 = *(undefined4 **)((int)piVar4 + 0x40);
    *(undefined1 *)(puVar7 + 3) = *(undefined1 *)((int)piVar4 + 0x30);
    *(undefined4 **)((int)piVar4 + -4) = puVar7;
    puVar6 = (undefined4 *)((int)piVar4 + 0x24);
    for (iVar3 = 3; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar7 = *puVar6;
      puVar6 = puVar6 + (uint)bVar8 * -2 + 1;
      puVar7 = puVar7 + (uint)bVar8 * -2 + 1;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined1 *)puVar7 = *(undefined1 *)puVar6;
      puVar6 = (undefined4 *)((int)puVar6 + (uint)bVar8 * -2 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + (uint)bVar8 * -2 + 1);
    }
  }
  else {
    aiStack_60[2] = 0x3d478;
    FUN_000552e0();
    piVar4 = aiStack_60 + 3;
  }
  return *(undefined4 *)((int)piVar4 + 0x40);
}

