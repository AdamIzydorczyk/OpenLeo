
/* WARNING: Removing unreachable block (ram,0x0005dcfa) */

void __regparm3 FUN_0005dcc0(undefined4 param_1)

{
  int iVar1;
  uint *puVar2;
  undefined4 uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  uint uVar4;
  uint *puVar5;
  undefined8 uVar6;
  
  iVar1 = FUN_0007607b(param_1);
  if (iVar1 == 0) {
    ll_printf(&DAT_000a92d4,s_Cannot_open_status_file__000926a3);
    ll_rethrow_report();
  }
  puVar2 = (uint *)thunk_FUN_0005c320();
  puVar5 = puVar2;
  do {
    uVar6 = FUN_00076bcc();
    uVar4 = (uint)((ulonglong)uVar6 >> 0x20);
    *puVar5 = (uVar4 | 0x77072400) ^ (*(uint *)(extraout_ECX + 4) | (uint)uVar6 & 0xffffff00);
    puVar5 = puVar5 + 1;
  } while ((int)(uVar4 + 1) < 0x15);
  FUN_0007721f();
  puVar5 = puVar2;
  do {
    uVar6 = FUN_00076bcc();
    uVar4 = (uint)((ulonglong)uVar6 >> 0x20);
    *puVar5 = (uVar4 | 0x77072400) ^ (*(uint *)(extraout_ECX_00 + 0x58) | (uint)uVar6 & 0xffffff00);
    puVar5 = puVar5 + 1;
  } while ((int)(uVar4 + 1) < 0x15);
  FUN_0007721f();
  puVar5 = puVar2;
  do {
    uVar6 = FUN_00076bcc();
    uVar4 = (uint)((ulonglong)uVar6 >> 0x20);
    *puVar5 = (uVar4 | 0x77072400) ^ ((&DAT_000cf46c)[uVar4] | (uint)uVar6 & 0xffffff00);
    puVar5 = puVar5 + 1;
  } while ((int)(uVar4 + 1) < 0x15);
  FUN_0007721f();
  uVar3 = FUN_00076bcc();
  *puVar2 = (CONCAT22((short)((uint)uVar3 >> 0x10),(ushort)(byte)uVar3) | DAT_000a94f0 << 8) ^
            0x77072400;
  FUN_0007721f();
  uVar3 = FUN_00076bcc();
  *puVar2 = (CONCAT22((short)((uint)uVar3 >> 0x10),(ushort)(byte)uVar3) | DAT_000cf464 << 8) ^
            0x77072400;
  FUN_0007721f();
  FUN_0007721f();
  FUN_0007721f();
  FUN_0007721f();
  FUN_0007721f();
  FUN_0007721f();
  thunk_FUN_0005c370();
  FUN_0007630a();
  return;
}

