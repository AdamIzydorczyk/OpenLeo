
undefined4 __regparm3 FUN_00053ae0(int param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint *unaff_EBX;
  uint *puVar3;
  byte bVar4;
  undefined1 auStackY_180c [2036];
  uint auStackY_1018 [1006];
  undefined4 local_50;
  undefined4 local_4c;
  uint local_48;
  undefined4 local_44;
  uint local_30;
  int local_20;
  int local_1c;
  
  bVar4 = 0;
  uVar1 = FUN_00053030();
  if ((local_20 == 0) && (local_1c == 0)) {
    return 0;
  }
  uVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x44))();
  if (((uVar2 & 0x400) != 0) &&
     (uVar2 = (**(code **)(*(int *)(param_2 + 8) + 0x44))(), (uVar2 & 0x10000) == 0)) {
    if ((extraout_ECX & 0x40400) == 0x40400) {
      if (5 < (int)((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f))) {
        FUN_00052b10();
      }
      FUN_00052b10();
    }
    else {
      FUN_000749a0();
      local_30 = local_30 | 1;
      puVar3 = unaff_EBX + (uint)bVar4 * -2 + 1;
      *unaff_EBX = local_30;
      *puVar3 = auStackY_1018[(uint)bVar4 * -2 + 0x3fb];
      puVar3[(uint)bVar4 * -2 + 1] = auStackY_1018[(uint)bVar4 * -2 + (uint)bVar4 * -2 + 0x3fc];
      *(char *)(puVar3 + (uint)bVar4 * -2 + 1 + (uint)bVar4 * -2 + 1) =
           (char)(auStackY_1018 + (uint)bVar4 * -2 + (uint)bVar4 * -2 + 0x3fc)[(uint)bVar4 * -2 + 1]
      ;
      if ((undefined1 *)(extraout_ECX_00 & 0x20400) == &LAB_00020400) {
        (**(code **)(*(int *)(param_2 + 8) + 0x10))(0,1,local_20,local_1c);
        if (local_1c != 0) {
          FUN_000749a0();
          FUN_00049580();
          return 1;
        }
      }
      else {
        local_50 = 0;
        local_4c = 1;
        local_44 = 0;
        local_48 = uVar1;
        (**(code **)(*(int *)(param_2 + 8) + 0x10))();
      }
    }
  }
  return 1;
}

