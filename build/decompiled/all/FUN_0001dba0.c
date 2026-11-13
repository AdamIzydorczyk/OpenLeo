
void FUN_0001dba0(void)

{
  byte bVar1;
  
  do {
    bVar1 = in(0x3da);
  } while ((bVar1 & 8) == 0);
  do {
    bVar1 = in(0x3da);
  } while ((bVar1 & 8) != 0);
  return;
}

