
undefined8 __regparm3 FUN_00018082(uint param_1,uint param_2)

{
  int iVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  
  uVar3 = 0;
  uVar4 = param_2;
  if (param_1 != 0) {
    iVar1 = param_1 * 5;
    bVar2 = in((ushort)(byte)(&DAT_000a87ce)[iVar1]);
    out((ushort)(byte)(&DAT_000a87ce)[iVar1],bVar2 & (&DAT_000a87cb)[iVar1]);
    uVar3 = (uint)(byte)(&DAT_000a87cc)[iVar1];
    uVar4 = (uint)(byte)(&DAT_000a87cd)[iVar1];
    out((ushort)(byte)(&DAT_000a87cd)[iVar1],(&DAT_000a87cc)[iVar1]);
  }
  if ((param_2 != param_1) && (param_2 != 0)) {
    iVar1 = param_2 * 5;
    bVar2 = in((ushort)(byte)(&DAT_000a87ce)[iVar1]);
    out((ushort)(byte)(&DAT_000a87ce)[iVar1],bVar2 & (&DAT_000a87cb)[iVar1]);
    uVar3 = (uint)(byte)(&DAT_000a87cc)[iVar1];
    uVar4 = (uint)(byte)(&DAT_000a87cd)[iVar1];
    out((ushort)(byte)(&DAT_000a87cd)[iVar1],(&DAT_000a87cc)[iVar1]);
  }
  if ((7 < (int)param_2) || (7 < (int)param_1)) {
    bVar2 = in((ushort)DAT_000a87d8);
    out((ushort)DAT_000a87d8,bVar2 & DAT_000a87d5);
    uVar3 = (uint)DAT_000a87d6;
    uVar4 = (uint)DAT_000a87d7;
    out((ushort)DAT_000a87d7,DAT_000a87d6);
  }
  return CONCAT44(uVar4,uVar3);
}

