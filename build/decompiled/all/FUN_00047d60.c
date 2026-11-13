
void __regparm3 FUN_00047d60(int param_1,undefined4 param_2)

{
  byte bVar1;
  byte *pbVar2;
  undefined **ppuVar3;
  char local_18 [4];
  
  *(undefined1 *)(param_1 + 0xc) = 0;
  FUN_000749a0();
  pbVar2 = (byte *)0x0;
  ppuVar3 = &PTR_FUN_000961fc;
  do {
    FUN_00057d30(pbVar2,ppuVar3);
    bVar1 = *pbVar2;
    if (((&DAT_000a68b4)[(byte)(bVar1 + 1)] & 0xc0) == 0) {
      if (bVar1 < 0x2a) {
        if (bVar1 == 0x21) {
          *(undefined1 *)(param_1 + 0xc) = 1;
        }
      }
      else if (bVar1 < 0x2b) {
        *(undefined1 *)(param_1 + 0xc) = 2;
      }
      else if (bVar1 == 0x7c) {
        *(undefined1 *)(param_1 + 0xc) = 3;
      }
      FUN_00057e70();
    }
    FUN_00048000();
    FUN_00049920(param_2,&DAT_000919c7,local_18);
  } while (local_18[0] == ',');
  FUN_00057a00();
  return;
}

