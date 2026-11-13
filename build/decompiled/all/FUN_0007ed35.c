
void __regparm3 FUN_0007ed35(undefined4 param_1)

{
  undefined8 *extraout_EDX;
  undefined8 uVar1;
  undefined8 *puVar2;
  
  FUN_0007ed78(param_1);
  puVar2 = extraout_EDX;
  FUN_0007e2ec();
  uVar1 = FUN_0007e316();
  *puVar2 = uVar1;
  *(undefined2 *)(puVar2 + 1) = 0x3fff;
  return;
}

