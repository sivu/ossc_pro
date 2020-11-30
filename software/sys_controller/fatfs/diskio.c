/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "mmc.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "av_controller.h"

extern struct mmc *mmc_dev;

static uint8_t is_sdcard_present()
{
	return !!(IORD_ALTERA_AVALON_PIO_DATA(PIO_2_BASE) & (1<<SSTAT_SD_DETECT_BIT));
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	if (mmc_dev->has_init != 0)
	{
		return is_sdcard_present() != 0 ? RES_OK : STA_NODISK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
    int err = mmc_init(mmc_dev);

    if (err != 0 || mmc_dev->has_init == 0) 
	{
        printf("mmc_init failed: %d\n\n", err);
		return STA_NOINIT;
	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	if (mmc_dev->has_init == 0 || is_sdcard_present() == 0)
		return RES_NOTRDY;

	if (mmc_bread(mmc_dev,(LBA_t)sector,(UINT)count,buff) == count)
		return RES_OK;

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if (mmc_dev->has_init == 0 || is_sdcard_present() == 0)
		return RES_NOTRDY;

	if (mmc_bwrite(mmc_dev,(LBA_t)sector,(UINT)count,buff) == count)
		return RES_OK;

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	if (mmc_dev->has_init == 0 || is_sdcard_present() == 0)
		return RES_NOTRDY;

	return RES_PARERR;
}

