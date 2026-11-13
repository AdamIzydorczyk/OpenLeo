
longlong __regparm3 FUN_00033370(undefined4 param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_00076a69();
  if (iVar1 == 0) {
    iVar1 = FUN_0005c320();
    if (iVar1 != 0) {
      uVar2 = FUN_00032530();
      return CONCAT44(param_2,uVar2);
    }
  }
  else {
    iVar1 = FUN_00076a69();
    if (iVar1 == 0) {
      iVar1 = FUN_0005c320();
      if (iVar1 != 0) {
        iVar1 = FUN_00053ff0();
        *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009d088;
        *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009d080;
        return CONCAT44(param_2,iVar1);
      }
    }
    else {
      iVar1 = FUN_00076a69();
      if (iVar1 == 0) {
        iVar1 = FUN_0005c320();
        if (iVar1 != 0) {
          iVar1 = FUN_000531f0();
          *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009d168;
          *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009d160;
          return CONCAT44(param_2,iVar1);
        }
      }
      else {
        iVar1 = FUN_00076a69();
        if (iVar1 == 0) {
          iVar1 = FUN_0005c320();
          if (iVar1 != 0) {
            iVar1 = FUN_00053ff0();
            *(undefined4 *)(iVar1 + 0xee) = 0;
            *(undefined4 *)(iVar1 + 0xf2) = 0;
            *(undefined4 *)(iVar1 + 0xf6) = 0;
            *(undefined4 *)(iVar1 + 0xfa) = 0;
            *(undefined4 *)(iVar1 + 0xfe) = 0;
            *(undefined4 *)(iVar1 + 0x102) = 0;
            *(undefined4 *)(iVar1 + 0x106) = 0;
            *(undefined4 *)(iVar1 + 0x10a) = 0;
            *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009d460;
            *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009d458;
            return CONCAT44(param_2,iVar1);
          }
        }
        else {
          iVar1 = FUN_00076a69();
          if (iVar1 == 0) {
            iVar1 = FUN_0005c320();
            if (iVar1 != 0) {
              uVar2 = FUN_0003d8c0();
              return CONCAT44(param_2,uVar2);
            }
          }
          else {
            iVar1 = FUN_00076a69();
            if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
              iVar1 = FUN_000531f0();
              *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009cfd0;
              *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009cfc8;
              return CONCAT44(param_2,iVar1);
            }
          }
        }
      }
    }
  }
  return (ulonglong)param_2 << 0x20;
}

