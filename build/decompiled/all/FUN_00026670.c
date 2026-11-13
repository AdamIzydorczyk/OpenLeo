
undefined8 __regparm3 FUN_00026670(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  undefined3 uVar3;
  
  iVar1 = FUN_00076bcc();
  uVar2 = (uint)(iVar1 * 10) / 0x7fff;
  uVar3 = (undefined3)(uVar2 >> 8);
  if (uVar2 < 3) {
    return CONCAT44(param_2,CONCAT31(uVar3,0x36));
  }
  if (uVar2 < 8) {
    return CONCAT44(param_2,CONCAT31(uVar3,0x37));
  }
  return CONCAT44(param_2,CONCAT31(uVar3,0x38));
}

