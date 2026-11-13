
int FUN_0001a5d0(void)

{
  code *pcVar1;
  undefined2 uVar2;
  uint in_ECX;
  undefined2 extraout_DX;
  undefined2 *local_c;
  
  pcVar1 = (code *)swi(0x31);
  uVar2 = (*pcVar1)();
  local_c = (undefined2 *)CONCAT22(local_c._2_2_,extraout_DX);
  *local_c = uVar2;
  return (in_ECX & 0xffff) << 4;
}

