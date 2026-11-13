
void __regparm3 FUN_0001a998(undefined4 param_1,uint param_2)

{
  undefined1 uVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined1 *unaff_ESI;
  
  uVar1 = *unaff_ESI;
  uVar2 = CONCAT11(uVar1,uVar1);
  for (uVar3 = param_2 >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
    *DAT_000a893c = uVar2;
    DAT_000a893c = DAT_000a893c + 1;
  }
  for (uVar3 = (uint)((param_2 & 1) != 0); uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined1 *)DAT_000a893c = uVar1;
    DAT_000a893c = (undefined2 *)((int)DAT_000a893c + 1);
  }
  for (uVar3 = param_2 >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
    *DAT_000a8940 = uVar2;
    DAT_000a8940 = DAT_000a8940 + 1;
  }
  for (uVar3 = (uint)((param_2 & 1) != 0); uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined1 *)DAT_000a8940 = uVar1;
    DAT_000a8940 = (undefined2 *)((int)DAT_000a8940 + 1);
  }
  for (uVar3 = param_2 >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
    *DAT_000a8944 = uVar2;
    DAT_000a8944 = DAT_000a8944 + 1;
  }
  for (uVar3 = (uint)((param_2 & 1) != 0); uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined1 *)DAT_000a8944 = uVar1;
    DAT_000a8944 = (undefined2 *)((int)DAT_000a8944 + 1);
  }
  for (uVar3 = param_2 >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
    *DAT_000a8948 = uVar2;
    DAT_000a8948 = DAT_000a8948 + 1;
  }
  for (uVar3 = (uint)((param_2 & 1) != 0); uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined1 *)DAT_000a8948 = uVar1;
    DAT_000a8948 = (undefined2 *)((int)DAT_000a8948 + 1);
  }
  return;
}

