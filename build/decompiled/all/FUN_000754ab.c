
char * __regparm3 FUN_000754ab(int param_1)

{
  char *pcVar1;
  
  pcVar1 = (char *)**(undefined4 **)(param_1 + 0x14);
  if (*pcVar1 == '\x02') {
    pcVar1 = *(char **)(pcVar1 + 1);
  }
  return pcVar1;
}

