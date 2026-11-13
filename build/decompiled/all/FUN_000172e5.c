
void __regparm3 FUN_000172e5(int param_1,int param_2,int param_3)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int unaff_EBX;
  byte local_24;
  char local_18;
  byte local_14;
  byte local_10;
  
  local_10 = 0;
  local_18 = '\0';
  if (param_1 != 0) {
    local_10 = (&DAT_000a85ac)[(param_1 + -1) * 0x4e];
  }
  if (param_2 != 0) {
    local_18 = (&DAT_000a85ac)[(param_2 + -1) * 0x4e];
  }
  if ((unaff_EBX == param_3) && (param_3 != 0)) {
    local_14 = (&DAT_000a87ca)[unaff_EBX * 5] | 0x40;
  }
  else {
    local_14 = (&DAT_000a87ca)[unaff_EBX * 5] | (&DAT_000a87ca)[param_3 * 5] << 3;
  }
  if ((param_1 == param_2) && (param_2 != 0)) {
    local_24 = local_10 | 0x40;
  }
  else {
    local_24 = local_10 | local_18 << 3;
  }
  out((short)DAT_000a881e + 0xf,5);
  uVar1 = (undefined2)DAT_000a8893;
  out(uVar1,DAT_000a8862);
  uVar2 = (undefined2)DAT_000a8897;
  out(uVar2,0);
  out((short)DAT_000a881e + 0xf,0);
  out(uVar1,DAT_000a8862);
  out(uVar2,local_24 | 0x80);
  out(uVar1,DAT_000a8862 | 0x40);
  out(uVar2,local_14);
  out(uVar1,DAT_000a8862);
  out(uVar2,local_24);
  out(uVar1,DAT_000a8862 | 0x40);
  out(uVar2,local_14);
  out((short)DAT_000a887b,0);
  out(uVar1,DAT_000a8862 | 9);
  out((short)DAT_000a887b,0);
  DAT_000a8862 = DAT_000a8862 | 9;
  return;
}

