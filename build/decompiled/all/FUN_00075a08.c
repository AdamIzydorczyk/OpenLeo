
void __regparm3 FUN_00075a08(int param_1,undefined1 param_2)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  char *pcVar4;
  byte bVar5;
  char local_38 [40];
  
  bVar5 = 0;
  switch(param_2) {
  case 2:
    FUN_0007546f();
  case 1:
    FUN_0007555c();
  case 3:
    if ((*(byte *)(param_1 + 0x24) & 2) == 0) {
      *(char **)(*(int *)(param_1 + 8) + 0xc) = s_no_handler_active_to_catch_throw_00095e25;
      FUN_00075384();
    }
    else {
      FUN_00077bdc();
    }
  case 4:
    pcVar4 = local_38;
    pcVar3 = s_system_exception__code___0x00000_00095e4f;
    do {
      cVar1 = *pcVar3;
      *pcVar4 = cVar1;
      if (cVar1 == '\0') break;
      cVar1 = pcVar3[1];
      pcVar3 = pcVar3 + 2;
      pcVar4[1] = cVar1;
      pcVar4 = pcVar4 + 2;
    } while (cVar1 != '\0');
    iVar2 = -1;
    pcVar4 = local_38;
    do {
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      cVar1 = *pcVar4;
      pcVar4 = pcVar4 + (uint)bVar5 * -2 + 1;
    } while (cVar1 != '\0');
    FUN_000796d3();
    FUN_00077bdc();
  }
  FUN_0007539b();
  return;
}

