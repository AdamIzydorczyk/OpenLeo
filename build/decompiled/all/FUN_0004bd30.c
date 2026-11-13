
undefined4 * __regparm3 FUN_0004bd30(undefined4 param_1,undefined4 *param_2,int param_3)

{
  int iVar1;
  undefined4 *puVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar3;
  int unaff_EBX;
  undefined4 *puVar4;
  byte bVar5;
  undefined1 auStack_1808 [2036];
  undefined4 auStack_1014 [1021];
  undefined1 local_20;
  undefined4 *local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  bVar5 = 0;
  local_20 = 0;
  local_1c = param_2;
  local_10 = param_3;
  FUN_000749a0();
  local_14 = *(int *)(unaff_EBX + 0x24);
  iVar3 = *(int *)(extraout_ECX + 100);
  do {
    if (iVar3 == 0) {
      (**(code **)(*(int *)(*(int *)(local_18 + 0x394) + 8) + 0x78))();
      return local_1c;
    }
    iVar1 = (**(code **)(*(int *)(iVar3 + 8) + 0x60))();
    iVar3 = extraout_ECX_00;
    if ((iVar1 != 0) &&
       (((local_14 == *(int *)(extraout_ECX_00 + 0x24) || (local_10 != 0)) &&
        (extraout_ECX_00 != unaff_EBX)))) {
      puVar2 = (undefined4 *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x78))();
      puVar4 = puVar2 + (uint)bVar5 * -2 + 1;
      auStack_1014[0x3fa] = *puVar2;
      auStack_1014[(uint)bVar5 * -2 + 0x3fb] = *puVar4;
      auStack_1014[(uint)bVar5 * -2 + (uint)bVar5 * -2 + 0x3fc] = puVar4[(uint)bVar5 * -2 + 1];
      *(undefined1 *)
       (auStack_1014 + (uint)bVar5 * -2 + (uint)bVar5 * -2 + 0x3fc + (uint)bVar5 * -2 + 1) =
           *(undefined1 *)(puVar4 + (uint)bVar5 * -2 + 1 + (uint)bVar5 * -2 + 1);
      iVar1 = FUN_000495b0();
      iVar3 = extraout_ECX_01;
      if (iVar1 == 0) {
        *(undefined1 *)(local_1c + 3) = local_20;
        puVar2 = auStack_1014 + 0x3fa;
        puVar4 = local_1c;
        for (iVar3 = 3; iVar3 != 0; iVar3 = iVar3 + -1) {
          *puVar4 = *puVar2;
          puVar2 = puVar2 + (uint)bVar5 * -2 + 1;
          puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
        }
        for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
          *(undefined1 *)puVar4 = *(undefined1 *)puVar2;
          puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar5 * -2 + 1);
          puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar5 * -2 + 1);
        }
        return local_1c;
      }
    }
    iVar3 = *(int *)(iVar3 + 0x34);
  } while( true );
}

