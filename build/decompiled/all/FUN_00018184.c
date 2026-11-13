
void __regparm3 FUN_00018184(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  
  out((short)DAT_000a887f,0x41);
  bVar1 = in((short)DAT_000a8887);
  if ((bVar1 & 0x40) != 0) {
    iVar2 = (DAT_000a8822 + -1) * 0x4e;
    pbVar3 = &DAT_000a85a8 + iVar2;
    if ((*pbVar3 & 4) == 0) {
      *pbVar3 = *pbVar3 & 0xfd;
      DAT_000a881a = DAT_000a881a & 0xfd;
      *(int *)(&DAT_000a85e1 + iVar2) =
           *(int *)(&DAT_000a85e1 + iVar2) + *(int *)(&DAT_000a85e5 + iVar2);
      (*DAT_000a8856)(param_2);
    }
    else {
      FUN_0001797e();
    }
  }
  out((short)DAT_000a887f,0x49);
  bVar1 = in((short)DAT_000a8887);
  if ((bVar1 & 0x40) != 0) {
    iVar2 = (DAT_000a8826 + -1) * 0x4e;
    pbVar3 = &DAT_000a85a8 + iVar2;
    if ((*pbVar3 & 4) != 0) {
      FUN_0001797e();
      return;
    }
    *pbVar3 = *pbVar3 & 0xfd;
    DAT_000a881a = DAT_000a881a & 0xfb;
    *(int *)(&DAT_000a85e1 + iVar2) =
         *(int *)(&DAT_000a85e1 + iVar2) + *(int *)(&DAT_000a85e5 + iVar2);
    (*DAT_000a885a)();
  }
  return;
}

