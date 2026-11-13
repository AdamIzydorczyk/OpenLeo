
int __regparm3 FUN_00049870(int param_1,char *param_2)

{
  int iVar1;
  int extraout_ECX;
  char *pcVar2;
  
  iVar1 = (**(code **)**(undefined4 **)(param_1 + 1))();
LAB_00049882:
  do {
    pcVar2 = param_2;
    if (*param_2 != '%') {
      if (*param_2 != '\0') {
        pcVar2 = param_2 + 1;
        if (*pcVar2 == '%') goto LAB_0004989a;
        param_2 = param_2 + 2;
        if (*pcVar2 != '\0') goto LAB_00049882;
      }
      pcVar2 = (char *)0x0;
    }
LAB_0004989a:
    param_2 = pcVar2;
    if (pcVar2 != (char *)0x0) {
      param_2 = pcVar2 + 1;
      if ((pcVar2[1] == '%') || (pcVar2[1] == '*')) {
        param_2 = pcVar2 + 2;
      }
    }
    if (param_2 == (char *)0x0) {
      if (iVar1 == -1) {
        *(undefined4 *)(extraout_ECX + 0xd) = 1;
      }
      return iVar1;
    }
  } while( true );
}

