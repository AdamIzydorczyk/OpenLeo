
void __regparm3 FUN_0001830e(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  byte in_AF;
  byte in_TF;
  byte in_IF;
  byte bVar2;
  byte in_NT;
  byte in_AC;
  byte in_VIF;
  byte in_VIP;
  byte in_ID;
  uint uVar3;
  
  bVar2 = 0;
  while( true ) {
    bVar1 = in((short)DAT_000a888b);
    uVar3 = (uint)bVar1;
    if (bVar1 == 0) break;
    if ((bVar1 & 0x80) != 0) {
      FUN_00018184();
      uVar3 = extraout_ECX;
    }
    if ((uVar3 & 3) != 0) {
      in((short)DAT_000a8877);
      if ((uVar3 & 1) != 0) {
        (*DAT_000a883e)();
        uVar3 = extraout_ECX_00;
      }
      if ((uVar3 & 2) != 0) {
        in((short)DAT_000a8873);
        (*DAT_000a8842)();
        uVar3 = extraout_ECX_01;
      }
    }
    if ((uVar3 & 4) != 0) {
      uVar3 = (uint)(in_NT & 1) * 0x4000 | (uint)(bVar2 & 1) * 0x400 | (uint)(in_IF & 1) * 0x200 |
              (uint)(in_TF & 1) * 0x100 | (uint)(in_AF & 1) * 0x10 |
              (uint)(POPCOUNT((byte)uVar3 & 4) == '\0') * 4 | (uint)(in_ID & 1) * 0x200000 |
              (uint)(in_VIP & 1) * 0x100000 | (uint)(in_VIF & 1) * 0x80000 |
              (uint)(in_AC & 1) * 0x40000;
      out((short)DAT_000a887f,0x45);
      out((short)DAT_000a8887,DAT_000a8871 & 0xfb);
      out((short)DAT_000a887f,0x45);
      out((short)DAT_000a8887,DAT_000a8871);
      (*DAT_000a8846)(uVar3);
      in_NT = (uVar3 & 0x4000) != 0;
      bVar2 = (uVar3 & 0x400) != 0;
      in_IF = (uVar3 & 0x200) != 0;
      in_TF = (uVar3 & 0x100) != 0;
      in_AF = (uVar3 & 0x10) != 0;
      in_ID = (uVar3 & 0x200000) != 0;
      in_AC = (uVar3 & 0x40000) != 0;
      in_VIP = 0;
      in_VIF = 0;
      uVar3 = extraout_ECX_02;
    }
    if ((uVar3 & 8) != 0) {
      uVar3 = (uint)(in_NT & 1) * 0x4000 | (uint)(bVar2 & 1) * 0x400 | (uint)(in_IF & 1) * 0x200 |
              (uint)(in_TF & 1) * 0x100 | (uint)(in_AF & 1) * 0x10 | (uint)(in_ID & 1) * 0x200000 |
              (uint)(in_AC & 1) * 0x40000;
      out((short)DAT_000a887f,0x45);
      out((short)DAT_000a8887,DAT_000a8871 & 0xf7);
      out((short)DAT_000a887f,0x45);
      out((short)DAT_000a8887,DAT_000a8871);
      (*DAT_000a884a)();
      in_NT = (uVar3 & 0x4000) != 0;
      bVar2 = (uVar3 & 0x400) != 0;
      in_IF = (uVar3 & 0x200) != 0;
      in_TF = (uVar3 & 0x100) != 0;
      in_AF = (uVar3 & 0x10) != 0;
      in_ID = (uVar3 & 0x200000) != 0;
      in_AC = (uVar3 & 0x40000) != 0;
      in_VIP = 0;
      in_VIF = 0;
      uVar3 = extraout_ECX_03;
    }
    if ((uVar3 & 0x60) != 0) {
      FUN_00018226();
    }
  }
  (*DAT_000a885e)(param_2,param_3);
  return;
}

