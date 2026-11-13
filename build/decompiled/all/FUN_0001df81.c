
undefined8 __regparm2
FUN_0001df81(undefined4 param_1,undefined4 param_2,byte *param_3,undefined1 *param_4)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  undefined1 *puVar4;
  
  iVar3 = 200;
  do {
    iVar2 = 0x140;
    do {
      puVar4 = param_4;
      bVar1 = *param_3;
      param_3 = param_3 + 1;
      *puVar4 = (char)((ushort)CONCAT31(0xff,*puVar4) >> (bVar1 & 0x1f));
      iVar2 = iVar2 + -1;
      param_4 = puVar4 + 1;
    } while (iVar2 != 0);
    param_4 = puVar4 + 0x51;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return CONCAT44(param_2,param_1);
}

