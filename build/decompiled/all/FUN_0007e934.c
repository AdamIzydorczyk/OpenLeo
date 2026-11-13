
void __regparm3 FUN_0007e934(int param_1,int param_2,undefined4 param_3)

{
  byte extraout_CH;
  undefined4 *unaff_EBX;
  
  if ((*(ushort *)(param_1 + 8) & 0x7fff) == 0) {
    if ((*(byte *)(param_2 + 9) & 0x80) == 0) {
      *(undefined2 *)(unaff_EBX + 2) = 0;
      unaff_EBX[1] = 0;
      *unaff_EBX = 0;
    }
    else {
      *(undefined2 *)(unaff_EBX + 2) = 0x4000;
      unaff_EBX[1] = 0xc90fdaa2;
      *unaff_EBX = 0x2168c235;
    }
  }
  else if ((*(ushort *)(param_2 + 8) & 0x7fff) == 0) {
    *(ushort *)(unaff_EBX + 2) = *(ushort *)(param_1 + 8) & 0x8000 | 0x3fff;
    unaff_EBX[1] = 0xc90fdaa2;
    *unaff_EBX = 0x2168c235;
  }
  else {
    FUN_0007e117(param_3);
    FUN_0007e9ea();
    if ((short)((ushort)extraout_CH << 8) < 0) {
      if ('\0' < *(char *)((int)unaff_EBX + 9)) {
        FUN_0007dee2(0x2168c235,0xc90fdaa2,0xc000);
      }
    }
    else if (*(char *)((int)unaff_EBX + 9) < '\0') {
      FUN_0007dee2(0x2168c235,0xc90fdaa2,0x4000);
    }
  }
  return;
}

