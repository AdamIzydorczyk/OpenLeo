
ulonglong __regparm3 FUN_000175cc(undefined1 param_1,undefined4 param_2)

{
  byte bVar1;
  undefined4 uVar2;
  byte in_CF;
  byte in_PF;
  byte in_AF;
  byte in_ZF;
  byte in_SF;
  byte in_TF;
  byte in_IF;
  byte in_OF;
  byte in_NT;
  byte in_AC;
  byte in_VIF;
  byte in_VIP;
  byte in_ID;
  
  out((short)DAT_000a887b,param_1);
  out((short)DAT_000a887f,0x8d);
  bVar1 = in((short)DAT_000a8887);
  out((short)DAT_000a887f,0xd);
  out((short)DAT_000a8887,bVar1 | 3);
  uVar2 = FUN_00017fc1((uint)(in_NT & 1) * 0x4000 | (uint)(in_OF & 1) * 0x800 |
                       (uint)(in_IF & 1) * 0x200 | (uint)(in_TF & 1) * 0x100 |
                       (uint)(in_SF & 1) * 0x80 | (uint)(in_ZF & 1) * 0x40 |
                       (uint)(in_AF & 1) * 0x10 | (uint)(in_PF & 1) * 4 | (uint)(in_CF & 1) |
                       (uint)(in_ID & 1) * 0x200000 | (uint)(in_VIP & 1) * 0x100000 |
                       (uint)(in_VIF & 1) * 0x80000 | (uint)(in_AC & 1) * 0x40000);
  out((short)DAT_000a8887,bVar1 | 3);
  return CONCAT44(param_2,CONCAT31((int3)((uint)uVar2 >> 8),bVar1)) | 3;
}

