
void __regparm3 FUN_00078b55(byte param_1,byte param_2)

{
  byte bVar1;
  char *pcVar2;
  char *pcVar3;
  
  while( true ) {
    pcVar2 = &DAT_000ad94e;
    bVar1 = param_1;
    for (pcVar3 = &DAT_000ad93c; pcVar3 < &DAT_000ad94e; pcVar3 = pcVar3 + 6) {
      if ((*pcVar3 != '\x02') && (bVar1 <= (byte)pcVar3[1])) {
        bVar1 = pcVar3[1];
        pcVar2 = pcVar3;
      }
    }
    if (pcVar2 == &DAT_000ad94e) break;
    if ((bVar1 <= param_2) && (*(code **)(pcVar2 + 2) != (code *)0x0)) {
      (**(code **)(pcVar2 + 2))();
    }
    *pcVar2 = '\x02';
  }
  return;
}

