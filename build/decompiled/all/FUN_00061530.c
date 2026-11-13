
int FUN_00061530(void)

{
  int iVar1;
  
  iVar1 = FUN_00076a69();
  if (iVar1 == 0) {
    iVar1 = FUN_0005c320();
    if (iVar1 != 0) {
      iVar1 = FUN_000524e0();
      *(undefined ***)(iVar1 + 0x44) = &PTR_LAB_000a2ed4;
      return iVar1;
    }
  }
  else {
    iVar1 = FUN_00076a69();
    if (iVar1 == 0) {
      iVar1 = FUN_0005c320();
      if (iVar1 != 0) {
        iVar1 = FUN_000524e0();
        *(undefined ***)(iVar1 + 0x44) = &PTR_LAB_000a2ed4;
        return iVar1;
      }
    }
    else {
      iVar1 = FUN_00076a69();
      if (iVar1 == 0) {
        iVar1 = FUN_0005c320();
        if (iVar1 != 0) {
          iVar1 = FUN_000524e0();
          *(undefined ***)(iVar1 + 0x44) = &PTR_LAB_000a2e9c;
          return iVar1;
        }
      }
      else {
        iVar1 = FUN_00076a69();
        if (iVar1 == 0) {
          iVar1 = FUN_0005c320();
          if (iVar1 != 0) {
            iVar1 = FUN_000524e0();
            *(undefined ***)(iVar1 + 0x44) = &PTR_LAB_0009a8cc;
            return iVar1;
          }
        }
        else {
          iVar1 = FUN_00076a69();
          if (iVar1 == 0) {
            iVar1 = FUN_0005c320();
            if (iVar1 != 0) {
              iVar1 = FUN_000524e0();
              *(undefined ***)(iVar1 + 0x44) = &PTR_LAB_0009a894;
              return iVar1;
            }
          }
          else {
            iVar1 = FUN_00076a69();
            if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
              iVar1 = FUN_000524e0();
              *(undefined ***)(iVar1 + 0x44) = &PTR_LAB_000a2e9c;
              return iVar1;
            }
          }
        }
      }
    }
  }
  return 0;
}

