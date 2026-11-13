
void __regparm3 FUN_00018455(int param_1,int param_2)

{
  undefined6 uVar1;
  
  if (param_1 != 0) {
    uVar1 = FUN_00075c5a();
    DAT_000a8836 = (undefined2)((uint6)uVar1 >> 0x20);
    DAT_000a8832 = (undefined4)uVar1;
    FUN_00075c8c();
  }
  if ((param_2 != 0) && (param_2 != param_1)) {
    uVar1 = FUN_00075c5a();
    DAT_000a883c = (undefined2)((uint6)uVar1 >> 0x20);
    DAT_000a8838 = (undefined4)uVar1;
    FUN_00075c8c();
  }
  return;
}

