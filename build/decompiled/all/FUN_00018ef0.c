
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_00018ef0(undefined4 param_1,undefined4 param_2)

{
  ushort *puVar1;
  uint extraout_ECX;
  uint uVar2;
  int in_GS_OFFSET;
  undefined4 local_90 [6];
  uint local_78;
  undefined4 local_74;
  undefined2 local_6e;
  undefined2 local_6c;
  uint local_5c;
  undefined4 local_58;
  undefined1 *local_48;
  undefined2 local_40;
  undefined2 uStack_3e;
  undefined2 local_3c;
  uint local_34;
  int local_28;
  undefined1 local_18;
  
  local_18 = 0;
  FUN_000749a0();
  _local_40 = CONCAT22(uStack_3e,0x100);
  local_3c = 0x10;
  local_5c = FUN_00075c3d();
  uVar2 = 0;
  if (local_28 == 0) {
    DAT_000c403c = (undefined2)_local_40;
    _DAT_000c403e = (undefined2)local_34;
    local_5c = local_34;
    uVar2 = local_34;
  }
  DAT_000c4048._4_2_ = (short)uVar2;
  DAT_000c4048._0_4_ = 0;
  if (DAT_000c4048._4_2_ != 0) {
    FUN_000749a0();
    local_6e = DAT_000c403c;
    local_6c = DAT_000c403c;
    local_90[0] = 0;
    local_74 = 0x4f01;
    local_78 = extraout_ECX & 0xffff;
    FUN_000749a0();
    FUN_000749a0();
    FUN_00075cb9();
    local_48 = (undefined1 *)local_90;
    local_5c = 0x300;
    local_58 = 0x10;
    FUN_00075ce1();
    local_5c = local_5c & 0xff;
    if (local_5c == 0) {
      DAT_000c4050 = (uint)*(ushort *)(in_GS_OFFSET + 4);
      _DAT_000c4058 = (uint)*(ushort *)(in_GS_OFFSET + 0x12);
      _DAT_000c405c = (uint)*(ushort *)(in_GS_OFFSET + 0x14);
      puVar1 = (ushort *)(*(byte *)(in_GS_OFFSET + 0x19) + 0x40);
      *puVar1 = *puVar1 + (ushort)(0 < (short)-(*puVar1 & 3)) * -(*puVar1 & 3);
      local_5c = 0;
      local_18 = 1;
    }
  }
  return CONCAT44(param_2,CONCAT31((int3)(local_5c >> 8),local_18));
}

