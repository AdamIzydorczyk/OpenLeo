
void __regparm3 FUN_0007569a(undefined4 param_1,int param_2)

{
  code *pcVar1;
  char *pcVar2;
  
  pcVar2 = *(char **)(param_2 + 8);
  if (*pcVar2 == '\x02') {
    pcVar2 = *(char **)(pcVar2 + 1);
  }
  if (((*pcVar2 == '\0') || (*pcVar2 == '\x04')) &&
     (pcVar1 = *(code **)(pcVar2 + 9), pcVar1 != (code *)0x0)) {
    *(undefined1 *)(param_2 + 0x1c) = 6;
    (*pcVar1)();
  }
  FUN_0007961b();
  return;
}

