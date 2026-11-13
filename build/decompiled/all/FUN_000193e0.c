
undefined8 __regparm3 FUN_000193e0(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 uStack_28;
  undefined2 local_24 [14];
  undefined4 uStack_8;
  
  local_24[0] = 0x1600;
  uStack_28 = 0x193fc;
  uStack_8 = param_2;
  uVar1 = FUN_00075c3d();
  puVar3 = (undefined4 *)local_24;
  if (DAT_000c406c == 0) {
    puVar3 = &uStack_28;
    puVar4 = &uStack_28;
    if ((uVar1 & 0x3d831002) != 0) {
      uVar2 = 0;
      goto LAB_0001941d;
    }
  }
  uVar2 = 1;
  puVar4 = puVar3;
LAB_0001941d:
  return CONCAT44(*(undefined4 *)((int)puVar4 + 0x1c),uVar2);
}

