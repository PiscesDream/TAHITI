
#include <utils/port.h>
#define INT_CTL         0x20	// I/O port for 1st 8259 interrupt controller
#define INT_CTLMASK     0x21	// 0 bits allow IRQ interrupts 
#define INT2_CTL        0xA0	// I/O port for 2nd 8259 interrupt controller
#define INT2_MASK       0xA1	// 0 bits allows IRQ interrupts

#define HD_DATA        0x1f0    // data port for R/W
#define HD_ERROR       0x1f1    // error register
#define HD_SEC_COUNT   0x1f2    // R/W sector count
#define HD_LBA_LOW     0x1f3    // LBA low  byte
#define HD_LBA_MID     0x1f4    // LBA mid  byte
#define HD_LBA_HI      0x1f5    // LBA high byte
#define HD_LBA_DRIVE   0x1f6    // 1B1D0000 => B=LBA, D=drive => 0xE0 or 0xF0
#define HD_CMD         0x1f7    // command : R=0x20 W=0x30
#define HD_STATUS      0x1f7    // status register

#define HD_CONTROL     0x3f6    // 0x08 (0000 1RE0): Reset, E=1: NO interrupt

/* HD disk controller command bytes. */
#define HD_READ        0x20	// read 
#define HD_WRITE       0x30	// write

/* Parameters for the disk drive. */
#define BLOCK_SIZE      4096    // Linux HD block size
#define SECTOR_SIZE      512	// sector size in bytes
#define BAD               -1    // return BAD on error

struct semaphore hd_mutex;     // semaphore for procs hd_rw() ONE at a time
struct semaphore hd_sem;       // sempahore for proc to wait for IDE interrupts

int delay()
{
   int i; for (i=0; i < 10000; i++);
}

int hd_reset()
{
  /****************** HD software reset sequence *******************
   ControlRegister (0x3F6)=(0000 1RE0); R=reset, E=0:enable interrupt
   Strobe R bit from HI to LO; with delay time in between:
          Write 0000 1100 to ControlReg; delay(); 
          Write 0000 1000 to ControlReg; wait for notBUSY & no error
   *****************************************************************/
  out_byte(0x3F6, 0x0C);     delay();
  out_byte(0x3F6, 0x08);     delay();
  if (hd_busy() || cd_error()) {
      printf("HD reset error\n"); return(BAD);
  }
  return 0;     // return 0 means OK 
}

int hd_busy()   // test for BUSY
{
  return in_byte(HD_STATUS) & 0x80;
}

int hd_ready()  // test for READY
{
  return in_byte(HD_STATUS) & 0x40;
}

int hd_drq()   // test for DRQ
{
  return in_byte(HD_STATUS) & 0x08;
}

int hd_error()  // test for error
{
   int r;
   if (in_byte(0x1F7) & 0x01){  // status.ERROR bit on
      r = in_byte(0x1F1);       // read error register
      printf("HD error=%x\n", r);
      return r;
   }
   return 0;                    // return 0 for OK 
}

int hd_init()
{
  printf("hd_init\n");
  hd_mutex.value = 1; 
  hd_mutex.queue = 0;
  hd_sem.value = hd_sem.queue = 0;
}

/* HD interrupt handler : simply "wakeup" the blocked process */ 
int hdhandler()           
{ 
  printf("hd interrupt! ");

  V(&hd_sem);           // wakeup blocked process

  out_byte(0xA0, 0x20);   // enable 8259 controllers
  out_byte(0x20, 0x20);
}


int hd_rw(rw, sector, buf, nsectors) // read/write nsectors
      u16 rw; u32 sector; char *buf; u16 nsectors;
{
    int i;
    P(&hd_mutex);         // procs execute hd_rw() ONE at a time
    hd_sem.value=hd_sem.queue = 0;

    while(hd_busy() || !hd_ready());  // wait until notBUSY & READY

    printf("\nHD NOT_BUSY and READY: write to IDE registers\n");
    
    out_byte(0x3F6, 0x08);                       // control = 0x08; interrupt
    out_byte(0x1F2, nsectors);                   // sector count
    out_byte(0x1F3, sector);                     // LBA low  byte
    out_byte(0x1F4, sector>>8);                  // LBA mid  byte
    out_byte(0x1F5, sector>>16);                 // LBA high byte
    out_byte(0x1F6, ((sector>>24)&0x0F) | 0xE0); // use LBA for drive 0
    out_byte(0x1F7, rw);                         // READ | WRITE command

    // ONE interrupt per sector read|write; transfer data via DATA port
    for (i=0; i < nsectors; i++){  // loop for each sector
      if (rw==HD_READ){
         P(&hd_sem);             // wait for interrupt 

         if (hd_error())
	   break;

         read_port(0x1F0, getds(), buf, 512);   // getds() return DS
         buf += 512;      
      }
      else{  // for DD_WRITE, must wait until notBUSY and DRQ=1
         while (hd_busy() || !hd_drq());

         write_port(0x1F0, getds(), buf, 512);   // getds() returns DS
         buf += 512;      

         P(&hd_sem);            // wait for interrupt

         if (hd_error())
	   break;
      }
    }                           // end loop

    V(&hd_mutex);               // release hd_mutex lock
    if (hd_error())
       return BAD;
    return 0;
}
