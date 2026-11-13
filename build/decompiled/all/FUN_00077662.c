
undefined8 __regparm3 FUN_00077662(int param_1,undefined4 param_2)

{
  code *pcVar1;
  undefined4 *puVar2;
  byte bVar3;
  undefined4 uStack_8;
  
  uStack_8 = param_2;
  puVar2 = &uStack_8;
  if (param_1 != 0) {
    bVar3 = 0;
    if (*(char *)(param_1 + 0x2d) != '\0') {
      *(undefined1 *)(param_1 + 0x2d) = 0;
      return CONCAT44(param_2,param_1);
    }
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    puVar2 = (undefined4 *)register0x00000010;
    if ((bVar3 & 1) != 0) {
      param_1 = 0;
      FUN_0007ba5f();
    }
  }
  return CONCAT44(*puVar2,param_1);
}

