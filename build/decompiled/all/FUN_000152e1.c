
void __regparm3 FUN_000152e1(undefined1 param_1)

{
  ushort uVar1;
  undefined4 *puVar2;
  undefined2 extraout_var;
  uint uVar3;
  undefined4 *extraout_EDX;
  undefined4 *puVar4;
  byte bVar5;
  undefined1 local_c [4];
  undefined1 local_8 [4];
  
  bVar5 = 0;
  if (DAT_000afb84 == (undefined4 *)0x0) {
    ll_printf(local_c,s_unibuf_NULL__000902d0);
    FUN_00075b10();
  }
  if (DAT_000afb88 <= DAT_000afb8a) {
    ll_printf(local_8,s_unipc__d__out_of_unimax__d_000902dd,DAT_000afb8a,DAT_000afb88);
    FUN_00075b10();
  }
  uVar3 = (uint)DAT_000afb8a;
  DAT_000afb8a = DAT_000afb8a + 1;
  *(undefined1 *)((int)DAT_000afb84 + uVar3) = param_1;
  if ((int)(DAT_000afb88 - 0x10) < (int)(uint)DAT_000afb8a) {
    puVar2 = (undefined4 *)ll_alloc_try();
    uVar1 = DAT_000afb88;
    puVar4 = DAT_000afb84;
    for (uVar3 = CONCAT22(extraout_var,DAT_000afb88) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar2 = *puVar4;
      puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
      puVar2 = puVar2 + (uint)bVar5 * -2 + 1;
    }
    for (uVar3 = (byte)uVar1 & 0xffffff03; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined1 *)puVar2 = *(undefined1 *)puVar4;
      puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar5 * -2 + 1);
      puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar5 * -2 + 1);
    }
    DAT_000cf2cc = DAT_000cf2cc + 1;
    *(undefined4 **)(&DAT_000cf2d0 + DAT_000cf2cc * 4) = DAT_000afb84;
    FUN_0005c370();
    DAT_000afb88 = DAT_000afb88 + 0x80;
    DAT_000afb84 = extraout_EDX;
  }
  return;
}

