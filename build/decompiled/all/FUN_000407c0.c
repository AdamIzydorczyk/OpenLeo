
void __regparm3 FUN_000407c0(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined8 uVar2;
  
  *param_1 = 0x210;
  iVar1 = *param_1;
  do {
    if (0x28f < iVar1) {
      *param_1 = 0;
      return;
    }
    uVar2 = FUN_00040770(param_2,param_3);
    param_1 = (int *)((ulonglong)uVar2 >> 0x20);
    if ((int)uVar2 != 0) {
      uVar2 = FUN_000407b0();
      param_1 = (int *)((ulonglong)uVar2 >> 0x20);
      if ((int)uVar2 != 0) {
        return;
      }
    }
    iVar1 = *param_1 + 0x10;
    *param_1 = iVar1;
  } while( true );
}

