
undefined8 __regparm3 FUN_00040730(undefined4 *param_1,undefined4 param_2)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = FUN_000406b0();
  uVar3 = 0;
  if (iVar2 != 0) {
    bVar1 = in((short)*param_1 + 10);
    uVar3 = (uint)bVar1;
  }
  return CONCAT44(param_2,uVar3);
}

