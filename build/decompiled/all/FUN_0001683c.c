
undefined8 __regparm3 FUN_0001683c(int param_1,undefined4 param_2)

{
  undefined2 uVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  undefined1 uVar4;
  undefined1 uVar5;
  short sVar6;
  uint uVar7;
  
  uVar1 = *(undefined2 *)(&DAT_000a7c4a + param_1 * 0x10);
  out(*(undefined2 *)(&DAT_000a7c50 + param_1 * 0x10),0xff);
  do {
    do {
      uVar2 = in(uVar1);
      uVar3 = in(uVar1);
      uVar4 = in(uVar1);
      uVar5 = in(uVar1);
      uVar7 = (uint)CONCAT11(uVar5,uVar4);
      sVar6 = CONCAT11(uVar3,uVar2) - CONCAT11(uVar5,uVar4);
    } while (0x40 < sVar6);
  } while (sVar6 < -0x40);
  if (3 < param_1) {
    uVar7 = uVar7 * 2;
  }
  return CONCAT44(param_2,uVar7);
}

