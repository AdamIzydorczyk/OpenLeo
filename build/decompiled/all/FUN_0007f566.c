
void __regparm3 FUN_0007f566(undefined8 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined2 uVar1;
  undefined8 uVar2;
  
  uVar1 = *(undefined2 *)(param_1 + 1);
  FUN_0007e316(param_2);
  FUN_0007f514(param_3);
  uVar2 = FUN_0007e316();
  *param_1 = uVar2;
  *(undefined2 *)(param_1 + 1) = uVar1;
  return;
}

