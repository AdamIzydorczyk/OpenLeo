
void __regparm3 FUN_0005e370(undefined4 *param_1)

{
  undefined4 *puVar2;
  undefined4 *puVar1;
  
  puVar1 = param_1;
  do {
    puVar2 = puVar1 + 1;
    puVar1[0x16] = 0;
    *puVar2 = puVar1[0x16];
    puVar1 = puVar2;
  } while (puVar2 != param_1 + 0x15);
  return;
}

