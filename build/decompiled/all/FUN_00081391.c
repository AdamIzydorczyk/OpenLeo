
void __regparm1 FUN_00081391(int param_1,undefined8 param_2,int param_3)

{
  undefined4 *puVar1;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  
  puVar1 = (undefined4 *)((int)register0x00000010 + -0x14);
  if (param_1 != param_3) {
    uStack_18 = param_3 - param_1;
    uStack_1c = param_2._4_4_;
    uStack_20 = (undefined4)param_2;
    FUN_000813d2();
    puVar1 = &uStack_20;
  }
  *(undefined4 *)((int)puVar1 + -4) = 0x813c1;
  FUN_00081c30();
  *(undefined4 *)((int)puVar1 + -4) = 0x813ca;
  FUN_00081bf6();
  return;
}

