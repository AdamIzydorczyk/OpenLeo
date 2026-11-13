
/* cdecl (stream,fmt,args...), add esp,0xc; stream obj 0xa92d4 */

undefined8 __regparm3
ll_printf(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4,
         undefined4 param_5,undefined1 param_6)

{
  undefined4 extraout_ECX;
  
  *param_4 = &PTR_LAB_0009632c;
  FUN_00076e83(&param_6,param_2,param_3);
  return CONCAT44(param_2,extraout_ECX);
}

