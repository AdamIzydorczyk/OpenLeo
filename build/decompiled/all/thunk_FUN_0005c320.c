
/* WARNING: Removing unreachable block (ram,0x0005c35d) */

undefined8 __regparm3 thunk_FUN_0005c320(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  
  iVar1 = 0;
  if ((param_1 != 0) && (iVar1 = crt_malloc(), iVar1 == 0)) {
    uVar2 = FUN_00077a14();
    uVar3 = FUN_00077a50(uVar2);
    ll_printf(&DAT_000a92d4,s_Mem_Out___d__mem___d__max___d_000924f4,(int)((ulonglong)uVar3 >> 0x20)
              ,(int)uVar3);
    ll_rethrow_report();
  }
  return CONCAT44(param_2,iVar1);
}

