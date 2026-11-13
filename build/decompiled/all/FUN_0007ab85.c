
/* WARNING: Removing unreachable block (ram,0x0007ae3b) */
/* WARNING: Removing unreachable block (ram,0x0007ae54) */
/* WARNING: Removing unreachable block (ram,0x0007ad05) */
/* WARNING: Removing unreachable block (ram,0x0007aca0) */
/* WARNING: Removing unreachable block (ram,0x0007acbf) */
/* WARNING: Removing unreachable block (ram,0x0007acde) */
/* WARNING: Removing unreachable block (ram,0x0007acc6) */
/* WARNING: Removing unreachable block (ram,0x0007accf) */
/* WARNING: Removing unreachable block (ram,0x0007aca8) */
/* WARNING: Removing unreachable block (ram,0x0007ac20) */
/* WARNING: Removing unreachable block (ram,0x0007ac3e) */
/* WARNING: Removing unreachable block (ram,0x0007ac83) */
/* WARNING: Removing unreachable block (ram,0x0007ac45) */
/* WARNING: Removing unreachable block (ram,0x0007ac28) */
/* WARNING: Removing unreachable block (ram,0x0007acef) */
/* WARNING: Removing unreachable block (ram,0x0007ac32) */
/* WARNING: Removing unreachable block (ram,0x0007aed6) */
/* WARNING: Removing unreachable block (ram,0x0007aee4) */
/* WARNING: Removing unreachable block (ram,0x0007aefd) */

undefined4 __regparm3 FUN_0007ab85(undefined4 param_1,char *param_2,code *param_3)

{
  int iVar1;
  int iVar2;
  char cVar3;
  undefined4 *puVar4;
  char *pcVar5;
  char *extraout_ECX;
  undefined4 *unaff_EBX;
  bool bVar6;
  int local_30;
  int local_2c;
  int local_28;
  char local_1e;
  char local_1d [5];
  code *local_18;
  undefined1 local_14;
  
  local_14 = 0;
  local_18 = param_3;
  while( true ) {
    while( true ) {
      cVar3 = *param_2;
      if (cVar3 == '\0') {
        return 0;
      }
      param_2 = param_2 + 1;
      if (cVar3 == '%') break;
      (*local_18)();
    }
    param_2 = (char *)FUN_0007af24();
    cVar3 = *param_2;
    param_2 = param_2 + 1;
    if (cVar3 == '\0') break;
    if (cVar3 == 'n') {
      puVar4 = (undefined4 *)*unaff_EBX;
      *unaff_EBX = puVar4 + 1;
      *(undefined4 *)*puVar4 = 0;
    }
    else {
      FUN_0007b262();
      if (local_1e == ' ') {
        while (local_30 = local_30 + -1, -1 < local_30) {
          (*local_18)();
        }
      }
      pcVar5 = local_1d;
      while (*pcVar5 != '\0') {
        (*local_18)();
        pcVar5 = extraout_ECX;
      }
      while (iVar1 = local_28 + -1, local_28 != 0) {
        (*local_18)();
        local_28 = iVar1;
      }
      if (local_1e != ' ') {
        while (local_30 = local_30 + -1, -1 < local_30) {
          (*local_18)();
        }
      }
      if ((cVar3 == 's') || (cVar3 == 'S')) {
        while (iVar2 = local_2c + -1, bVar6 = local_2c != 0, local_2c = iVar2, local_28 = iVar1,
              bVar6) {
          (*local_18)();
        }
      }
      else {
        while (iVar2 = local_2c + -1, bVar6 = local_2c != 0, local_2c = iVar2, local_28 = iVar1,
              bVar6) {
          (*local_18)();
        }
      }
    }
  }
  return 0;
}

