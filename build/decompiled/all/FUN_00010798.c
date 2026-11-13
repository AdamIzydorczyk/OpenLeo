
undefined8 __regparm3 FUN_00010798(int param_1,undefined1 *param_2)

{
  int iVar1;
  undefined1 *puVar2;
  int unaff_EBX;
  
  iVar1 = param_1;
  puVar2 = param_2;
  if (unaff_EBX != 0) {
    out(DAT_000ade94 + 0x103,0x44);
    out(DAT_000ade94 + 0x105,(char)((uint)param_1 >> 0x10));
    DAT_000adea8 = 0x43;
    iVar1 = 0x43;
    puVar2 = (undefined1 *)(DAT_000ade94 + 0x103);
    while( true ) {
      out((short)puVar2,(char)iVar1);
      unaff_EBX = unaff_EBX + -1;
      if (unaff_EBX == -1) break;
      out(DAT_000ade94 + 0x104,(short)param_1);
      iVar1 = CONCAT31((int3)((uint)param_1 >> 8),*param_2);
      puVar2 = (undefined1 *)(DAT_000ade94 + 0x107);
      param_2 = param_2 + 1;
      param_1 = param_1 + 1;
    }
  }
  return CONCAT44(puVar2,iVar1);
}

