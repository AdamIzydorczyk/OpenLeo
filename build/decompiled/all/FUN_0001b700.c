
void FUN_0001b700(void)

{
  char cVar1;
  char cVar2;
  ushort uVar3;
  ushort uVar4;
  char local_1c;
  
  for (uVar4 = 0; uVar4 < 0x40; uVar4 = uVar4 + 1) {
    uVar3 = 0;
    while (uVar3 < 0x100) {
      out(0x3c7,(char)uVar3);
      local_1c = in(0x3c9);
      if (local_1c != '\0') {
        local_1c = local_1c + -1;
      }
      cVar1 = in(0x3c9);
      if (cVar1 != '\0') {
        cVar1 = cVar1 + -1;
      }
      cVar2 = in(0x3c9);
      if (cVar2 != '\0') {
        cVar2 = cVar2 + -1;
      }
      out(0x3c8,(char)uVar3);
      out(0x3c9,local_1c);
      out(0x3c9,cVar1);
      uVar3 = uVar3 + 1;
      out(0x3c9,cVar2);
    }
    FUN_0001b6e0();
  }
  return;
}

