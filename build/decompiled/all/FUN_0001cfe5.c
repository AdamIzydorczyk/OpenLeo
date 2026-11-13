
undefined8 __regparm3 FUN_0001cfe5(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined **ppuVar4;
  
  puVar3 = (undefined4 *)(DAT_000cef2c + 400 + DAT_000cf0e8);
  ppuVar4 = &PTR_FUN_000a0000;
  iVar2 = 200;
  do {
    for (iVar1 = 0x50; iVar1 != 0; iVar1 = iVar1 + -1) {
      *ppuVar4 = (undefined *)*puVar3;
      puVar3 = puVar3 + 1;
      ppuVar4 = ppuVar4 + 1;
    }
    puVar3 = puVar3 + 0x14;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return CONCAT44(param_2,param_1);
}

