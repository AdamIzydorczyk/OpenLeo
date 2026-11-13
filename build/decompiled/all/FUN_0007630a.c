
undefined8 __regparm3 FUN_0007630a(int param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = DAT_000cfa34;
  while( true ) {
    if (puVar1 == (undefined4 *)0x0) {
      return CONCAT44(param_2,0xffffffff);
    }
    if (param_1 == puVar1[1]) break;
    puVar1 = (undefined4 *)*puVar1;
  }
  uVar2 = FUN_00076337();
  return CONCAT44(param_2,uVar2);
}

