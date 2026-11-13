
void __regparm3 FUN_0007a3c0(int param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar2 = &DAT_000cfa34;
  while( true ) {
    puVar1 = (undefined4 *)*puVar2;
    if (puVar1 == (undefined4 *)0x0) {
      return;
    }
    if (param_1 == puVar1[1]) break;
    puVar2 = puVar1;
  }
  *(byte *)(param_1 + 0xc) = *(byte *)(puVar1[1] + 0xc) | 3;
  *puVar2 = *puVar1;
  *puVar1 = DAT_000cf9bc;
  DAT_000cf9bc = puVar1;
  return;
}

