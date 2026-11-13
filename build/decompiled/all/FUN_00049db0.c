
void __regparm3 FUN_00049db0(undefined4 param_1,undefined1 *param_2,byte param_3)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte *unaff_EBX;
  uint uVar5;
  
  bVar1 = unaff_EBX[4];
  bVar2 = unaff_EBX[1];
  bVar3 = unaff_EBX[2];
  bVar4 = unaff_EBX[5];
  uVar5 = (uint)param_3;
  *param_2 = (char)((((int)((uint)*unaff_EBX + (uint)unaff_EBX[3]) >> 1) * uVar5) / 100);
  param_2[1] = (char)((((int)((uint)bVar2 + (uint)bVar1) >> 1) * uVar5) / 100);
  param_2[2] = (char)((((int)((uint)bVar4 + (uint)bVar3) >> 1) * uVar5) / 0x78);
  return;
}

