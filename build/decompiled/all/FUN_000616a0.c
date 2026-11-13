
longlong __regparm3 FUN_000616a0(undefined4 param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_00076a69();
  if (iVar1 == 0) {
    iVar1 = FUN_0005c320();
    if (iVar1 != 0) {
      uVar2 = FUN_00062e40();
      return CONCAT44(param_2,uVar2);
    }
  }
  else {
    iVar1 = FUN_00076a69();
    if (iVar1 == 0) {
      iVar1 = FUN_0005c320();
      if (iVar1 != 0) {
        uVar2 = FUN_00061fb0();
        return CONCAT44(param_2,uVar2);
      }
    }
    else {
      iVar1 = FUN_00076a69();
      if (iVar1 == 0) {
        iVar1 = FUN_0005c320();
        if (iVar1 != 0) {
          uVar2 = FUN_00064830();
          return CONCAT44(param_2,uVar2);
        }
      }
      else {
        iVar1 = FUN_00076a69();
        if (iVar1 == 0) {
          iVar1 = FUN_0005c320();
          if (iVar1 != 0) {
            uVar2 = FUN_00061890();
            return CONCAT44(param_2,uVar2);
          }
        }
        else {
          iVar1 = FUN_00076a69();
          if (iVar1 == 0) {
            iVar1 = FUN_0005c320();
            if (iVar1 != 0) {
              uVar2 = FUN_000626c0();
              return CONCAT44(param_2,uVar2);
            }
          }
          else {
            iVar1 = FUN_00076a69();
            if (iVar1 == 0) {
              iVar1 = FUN_0005c320();
              if (iVar1 != 0) {
                uVar2 = FUN_00063750();
                return CONCAT44(param_2,uVar2);
              }
            }
            else {
              iVar1 = FUN_00076a69();
              if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
                iVar1 = FUN_00053ff0();
                *(undefined ***)(iVar1 + 8) = &PTR_LAB_00098078;
                *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00098070;
                return CONCAT44(param_2,iVar1);
              }
            }
          }
        }
      }
    }
  }
  return (ulonglong)param_2 << 0x20;
}

