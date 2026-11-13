
undefined4 __regparm3 FUN_0001b800(int param_1,int param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint extraout_ECX;
  uint unaff_EBX;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined8 uVar7;
  int local_80 [12];
  undefined4 local_50;
  uint local_4c;
  uint local_48;
  uint local_40;
  undefined1 *local_3c;
  int local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 *local_28;
  undefined1 local_24 [8];
  int local_1c;
  int local_18;
  int local_14;
  char local_10;
  
  bVar6 = 0;
  *(undefined4 *)(param_1 + 0x150) = 0;
  local_14 = 0;
  local_1c = 0;
  local_18 = param_2;
  if ((param_3 == 0) || (unaff_EBX == 0)) {
    local_50 = 0x500;
    local_3c = (undefined1 *)local_80;
    FUN_00075c3d();
    *(int *)(param_1 + 0xa4) = local_80[0];
    uVar3 = local_80[0] - 0x7d000;
    *(uint *)(param_1 + 0xa4) = uVar3;
    if (uVar3 < 0x7a1201) {
      if (uVar3 < 500000) {
        DAT_000cde50 = 2;
        return 0x1945;
      }
    }
    else {
      *(uint *)(param_1 + 0xa4) = uVar3 >> 1;
    }
    uVar1 = DAT_000cde34;
    for (uVar3 = *(uint *)(param_1 + 0xa4); 499999 < uVar3; uVar3 = uVar3 - 50000) {
      uVar7 = crt_malloc();
      uVar3 = (uint)((ulonglong)uVar7 >> 0x20);
      DAT_000cde34 = (uint)uVar7;
      if (DAT_000cde34 != 0) {
        *(uint *)(param_1 + 0xa4) = uVar3;
        uVar1 = DAT_000cde34;
        break;
      }
      uVar1 = DAT_000cde34;
      DAT_000cde34 = 0;
    }
    DAT_000cde34 = uVar1;
    if (uVar3 < 500000) {
      DAT_000cde50 = 2;
      return 0x1945;
    }
    local_4c = DAT_000cde34 >> 0x10;
    local_48 = DAT_000cde34 & 0xffff;
    local_50 = 0x600;
    local_40 = *(uint *)(param_1 + 0xa4) >> 0x10;
    local_3c = (undefined1 *)(*(uint *)(param_1 + 0xa4) & 0xffff);
    FUN_00075c3d();
  }
  else {
    DAT_000cde34 = unaff_EBX;
    *(int *)(param_1 + 0xa4) = param_3;
    local_1c = 1;
  }
  *(undefined4 *)(param_1 + 0xa8) = 0;
  *(undefined4 *)(param_1 + 0xac) = 0;
  DAT_000cde30 = *(uint *)(param_1 + 0xa8);
  *(undefined4 *)(param_1 + 0xb0) = 0;
  DAT_000cde1c = 0;
  DAT_000cde18 = 0;
  local_34 = 0;
  local_30 = 1000;
  local_2c = 0;
  local_28 = &DAT_000cde18;
  if ((*(byte *)(param_1 + 0x154) & 1) == 0) {
    FUN_0007612f();
    *(int *)(param_1 + 0xa8) = *(int *)(param_1 + 0xa8) + 7;
    FUN_0007612f();
    *(int *)(param_1 + 0xa8) = *(int *)(param_1 + 0xa8) + *(int *)(DAT_000cde34 + 1) + -7;
    FUN_0001b7b0();
    *(undefined4 *)(param_1 + 0x150) = 1;
  }
  else {
    FUN_00076527();
    FUN_000765ab();
    FUN_00076527();
    FUN_000765ab();
    if (*(uint *)(param_1 + 0xa8) < *(uint *)(param_1 + 0xa4)) {
      while (iVar2 = FUN_0007612f(), iVar2 != 0) {
        FUN_000765ab();
        if ((uint)(*(int *)(param_1 + 0xa4) - *(int *)(param_1 + 0xa8)) < (uint)local_24._1_4_)
        goto LAB_0001bad6;
        FUN_0007612f();
        FUN_0001b7b0();
        *(int *)(param_1 + 0xa8) = *(int *)(param_1 + 0xa8) + local_24._1_4_;
        *(int *)(param_1 + 0x150) = *(int *)(param_1 + 0x150) + 1;
        iVar2 = FUN_000766a7();
        if ((iVar2 != 0) || (*(uint *)(param_1 + 0xa4) <= *(uint *)(param_1 + 0xa8)))
        goto LAB_0001bad6;
      }
      local_14 = 1;
    }
LAB_0001bad6:
    if ((*(byte *)(*(int *)(param_1 + 0x90) + 0xc) & 0x10) != 0) {
      FUN_000765ab();
    }
  }
  if (*(int *)(param_1 + 0x9c) == 0) {
    FUN_0001a450();
  }
  else {
    FUN_0001a840();
    FUN_0001a980();
  }
  FUN_0001b790();
  local_30 = 0x96;
  do {
    if (*(uint *)(param_1 + 0xa8) < DAT_000cde30) {
      uVar3 = DAT_000cde30 - *(int *)(param_1 + 0xa8);
    }
    else {
      uVar3 = *(int *)(param_1 + 0xa4) - *(int *)(param_1 + 0xa8);
      if (uVar3 <= DAT_000cde30) {
        uVar3 = DAT_000cde30;
      }
    }
    iVar2 = FUN_0007612f();
    if (iVar2 == 0) {
      FUN_000765ab();
      FUN_0007612f();
      local_14 = 1;
    }
    if ((uint)local_24._1_4_ < uVar3) {
      *(undefined4 *)(param_1 + 0xb0) = 0;
      *(uint *)(param_1 + 0xac) = local_24._1_4_ - 7;
      if ((uint)(*(int *)(param_1 + 0xa4) - *(int *)(param_1 + 0xa8)) < (uint)local_24._1_4_) {
        *(undefined4 *)(param_1 + 0xa8) = 0;
      }
      FUN_0001b7b0();
      puVar4 = (undefined4 *)local_24;
      puVar5 = (undefined4 *)(DAT_000cde34 + *(int *)(param_1 + 0xa8));
      for (uVar3 = extraout_ECX >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
        puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
      }
      for (uVar3 = extraout_ECX & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
        puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
        puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
      }
      *(int *)(param_1 + 0xa8) = *(int *)(param_1 + 0xa8) + 7;
      FUN_0001c020();
      *(int *)(param_1 + 0x150) = *(int *)(param_1 + 0x150) + 1;
    }
    else {
      FUN_000765ab();
      if (*(int *)(param_1 + 0x94) != 0) {
        FUN_0001bee0();
      }
    }
    iVar2 = FUN_000766a7();
    if (iVar2 != 0) {
      local_10 = FUN_000766be();
      if ((local_10 == '\t') || (local_10 == '\x1b')) goto LAB_0001bd7e;
      if (DAT_000cde2c != 0) {
        DAT_000cde2c = 0;
        DAT_000c410c = 1;
      }
    }
    if ((local_18 == 0) &&
       (((uint)*(ushort *)(*(int *)(param_1 + 0xb8) + 0x57) == *(uint *)(param_1 + 0x150) ||
        (local_14 != 0)))) {
      local_14 = 1;
      goto LAB_0001bd7e;
    }
    iVar2 = FUN_0001c130();
  } while (iVar2 == 0);
  local_10 = '\x1b';
  DAT_000cde50 = 0xc;
LAB_0001bd7e:
  if ((local_18 == 0) && (local_14 != 0)) {
    local_34 = local_18;
    local_30 = 1500000;
    do {
      if (DAT_000cde1c - DAT_000cde18 == 1) goto LAB_0001be1f;
      iVar2 = FUN_000766a7();
      if (iVar2 != 0) {
        uVar7 = FUN_000766be();
        iVar2 = (int)((ulonglong)uVar7 >> 0x20);
        local_10 = (char)uVar7;
        uVar3 = (uint)uVar7 & 0xff;
        if ((uVar3 == 9) || (uVar3 == 0x1b)) {
          FUN_0001b7a0();
          goto LAB_0001be82;
        }
        if (iVar2 != DAT_000cde2c) {
          DAT_000c410c = 1;
          DAT_000cde2c = iVar2;
        }
      }
      iVar2 = FUN_0001c130();
    } while (iVar2 == 0);
    local_10 = '\x1b';
    DAT_000cde50 = 0xc;
LAB_0001be1f:
    do {
      if (DAT_000cde1c - DAT_000cde18 == 1) goto LAB_0001be82;
      iVar2 = FUN_000766a7();
      if (iVar2 != 0) {
        uVar7 = FUN_000766be();
        iVar2 = (int)((ulonglong)uVar7 >> 0x20);
        local_10 = (char)uVar7;
        uVar3 = (uint)uVar7 & 0xff;
        if ((uVar3 == 9) || (uVar3 == 0x1b)) goto LAB_0001be82;
        if (iVar2 != DAT_000cde2c) {
          DAT_000c410c = 1;
          DAT_000cde2c = iVar2;
        }
      }
      iVar2 = FUN_0001c130();
    } while (iVar2 == 0);
    local_10 = '\x1b';
    DAT_000cde50 = 0xc;
  }
LAB_0001be82:
  FUN_0001b7a0();
  if (*(int *)(param_1 + 0x9c) == 0) {
    FUN_0001a590();
  }
  else {
    FUN_0001a8b0();
  }
  if ((*(byte *)(param_1 + 0x154) & 8) != 0) {
    FUN_0001b700();
  }
  if (local_1c == 0) {
    FUN_00075302();
  }
  if (local_10 == '\x1b') {
    return 0x1f1f;
  }
  return 0;
}

