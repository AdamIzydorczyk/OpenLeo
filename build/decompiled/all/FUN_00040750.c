
undefined4 * __regparm3 FUN_00040750(undefined4 *param_1,int param_2)

{
  byte bVar1;
  undefined4 *puVar2;
  int iVar3;
  
  iVar3 = 0;
  puVar2 = param_1;
  if (0 < param_2) {
    do {
      iVar3 = iVar3 + 1;
      bVar1 = in((short)*param_1 + 0xe);
      puVar2 = (undefined4 *)(uint)bVar1;
    } while (iVar3 < param_2);
  }
  return puVar2;
}

