

Method(HPTS,0,NotSerialized)
{
  OperationRegion(YPTS, SystemIO, 0x80, 0x01) 
  Field(YPTS, ByteAcc, NoLock, Preserve)
  {
    PTS8, 8,
   
  }
  Store(0xcc,PTS8)  
  Sleep(5000)

}
Method(HWAK,0,NotSerialized)
{
  OperationRegion(YWAK, SystemIO, 0x80, 0x1) 
  Field(YWAK, ByteAcc, NoLock, Preserve)
  {
   WAK8, 8,
   
  }
  Store(0xEE,WAK8)
    Sleep(5000)

}