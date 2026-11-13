
undefined4 * __regparm3 FUN_0003e980(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  bool bVar6;
  int iVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int unaff_EBX;
  int iVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  byte bVar14;
  undefined4 local_24;
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar14 = 0;
  local_18 = 0;
  local_14 = param_2;
  FUN_000749a0();
  iVar7 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if ((iVar7 != 0) && (iVar7 = (**(code **)(*(int *)(extraout_ECX + 8) + 0xb0))(), iVar7 != 0)) {
    piVar8 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
    iVar7 = *piVar8;
    iVar10 = piVar8[1];
    iVar11 = piVar8[2];
    iVar1 = piVar8[3];
    piVar8 = (int *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x4c))();
    iVar2 = *piVar8;
    iVar3 = piVar8[1];
    iVar4 = piVar8[2];
    iVar5 = piVar8[3];
    FUN_0004a050();
    FUN_0004a050();
    iVar9 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x70))();
    iVar9 = iVar9 * *(int *)(extraout_ECX_01 + 0xa9);
    iVar7 = iVar7 + iVar9;
    iVar11 = iVar11 + iVar9;
    if ((iVar1 < iVar3) || (((iVar5 < iVar10 || (iVar11 < iVar2)) || (iVar4 < iVar7)))) {
      bVar6 = false;
    }
    else {
      bVar6 = true;
    }
    if (bVar6) {
      iVar9 = *(int *)(DAT_000cef40 + 0x20);
      *(int *)(DAT_000cef40 + 0x20) = 0;
      if (iVar9 == 0) {
        FUN_0004a050();
        if (((iVar1 < iVar3) || (iVar5 < iVar10)) || ((iVar11 < iVar2 || (iVar4 < iVar7)))) {
          bVar6 = false;
        }
        else {
          bVar6 = true;
        }
        if (bVar6) {
          local_24._1_1_ = local_24._1_1_ | 0x80;
        }
      }
      else {
        *(byte *)(extraout_ECX_01 + 0x66) = *(byte *)(extraout_ECX_01 + 0x66) | 0x40;
        if (*(int *)(extraout_ECX_01 + 0x30) != 0) {
          for (iVar7 = 0;
              (iVar10 = FUN_00055760(), iVar10 != extraout_ECX_02 &&
              (iVar7 < *(int *)(*(int *)(extraout_ECX_02 + 0x30) + 0x3c))); iVar7 = iVar7 + 1) {
            iVar10 = FUN_00055760();
            (**(code **)(*(int *)(iVar10 + 8) + 0x10))();
          }
        }
      }
    }
  }
  *(undefined1 *)(local_14 + 3) = local_18;
  puVar12 = &local_24;
  puVar13 = local_14;
  for (iVar7 = 3; iVar7 != 0; iVar7 = iVar7 + -1) {
    *puVar13 = *puVar12;
    puVar12 = puVar12 + (uint)bVar14 * -2 + 1;
    puVar13 = puVar13 + (uint)bVar14 * -2 + 1;
  }
  for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
    *(undefined1 *)puVar13 = *(undefined1 *)puVar12;
    puVar12 = (undefined4 *)((int)puVar12 + (uint)bVar14 * -2 + 1);
    puVar13 = (undefined4 *)((int)puVar13 + (uint)bVar14 * -2 + 1);
  }
  return local_14;
}

