
undefined2 __regparm3 FUN_0001a9fe(undefined2 param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  int unaff_ESI;
  byte *pbVar3;
  
  pbVar3 = (byte *)(unaff_ESI + 2);
  if (param_2 != 1) {
    uVar2 = param_2 >> 1;
    do {
      bVar1 = *pbVar3;
      pbVar3 = pbVar3 + 1;
      *DAT_000a893c =
           CONCAT11(*(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 3 & 1)),
                    *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 7)));
      DAT_000a893c = DAT_000a893c + 1;
      *DAT_000a8940 =
           CONCAT11(*(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 2 & 1)),
                    *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 6 & 1)));
      DAT_000a8940 = DAT_000a8940 + 1;
      *DAT_000a8944 =
           CONCAT11(*(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 1 & 1)),
                    *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 5 & 1)));
      DAT_000a8944 = DAT_000a8944 + 1;
      param_1 = CONCAT11(*(undefined1 *)(unaff_ESI + (uint)(bVar1 & 1)),
                         *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 4 & 1)));
      *DAT_000a8948 = param_1;
      DAT_000a8948 = DAT_000a8948 + 1;
      uVar2 = uVar2 - 1;
    } while (uVar2 != 0);
  }
  if ((param_2 & 1) != 0) {
    bVar1 = *pbVar3;
    *(undefined1 *)DAT_000a8948 = *(undefined1 *)(unaff_ESI + (uint)((bVar1 >> 4 & 1) != 0));
    DAT_000a8948 = (undefined2 *)((int)DAT_000a8948 + 1);
    *(undefined1 *)DAT_000a8944 = *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 5 & 1));
    DAT_000a8944 = (undefined2 *)((int)DAT_000a8944 + 1);
    *(undefined1 *)DAT_000a8940 = *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 6 & 1));
    DAT_000a8940 = (undefined2 *)((int)DAT_000a8940 + 1);
    param_1 = 0;
    *(undefined1 *)DAT_000a893c = *(undefined1 *)(unaff_ESI + (uint)(bVar1 >> 7));
    DAT_000a893c = (undefined2 *)((int)DAT_000a893c + 1);
  }
  return param_1;
}

