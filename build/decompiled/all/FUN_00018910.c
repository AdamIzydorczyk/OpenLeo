
undefined8 __regparm3 FUN_00018910(undefined4 *param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined8 uVar3;
  
  *param_1 = 0x220;
  param_1[1] = 1;
  param_1[2] = 1;
  param_1[3] = 0xb;
  param_1[4] = 5;
  uVar3 = FUN_00074c13();
  iVar2 = (int)((ulonglong)uVar3 >> 0x20);
  iVar1 = (int)uVar3;
  if (iVar1 != 0) {
    FUN_00074cc8(iVar1,s__x__d__d__d__d_00090429,iVar2,iVar2 + 4,iVar2 + 8,iVar2 + 0xc,iVar2 + 0x10)
    ;
  }
  return CONCAT44(param_2,(uint)(iVar1 != 0));
}

