
undefined8 __regparm3 FUN_00079284(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  int *piVar2;
  undefined3 uVar3;
  undefined4 uVar4;
  int iVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
  ushort in_DS;
  undefined8 uVar9;
  uint local_18;
  
  if (((DAT_000a9bee == 1) && (DAT_000a9bef == '\0')) || (DAT_000a9bee == 9)) {
    uVar4 = FUN_000791d4();
    goto LAB_000793c7;
  }
  if ((DAT_000a9fa8 == 0) || (DAT_000a9bc0 == -2)) goto LAB_000792c1;
  iVar5 = FUN_000793d0();
  uVar4 = 0;
  if (iVar5 == 0) goto LAB_000793c7;
  if (((1 < DAT_000a9bee) && (DAT_000a9bee < 9)) && (DAT_000a9bf0 == '\0')) {
    uVar3 = SegmentLimit((uint)in_DS);
    DAT_000a9bc0 = (ushort)uVar3 + 1;
  }
  uVar9 = crt_dpmi_alloc();
  puVar8 = (uint *)((ulonglong)uVar9 >> 0x20);
  puVar6 = (uint *)uVar9;
  if (((puVar6 != (uint *)0xffffffff) && (puVar6 < (uint *)0xfffffff9)) && (puVar6 < puVar8)) {
    local_18 = (int)puVar8 - (int)puVar6;
    uVar1 = local_18 - 4;
    if (uVar1 <= local_18) {
      for (puVar8 = DAT_000a9b58;
          ((puVar8 != (uint *)0x0 && ((uint *)puVar8[2] != (uint *)0x0)) &&
          ((puVar6 < puVar8 || ((uint *)puVar8[2] <= puVar6)))); puVar8 = (uint *)puVar8[2]) {
      }
      if (puVar8 == (uint *)0x0) {
LAB_0007938a:
        if (uVar1 < 0x38) goto LAB_000792c1;
        *puVar6 = uVar1;
        uVar9 = FUN_0007903f();
        local_18 = *(uint *)uVar9;
      }
      else {
        puVar7 = (uint *)(*puVar8 + (int)puVar8);
        uVar9 = CONCAT44(puVar8,puVar7);
        if (puVar6 + -1 != puVar7) goto LAB_0007938a;
        *puVar8 = *puVar8 + local_18;
        *(undefined4 *)((int)puVar7 + local_18) = 0xffffffff;
      }
      iVar5 = (int)((ulonglong)uVar9 >> 0x20);
      *(uint *)uVar9 = local_18 | 1;
      piVar2 = (int *)(iVar5 + 0x18);
      *piVar2 = *piVar2 + 1;
      *(undefined4 *)(iVar5 + 0x14) = 0xffffffff;
      FUN_00075302();
      uVar4 = 1;
      goto LAB_000793c7;
    }
  }
LAB_000792c1:
  uVar4 = 0;
LAB_000793c7:
  return CONCAT44(param_2,uVar4);
}

