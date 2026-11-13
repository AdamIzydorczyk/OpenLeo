
void __regparm3 FUN_0007aa64(char param_1,int param_2,int param_3,uint param_4)

{
  int iVar1;
  undefined1 uVar2;
  int *unaff_EBX;
  
  if ((param_4 & 0x80) != 0) {
    if (param_3 == 8) {
      if (param_1 == '\0') {
        iVar1 = *unaff_EBX;
        *unaff_EBX = iVar1 + 1;
        *(undefined1 *)(iVar1 + param_2) = 0x30;
        return;
      }
    }
    else if (param_3 == 0x10) {
      iVar1 = *unaff_EBX;
      *unaff_EBX = iVar1 + 1;
      *(undefined1 *)(iVar1 + param_2) = 0x30;
      if ((param_4 & 0x200) == 0) {
        uVar2 = 0x78;
      }
      else {
        uVar2 = 0x58;
      }
      iVar1 = *unaff_EBX;
      *unaff_EBX = iVar1 + 1;
      *(undefined1 *)(iVar1 + param_2) = uVar2;
    }
  }
  return;
}

