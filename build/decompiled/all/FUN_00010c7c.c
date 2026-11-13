
void __regparm3 FUN_00010c7c(int param_1,int param_2,int param_3)

{
  byte bVar1;
  byte bVar2;
  int unaff_EBX;
  
  if ((unaff_EBX == param_3) && (param_3 != 0)) {
    bVar2 = (&DAT_000a6c68)[unaff_EBX] | 0x40;
  }
  else {
    bVar2 = (&DAT_000a6c68)[unaff_EBX] | (&DAT_000a6c68)[param_3] << 3;
  }
  if ((param_1 == param_2) && (param_2 != 0)) {
    bVar1 = (&DAT_000a6c60)[param_1] | 0x40;
  }
  else {
    bVar1 = (&DAT_000a6c60)[param_1] | (&DAT_000a6c60)[param_2] << 3;
  }
  out(DAT_000ade94 + 0xf,5);
  out(DAT_000ade94,DAT_000ade99);
  out(DAT_000ade94 + 0xb,0);
  out(DAT_000ade94 + 0xf,0);
  out(DAT_000ade94,DAT_000ade99);
  out(DAT_000ade94 + 0xb,bVar1 | 0x80);
  out(DAT_000ade94,DAT_000ade99 | 0x40);
  out(DAT_000ade94 + 0xb,bVar2);
  out(DAT_000ade94,DAT_000ade99);
  out(DAT_000ade94 + 0xb,bVar1);
  out(DAT_000ade94,DAT_000ade99 | 0x40);
  out(DAT_000ade94 + 0xb,bVar2);
  out(DAT_000ade94 + 0x102,0);
  out(DAT_000ade94,DAT_000ade99 | 9);
  out(DAT_000ade94 + 0x102,0);
  DAT_000ade99 = DAT_000ade99 | 9;
  return;
}

