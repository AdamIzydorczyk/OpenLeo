
undefined8 FUN_00015550(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  undefined4 *puVar2;
  undefined2 extraout_var;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined1 local_c [4];
  undefined1 local_8 [4];
  
  bVar6 = 0;
  if (DAT_000afb84 == (undefined4 *)0x0) {
    ll_printf(local_c,s_unibuf_NULL__000902f8);
    FUN_00075b10();
  }
  if (DAT_000afb88 <= DAT_000afb8c) {
    ll_printf(local_8,s_unitt__d_out_of_unimax__d_00090305,DAT_000afb8c,DAT_000afb88);
    FUN_00075b10();
  }
  *(undefined1 *)((uint)DAT_000afb8c + (int)DAT_000afb84) = 0;
  puVar2 = (undefined4 *)ll_alloc_try();
  uVar1 = DAT_000afb8a;
  puVar4 = DAT_000afb84;
  puVar5 = puVar2;
  for (uVar3 = CONCAT22(extraout_var,DAT_000afb8a) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
    puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
  }
  for (uVar3 = (byte)uVar1 & 0xffffff03; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
    puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
    puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
  }
  return CONCAT44(param_2,puVar2);
}

