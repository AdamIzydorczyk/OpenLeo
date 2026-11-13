
void __regparm3 FUN_0001ab9b(undefined4 param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  int unaff_ESI;
  byte *pbVar3;
  
  pbVar3 = (byte *)(unaff_ESI + 4);
  iVar2 = param_2;
  do {
    bVar1 = *pbVar3;
    pbVar3 = pbVar3 + 1;
    *DAT_000a893c = *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 6));
    DAT_000a893c = DAT_000a893c + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  pbVar3 = (byte *)(unaff_ESI + 4);
  iVar2 = param_2;
  do {
    bVar1 = *pbVar3;
    pbVar3 = pbVar3 + 1;
    *DAT_000a8940 = *(undefined1 *)(unaff_ESI + (bVar1 >> 4 & 0xffffff03));
    DAT_000a8940 = DAT_000a8940 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  pbVar3 = (byte *)(unaff_ESI + 4);
  iVar2 = param_2;
  do {
    bVar1 = *pbVar3;
    pbVar3 = pbVar3 + 1;
    *DAT_000a8944 = *(undefined1 *)(unaff_ESI + (bVar1 >> 2 & 0xffffff03));
    DAT_000a8944 = DAT_000a8944 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  pbVar3 = (byte *)(unaff_ESI + 4);
  do {
    bVar1 = *pbVar3;
    pbVar3 = pbVar3 + 1;
    *DAT_000a8948 = *(undefined1 *)(unaff_ESI + (bVar1 & 0xffffff03));
    DAT_000a8948 = DAT_000a8948 + 1;
    param_2 = param_2 + -1;
  } while (param_2 != 0);
  return;
}

