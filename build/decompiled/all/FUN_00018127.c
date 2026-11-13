
undefined8 __regparm3 FUN_00018127(uint param_1,uint param_2)

{
  int iVar1;
  byte bVar2;
  uint uVar3;
  
  uVar3 = param_2;
  if ((param_1 != 2) && (param_1 != 0)) {
    iVar1 = param_1 * 5;
    bVar2 = in((ushort)(byte)(&DAT_000a87ce)[iVar1]);
    uVar3 = (uint)(byte)(&DAT_000a87ce)[iVar1];
    param_1 = (uint)(byte)(bVar2 | ~(&DAT_000a87cb)[iVar1]);
    out((ushort)(byte)(&DAT_000a87ce)[iVar1],bVar2 | ~(&DAT_000a87cb)[iVar1]);
  }
  if ((param_2 != 2) && (param_2 != 0)) {
    iVar1 = param_2 * 5;
    bVar2 = in((ushort)(byte)(&DAT_000a87ce)[iVar1]);
    uVar3 = (uint)(byte)(&DAT_000a87ce)[iVar1];
    param_1 = (uint)(byte)(bVar2 | ~(&DAT_000a87cb)[iVar1]);
    out((ushort)(byte)(&DAT_000a87ce)[iVar1],bVar2 | ~(&DAT_000a87cb)[iVar1]);
  }
  return CONCAT44(uVar3,param_1);
}

