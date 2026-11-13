
undefined4 __regparm3 FUN_00016716(int param_1)

{
  undefined8 uVar1;
  byte bVar2;
  undefined4 extraout_ECX;
  ushort unaff_BX;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  undefined1 local_10;
  short local_c;
  byte local_8;
  
  uVar5 = FUN_00016688();
  iVar3 = param_1 * 0x10;
  local_c = (short)((uint)unaff_BX + (uint)uVar5 + -1 >> 0x10);
  if ((short)((ulonglong)uVar5 >> 0x10) != local_c) {
    return 0;
  }
  uVar1 = uVar5;
  if (3 < param_1) {
    uVar1 = CONCAT44((int)((uint)((ulonglong)uVar5 >> 0x20) & 0xffff) >> 1,(uint)uVar5 >> 1);
  }
  iVar4 = (int)((ulonglong)uVar1 >> 0x20) + -1;
  switch(extraout_ECX) {
  case 0:
    local_8 = (&DAT_000a7c53)[iVar3];
    break;
  case 1:
    local_8 = (&DAT_000a7c52)[iVar3];
    break;
  case 2:
    bVar2 = (&DAT_000a7c53)[iVar3];
    goto LAB_000167a1;
  case 3:
    bVar2 = (&DAT_000a7c52)[iVar3];
LAB_000167a1:
    local_8 = bVar2 | 0x10;
  }
  out(*(undefined2 *)(&DAT_000a7c4c + iVar3),(&DAT_000a7c44)[iVar3]);
  out(*(undefined2 *)(&DAT_000a7c4e + iVar3),local_8);
  out(*(undefined2 *)(&DAT_000a7c50 + iVar3),0);
  out(*(undefined2 *)(&DAT_000a7c48 + iVar3),(char)uVar1);
  out(*(undefined2 *)(&DAT_000a7c48 + iVar3),(char)((ulonglong)uVar1 >> 8));
  local_10 = (undefined1)((ulonglong)uVar5 >> 0x10);
  out(*(undefined2 *)(&DAT_000a7c46 + iVar3),local_10);
  out(*(undefined2 *)(&DAT_000a7c50 + iVar3),0);
  out(*(undefined2 *)(&DAT_000a7c4a + iVar3),(char)iVar4);
  out(*(undefined2 *)(&DAT_000a7c4a + iVar3),(char)((uint)iVar4 >> 8));
  out(*(undefined2 *)(&DAT_000a7c4c + iVar3),(&DAT_000a7c45)[iVar3]);
  return 1;
}

