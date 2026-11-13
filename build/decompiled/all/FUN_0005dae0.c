
void __regparm3 FUN_0005dae0(uint *param_1)

{
  int iVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  
  iVar1 = FUN_0007607b();
  if (iVar1 == 0) {
    FUN_0005e3a0();
    return;
  }
  puVar2 = (uint *)thunk_FUN_0005c320();
  FUN_0007612f();
  uVar3 = 0;
  puVar5 = puVar2;
  puVar6 = param_1;
  do {
    puVar6 = puVar6 + 1;
    uVar4 = uVar3 ^ *puVar5;
    puVar5 = puVar5 + 1;
    uVar3 = uVar3 + 1;
    *puVar6 = uVar4 & 0xff;
  } while ((int)uVar3 < 0x15);
  FUN_0007612f();
  uVar3 = 0;
  puVar5 = puVar2;
  do {
    uVar4 = uVar3 ^ *puVar5;
    puVar5 = puVar5 + 1;
    uVar3 = uVar3 + 1;
    param_1[0x16] = uVar4 & 0xff;
    param_1 = param_1 + 1;
  } while ((int)uVar3 < 0x15);
  FUN_0007612f();
  uVar3 = 0;
  puVar5 = puVar2;
  do {
    uVar4 = uVar3 ^ *puVar5;
    uVar3 = uVar3 + 1;
    puVar5 = puVar5 + 1;
    (&DAT_000cf468)[uVar3] = uVar4 & 0xff;
  } while ((int)uVar3 < 0x15);
  FUN_0007612f();
  DAT_000a94f0 = (int)((*puVar2 ^ 0x77072400) & 0xff00) >> 8;
  FUN_0007612f();
  DAT_000cf464 = (int)((*puVar2 ^ 0x77072400) & 0xff00) >> 8;
  FUN_0007612f();
  FUN_0007612f();
  FUN_0007612f();
  FUN_0007612f();
  FUN_0007612f();
  thunk_FUN_0005c370();
  FUN_0007630a();
  return;
}

