
void __regparm3 FUN_0005ba40(undefined4 *param_1)

{
  undefined4 *puVar1;
  undefined8 uVar2;
  
  uVar2 = FUN_0005bac0();
  if ((int)uVar2 == 0) {
    param_1[2] = (int)((ulonglong)uVar2 >> 0x20);
    *param_1 = 0;
    puVar1 = param_1;
    param_1[1] = DAT_000cf1bc;
    DAT_000cf1bc = puVar1;
    param_1[4] = param_1[4] | 2;
  }
  return;
}

