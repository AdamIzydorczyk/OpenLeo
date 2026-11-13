
void __regparm3 FUN_0007b74b(char *param_1,undefined4 param_2)

{
  undefined1 uVar1;
  undefined1 *extraout_EDX;
  
  while (*param_1 != '\0') {
    uVar1 = FUN_00076e98(param_2);
    *extraout_EDX = uVar1;
    param_1 = extraout_EDX + 1;
  }
  return;
}

