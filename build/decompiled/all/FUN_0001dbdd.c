
bool FUN_0001dbdd(void)

{
  code *pcVar1;
  int unaff_ESI;
  undefined1 in_CF;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined1 auStack_10 [12];
  
  puVar3 = auStack_10 + 0xc;
  puVar2 = auStack_10 + 0xc;
  pcVar1 = (code *)swi(0x31);
  (*pcVar1)();
  if (!(bool)in_CF) {
    in_CF = unaff_ESI << 0xf < 0;
    pcVar1 = (code *)swi(0x31);
    (*pcVar1)(puVar2,puVar3);
  }
  return !(bool)in_CF;
}

