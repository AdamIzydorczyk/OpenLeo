
void __regparm3 FUN_00034495(int *param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int aiStack_20 [5];
  int *local_c;
  int *local_8;
  
  aiStack_20[3] = 0;
  aiStack_20[4] = 0;
  local_c = param_1;
  aiStack_20[2] = (**(code **)(param_1[2] + 0x70))();
  aiStack_20[2] = aiStack_20[2] * 10;
  (**(code **)(local_c[2] + 0xcc))();
  iVar1 = (**(code **)(local_c[2] + 0x70))();
  if ((iVar1 < 1) || (*local_c <= *(int *)((int)local_c + 0x126))) {
    iVar1 = (**(code **)(local_c[2] + 0x70))();
    if ((-1 < iVar1) || (*(int *)((int)local_c + 0x122) <= *local_c)) goto LAB_00034516;
  }
  FUN_00053670();
LAB_00034516:
  piVar5 = aiStack_20 + 2;
  piVar4 = aiStack_20 + 2;
  if (*(int *)((int)local_c + 0x12e) != 0) {
    piVar5 = aiStack_20 + 2;
    if (local_c[0x17] == 5) {
      local_8 = (int *)((int)local_c + 0x136);
      iVar1 = 0;
      for (iVar6 = 0; piVar5 = piVar4, iVar6 < local_8[2]; iVar6 = iVar6 + 1) {
        uVar2 = *(int *)(iVar1 + 4 + *local_8) - *local_c;
        uVar3 = (int)uVar2 >> 0x1f;
        if ((int)((uVar2 ^ uVar3) - uVar3) < 5) {
          *(undefined4 *)((int)piVar4 + -4) = 0;
          *(undefined4 *)((int)piVar4 + -8) = 0x34581;
          FUN_0004bc40();
          piVar4 = (int *)((int)piVar4 + -4);
        }
        iVar1 = iVar1 + 8;
      }
    }
    uVar2 = *local_c - *(int *)((int)local_c + 0x12a);
    uVar3 = (int)uVar2 >> 0x1f;
    if ((int)((uVar2 ^ uVar3) - uVar3) < 5) {
      if (*(int *)((int)local_c + 0x132) == 0) {
        *(byte *)((int)local_c + 0x67) = *(byte *)((int)local_c + 0x67) | 1;
      }
      else {
        *(byte *)((int)local_c + 0x66) = *(byte *)((int)local_c + 0x66) | 0x40;
        *(undefined4 *)((int)local_c + 0x132) = 0;
      }
    }
  }
  *(undefined4 *)((int)piVar5 + -4) = 0x345cd;
  FUN_000550a0();
  return;
}

