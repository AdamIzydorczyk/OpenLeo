
undefined8 __regparm3 FUN_000174f4(undefined4 param_1,uint param_2,undefined1 param_3,byte param_4)

{
  uint uVar1;
  byte bVar2;
  undefined4 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  uint unaff_EBX;
  uint uVar6;
  bool bVar7;
  byte in_AF;
  byte in_TF;
  byte in_IF;
  bool bVar8;
  byte in_NT;
  byte in_AC;
  byte in_VIF;
  byte in_VIP;
  byte in_ID;
  
  if (param_2 != unaff_EBX) {
    param_4 = param_4 & 0x78;
    uVar6 = unaff_EBX;
    if (unaff_EBX < param_2) {
      param_4 = param_4 | 0x40;
      uVar6 = param_2;
      param_2 = unaff_EBX;
    }
    if (param_2 < 0x40) {
      param_2 = 0x40;
    }
    bVar7 = uVar6 < 0xfc0;
    bVar8 = SBORROW4(uVar6,0xfc0);
    uVar1 = uVar6 - 0xfc0;
    if (!bVar7 && uVar1 != 0) {
      uVar6 = 0xfc0;
    }
    out((short)DAT_000a887b,(char)param_1);
    uVar4 = (undefined2)DAT_000a887f;
    out(uVar4,6);
    uVar5 = (undefined2)DAT_000a8887;
    out(uVar5,param_3);
    out(uVar4,7);
    out(uVar5,(char)(param_2 >> 4));
    out(uVar4,8);
    out(uVar5,(char)(uVar6 >> 4));
    FUN_00017619((uint)(in_NT & 1) * 0x4000 | (uint)bVar8 * 0x800 | (uint)(in_IF & 1) * 0x200 |
                 (uint)(in_TF & 1) * 0x100 | (uint)((int)uVar1 < 0) * 0x80 |
                 (uint)(uVar1 == 0) * 0x40 | (uint)(in_AF & 1) * 0x10 |
                 (uint)((POPCOUNT(uVar1 & 0xff) & 1U) == 0) * 4 | (uint)bVar7 |
                 (uint)(in_ID & 1) * 0x200000 | (uint)(in_VIP & 1) * 0x100000 |
                 (uint)(in_VIF & 1) * 0x80000 | (uint)(in_AC & 1) * 0x40000,param_1,param_3);
    out((short)DAT_000a887f,0x8d);
    bVar2 = in((short)DAT_000a8887);
    if ((bVar2 & 4) != 0) {
      param_4 = param_4 | 4;
    }
    out((short)DAT_000a887f,0xd);
    out((short)DAT_000a8887,param_4);
    uVar3 = FUN_00017fc1();
    param_1 = CONCAT31((int3)((uint)uVar3 >> 8),param_4);
    out((short)DAT_000a8887,param_4);
    param_2 = DAT_000a8887;
  }
  return CONCAT44(param_2,param_1);
}

