
undefined4 __regparm3 FUN_0007a5bd(undefined4 param_1,uint param_2)

{
  code *pcVar1;
  undefined4 unaff_EDI;
  byte bVar2;
  
  bVar2 = (param_2 >> 0xf & 1) != 0;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bVar2 & 1) != 0) {
    FUN_0007ba5f();
    unaff_EDI = 0xffffffff;
  }
  return unaff_EDI;
}

