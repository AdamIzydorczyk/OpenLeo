
void __regparm3 FUN_0005c6c0(int param_1,int param_2)

{
  byte bVar1;
  char *pcVar2;
  int iVar3;
  undefined4 *puVar4;
  
  iVar3 = 1;
  if (param_1 < 2) {
    return;
  }
  puVar4 = (undefined4 *)(param_2 + 4);
  do {
    while (pcVar2 = (char *)*puVar4, *pcVar2 != '-') {
LAB_0005c6e5:
      iVar3 = iVar3 + 1;
      puVar4 = puVar4 + 1;
      if (param_1 <= iVar3) {
        return;
      }
    }
    if (pcVar2[1] == 's') {
      bVar1 = pcVar2[2];
      if (bVar1 < 0x31) {
        if (bVar1 == 0x30) {
          DAT_000cf4c0 = 2;
        }
      }
      else if (bVar1 < 0x32) {
        DAT_000cf4c0 = 0x10;
      }
      else if (bVar1 == 0x32) {
        DAT_000cf4c0 = 0x20;
      }
      goto LAB_0005c6e5;
    }
    iVar3 = iVar3 + 1;
    puVar4 = puVar4 + 1;
    if (param_1 <= iVar3) {
      return;
    }
  } while( true );
}

