
undefined8 __regparm3 FUN_00017644(undefined4 param_1,uint param_2,uint param_3,byte param_4)

{
  byte bVar1;
  int iVar2;
  uint extraout_ECX;
  undefined2 uVar3;
  undefined2 uVar4;
  uint unaff_EBX;
  byte bVar5;
  byte bVar6;
  byte in_AF;
  byte bVar7;
  byte bVar8;
  byte in_TF;
  byte in_IF;
  byte bVar9;
  byte bVar10;
  byte in_NT;
  byte in_AC;
  byte in_VIF;
  byte in_VIP;
  byte in_ID;
  
  bVar9 = 0;
  if (param_3 < unaff_EBX) {
    param_4 = param_4 | 0x40;
    unaff_EBX = param_3;
  }
  bVar5 = 0;
  bVar10 = 0;
  bVar8 = 0;
  bVar7 = (param_4 & 4) == 0;
  bVar6 = POPCOUNT(param_4 & 4) == '\0';
  if (!(bool)bVar7) {
    param_2 = FUN_0001800f(param_1);
    unaff_EBX = FUN_0001800f();
    FUN_0001800f();
  }
  out((short)DAT_000a887b,(char)param_1);
  out((short)DAT_000a887f,0x8d);
  bVar1 = in((short)DAT_000a8887);
  if ((param_4 & 1) == 0) {
    bVar1 = bVar1 & 0xfb;
  }
  else {
    bVar1 = bVar1 | 4;
  }
  out((short)DAT_000a887f,0xd);
  out((short)DAT_000a8887,bVar1);
  FUN_00017fc1((uint)(in_NT & 1) * 0x4000 | (uint)(bVar10 & 1) * 0x800 | (uint)(bVar9 & 1) * 0x400 |
               (uint)(in_IF & 1) * 0x200 | (uint)(in_TF & 1) * 0x100 | (uint)(bVar8 & 1) * 0x80 |
               (uint)(bVar7 & 1) * 0x40 | (uint)(in_AF & 1) * 0x10 | (uint)(bVar6 & 1) * 4 |
               (uint)(bVar5 & 1) | (uint)(in_ID & 1) * 0x200000 | (uint)(in_VIP & 1) * 0x100000 |
               (uint)(in_VIF & 1) * 0x80000 | (uint)(in_AC & 1) * 0x40000);
  out((short)DAT_000a8887,bVar1);
  uVar3 = (undefined2)DAT_000a887f;
  out(uVar3,0xb);
  uVar4 = (undefined2)DAT_000a8883;
  out(uVar4,(short)((param_2 & 0x7f) << 9));
  out(uVar3,10);
  out(uVar4,(ushort)(param_2 >> 7) & 0x1fff);
  out(uVar3,2);
  out(uVar4,(ushort)(unaff_EBX >> 7) & 0x1fff);
  out(uVar3,3);
  out(uVar4,(short)((unaff_EBX & 0x7f) << 9));
  out(uVar3,4);
  out(uVar4,(ushort)(extraout_ECX >> 7) & 0x1fff);
  out(uVar3,5);
  iVar2 = (extraout_ECX & 0x7f) << 9;
  out(uVar4,(short)iVar2);
  return CONCAT44(DAT_000a8883,CONCAT31((int3)((uint)iVar2 >> 8),param_4));
}

