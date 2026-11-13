
undefined8 __regparm3 FUN_00017db6(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined2 extraout_CX;
  undefined2 extraout_CX_00;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  byte bVar7;
  byte bVar8;
  byte in_AF;
  byte bVar9;
  byte bVar10;
  byte in_TF;
  byte in_IF;
  byte bVar11;
  byte bVar12;
  byte in_NT;
  byte in_AC;
  byte in_VIF;
  byte in_VIP;
  byte in_ID;
  uint uVar13;
  char local_1c;
  
  uVar2 = DAT_000a8887;
  uVar1 = DAT_000a8883;
  bVar11 = 0;
  if ((param_1 < 0xe) || (0x20 < param_1)) {
    uVar1 = 2;
  }
  else {
    local_1c = (char)param_1;
    DAT_000a8871 = 0;
    DAT_000a8872 = 0;
    DAT_000a8863 = local_1c;
    FUN_00016c99();
    bVar7 = 0;
    bVar12 = 0;
    bVar10 = 0;
    bVar9 = 1;
    bVar8 = 1;
    FUN_00016c99();
    uVar13 = (uint)(in_NT & 1) * 0x4000 | (uint)(bVar12 & 1) * 0x800 | (uint)(bVar11 & 1) * 0x400 |
             (uint)(in_IF & 1) * 0x200 | (uint)(in_TF & 1) * 0x100 | (uint)(bVar10 & 1) * 0x80 |
             (uint)(bVar9 & 1) * 0x40 | (uint)(in_AF & 1) * 0x10 | (uint)(bVar8 & 1) * 4 |
             (uint)(bVar7 & 1) | (uint)(in_ID & 1) * 0x200000 | (uint)(in_VIP & 1) * 0x100000 |
             (uint)(in_VIF & 1) * 0x80000 | (uint)(in_AC & 1) * 0x40000;
    out(extraout_CX,0x4c);
    iVar6 = 0;
    uVar3 = (undefined2)uVar2;
    out(uVar3,0);
    do {
      iVar6 = iVar6 + 1;
      FUN_00017fc1(uVar13);
    } while (iVar6 < 10);
    out(extraout_CX_00,0x4c);
    iVar6 = 0;
    out(uVar3,1);
    do {
      iVar6 = iVar6 + 1;
      FUN_00017fc1(uVar13);
    } while (iVar6 < 10);
    out((short)DAT_000a8877,3);
    iVar6 = 0;
    do {
      iVar6 = iVar6 + 1;
      FUN_00017fc1();
    } while (iVar6 < 10);
    out((short)DAT_000a8877,0);
    uVar4 = (undefined2)extraout_ECX;
    out(uVar4,0x41);
    out(uVar3,0);
    out(uVar4,0x45);
    out(uVar3,0);
    out(uVar4,0x49);
    out(uVar3,0);
    out(uVar4,0xe);
    out(uVar3,local_1c - 1U | 0xc0);
    in((short)DAT_000a888b);
    out(uVar4,0x41);
    in(uVar3);
    out(uVar4,0x49);
    in(uVar3);
    out(uVar4,0x8f);
    iVar6 = 0;
    in(uVar3);
    uVar2 = extraout_ECX;
    while (uVar4 = (undefined2)uVar2, iVar6 < param_1) {
      out((short)DAT_000a887b,(char)iVar6);
      out(uVar4,0);
      out(uVar3,3);
      out(uVar4,0xd);
      out(uVar3,3);
      FUN_00017fc1();
      uVar4 = (undefined2)extraout_ECX_00;
      out(uVar4,1);
      uVar5 = (undefined2)uVar1;
      out(uVar5,0x400);
      out(uVar4,2);
      out(uVar5,0);
      out(uVar4,3);
      out(uVar5,0);
      out(uVar4,4);
      out(uVar5,0);
      out(uVar4,5);
      out(uVar5,0);
      out(uVar4,6);
      out(uVar3,1);
      out(uVar4,7);
      out(uVar3,0x10);
      out(uVar4,8);
      out(uVar3,0xe0);
      out(uVar4,9);
      out(uVar5,0);
      out(uVar4,10);
      out(uVar5,0);
      out(uVar4,0xb);
      out(uVar5,0);
      out(uVar4,0xc);
      iVar6 = iVar6 + 1;
      out(uVar3,7);
      uVar2 = extraout_ECX_00;
    }
    in((short)DAT_000a888b);
    out(uVar4,0x41);
    in(uVar3);
    out(uVar4,0x49);
    in(uVar3);
    out(uVar4,0x8f);
    in(uVar3);
    out(uVar4,0x4c);
    out(uVar3,7);
    uVar1 = 1;
  }
  return CONCAT44(param_2,uVar1);
}

