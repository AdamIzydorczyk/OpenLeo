
longlong __regparm3 FUN_0005d830(undefined4 param_1,uint param_2)

{
  undefined4 uVar1;
  char cVar2;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 local_28;
  undefined4 local_20;
  int local_1c;
  undefined4 local_18;
  
switchD_0005d8dc_caseD_1:
  uVar1 = 0;
switchD_0005d8dc_default:
  if ((char)uVar1 == '\0') goto LAB_0005d886;
  do {
    cVar2 = (char)uVar1;
    do {
      if (cVar2 == '\x01') {
        FUN_0005d170();
        uVar1 = extraout_ECX_00;
        switch(local_18) {
        case 1:
          goto switchD_0005d8dc_caseD_1;
        case 2:
switchD_0005d8dc_caseD_2:
          uVar1 = FUN_0005d2f0();
          return CONCAT44(param_2,uVar1);
        case 3:
          goto switchD_0005d8dc_caseD_3;
        case 4:
          goto LAB_0005d95a;
        }
        goto switchD_0005d8dc_default;
      }
      if (cVar2 == '\x02') {
        FUN_0005d170();
        uVar1 = extraout_ECX_01;
        switch(local_28) {
        case 1:
          goto switchD_0005d8dc_caseD_1;
        case 2:
          goto switchD_0005d8dc_caseD_2;
        case 3:
          FUN_0005da60();
          FUN_0005d050();
          FUN_0005d050();
          FUN_0004a230();
        case 4:
          return (ulonglong)param_2 << 0x20;
        }
        goto switchD_0005d8dc_default;
      }
    } while (cVar2 != '\0');
LAB_0005d886:
    FUN_0005d170();
    uVar1 = extraout_ECX;
    switch(local_20) {
    case 1:
      if (local_1c == 1) {
        uVar1 = 1;
      }
      else {
switchD_0005d8dc_caseD_3:
        uVar1 = 2;
      }
      break;
    case 2:
      ll_printf(&DAT_000a92d4,s_Lew_zszedl_w_Junction___00092676);
      ll_rethrow_report();
    case 4:
LAB_0005d95a:
      return (ulonglong)param_2 << 0x20;
    case 3:
      uVar1 = 1;
      break;
    default:
      goto switchD_0005d8dc_default;
    }
  } while( true );
}

