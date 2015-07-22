import re
import struct

cdef extern from "os2.h":
 pass
cdef extern from "stdio.h":
 ctypedef int size_t
cdef extern from "string.h":
 char * strncpy ( char *dest, char *src, size_t anzahl )
cdef extern from "k8055lib.h":
 ctypedef unsigned long ULONG
 ctypedef unsigned long APIRET
 ctypedef char CHAR
 ctypedef unsigned char BYTE
 APIRET __stdcall K8055_Open( CHAR *pcaDrivername, ULONG *pulFileDesc )
 APIRET __stdcall K8055_Init( ULONG *pulFileDesc )
 APIRET __stdcall K8055_Read( ULONG *pulFileDesc, BYTE byNumberOfBytes, BYTE *pbyaGetData )
 APIRET __stdcall K8055_Write( ULONG *pulFileDesc, BYTE byToWrite, BYTE *toData       )
 APIRET __stdcall K8055_Close( ULONG *pulFileDesc )


cdef class K8055LibClass:
 cdef unsigned long ulDevHandle
 cdef unsigned long iRet
 cdef int iWasInit
 cdef int myinit
 cdef char * pcDevName
 def __cinit__(self,devname):
  print("cinit")
  self.pcDevName = devname
  iRet =  K8055_Open(self.pcDevName,&self.ulDevHandle)
  K8055_Init(&self.ulDevHandle)
  return
 def __init__(self,devname):
  print("Init")
  self.iWasInit = 1
  self.myinit = 1
  return
 def __dealloc__(self):
  K8055_Close(&self.ulDevHandle)
  return
 def ReadasString(self,readdata):
  print readdata
  cdef BYTE abyRead[8]
  strncpy(<char *> &abyRead[0],readdata,8)
  K8055_Read(&self.ulDevHandle,8,&abyRead[0])
  strread = ""
  i = 0
  for i in 7:
    strread += abyRead[i]
  return strread
 def ReadData(self):
  Erg=[0,0,0,0]
  cdef BYTE abyRead[8]
  K8055_Read(&self.ulDevHandle,8,&abyRead[0])
  Erg[0] = abyRead[0]
  Erg[1] = abyRead[1]
  Erg[2] = abyRead[2]
  Erg[3] = abyRead[3]
  return Erg
 def WriteData(self,byOut,byDac1,byDac2):
  cdef BYTE abyWrite[8]
  abyWrite[0] = 0x05
  abyWrite[1] = byOut
  abyWrite[2] = byDac1
  abyWrite[3] = byDac2
  abyWrite[4] = 0x00
  abyWrite[5] = 0x00
  abyWrite[6] = 0x00
  abyWrite[7] = 0x00
  K8055_Write(&self.ulDevHandle,8,&abyWrite[0])
  return
 def WriteasString(self,writedata):
  print writedata
  cdef BYTE abyWrite[8]
  i = 0
  for i in 7:
     abyWrite[i] = writedata[i]
  K8055_Write(&self.ulDevHandle,8,&abyWrite[0])
  strwrite = ""
  i=0
  for i in 7:
    strwrite += abyWrite[i]
  return strwrite

