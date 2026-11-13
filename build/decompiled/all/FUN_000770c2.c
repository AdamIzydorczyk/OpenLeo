
undefined8 __regparm3 FUN_000770c2(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  byte *pbVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  
  uVar3 = FUN_000770f1(param_2,param_3);
  puVar2 = (undefined4 *)((ulonglong)uVar3 >> 0x20);
  if ((int)uVar3 == 0) {
    return CONCAT44(param_2,0xffffffff);
  }
  pbVar1 = (byte *)*puVar2;
  puVar2[1] = puVar2[1] + -1;
  *puVar2 = pbVar1 + 1;
  return CONCAT44(param_2,(uint)*pbVar1);
}

