
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_00013600(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined1 uVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  
  bVar7 = 0;
  _DAT_000c3fb0 = 0;
  DAT_000aee90 = param_1;
  FUN_000749a0();
  iVar2 = 0;
  do {
    if ((iVar2 + 1U & 2) == 0) {
      uVar4 = 0;
    }
    else {
      uVar4 = 0xff;
    }
    iVar1 = iVar2 + 1;
    (&DAT_000aefa1)[iVar2] = uVar4;
    iVar2 = iVar1;
  } while (iVar1 < 0x20);
  iVar2 = FUN_00013440();
  if (((iVar2 == 0) || (iVar2 = FUN_0001338c(), iVar2 == 0)) ||
     (puVar3 = (undefined4 *)FUN_0001317c(), puVar3 == (undefined4 *)0x0)) {
    FUN_00013510();
    puVar3 = (undefined4 *)0x0;
  }
  else {
    puVar5 = (undefined4 *)&DAT_000aee94;
    puVar6 = puVar3;
    for (iVar2 = 0x52; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar6 = *puVar5;
      puVar5 = puVar5 + (uint)bVar7 * -2 + 1;
      puVar6 = puVar6 + (uint)bVar7 * -2 + 1;
    }
    for (iVar2 = 2; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined1 *)puVar6 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar7 * -2 + 1);
      puVar6 = (undefined4 *)((int)puVar6 + (uint)bVar7 * -2 + 1);
    }
  }
  return CONCAT44(param_2,puVar3);
}

