
void __regparm3 FUN_0001ab32(undefined4 param_1,uint param_2)

{
  uint uVar1;
  undefined4 *unaff_ESI;
  
  for (uVar1 = param_2 >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
    *DAT_000a893c = *unaff_ESI;
    unaff_ESI = unaff_ESI + 1;
    DAT_000a893c = DAT_000a893c + 1;
  }
  for (uVar1 = param_2 & 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *(undefined1 *)DAT_000a893c = *(undefined1 *)unaff_ESI;
    unaff_ESI = (undefined4 *)((int)unaff_ESI + 1);
    DAT_000a893c = (undefined4 *)((int)DAT_000a893c + 1);
  }
  for (uVar1 = param_2 >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
    *DAT_000a8940 = *unaff_ESI;
    unaff_ESI = unaff_ESI + 1;
    DAT_000a8940 = DAT_000a8940 + 1;
  }
  for (uVar1 = param_2 & 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *(undefined1 *)DAT_000a8940 = *(undefined1 *)unaff_ESI;
    unaff_ESI = (undefined4 *)((int)unaff_ESI + 1);
    DAT_000a8940 = (undefined4 *)((int)DAT_000a8940 + 1);
  }
  for (uVar1 = param_2 >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
    *DAT_000a8944 = *unaff_ESI;
    unaff_ESI = unaff_ESI + 1;
    DAT_000a8944 = DAT_000a8944 + 1;
  }
  for (uVar1 = param_2 & 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *(undefined1 *)DAT_000a8944 = *(undefined1 *)unaff_ESI;
    unaff_ESI = (undefined4 *)((int)unaff_ESI + 1);
    DAT_000a8944 = (undefined4 *)((int)DAT_000a8944 + 1);
  }
  for (uVar1 = param_2 >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
    *DAT_000a8948 = *unaff_ESI;
    unaff_ESI = unaff_ESI + 1;
    DAT_000a8948 = DAT_000a8948 + 1;
  }
  for (param_2 = param_2 & 3; param_2 != 0; param_2 = param_2 - 1) {
    *(undefined1 *)DAT_000a8948 = *(undefined1 *)unaff_ESI;
    unaff_ESI = (undefined4 *)((int)unaff_ESI + 1);
    DAT_000a8948 = (undefined4 *)((int)DAT_000a8948 + 1);
  }
  return;
}

