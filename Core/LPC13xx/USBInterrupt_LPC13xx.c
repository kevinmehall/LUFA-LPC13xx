/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#define  __INCLUDE_FROM_USB_DRIVER
#include "../USBInterrupt.h"
#include "reg.h"

void USB_INT_DisableAllInterrupts(void)
{
	
}

void USB_INT_ClearAllInterrupts(void)
{
	
}

void USB_IRQHandler (void)
{
  uint32_t status_reg, val, n, m;

  status_reg = USB_DEVINTST;
  USB_DEVINTCLR = status_reg;

  /* Device Status Interrupt (Reset, Connect change, Suspend/Resume) */
  if (status_reg & DEV_STAT_INT) 
  {
    WriteCommand(CMD_GET_DEV_STAT);
    val = ReadCommandData(DAT_GET_DEV_STAT);
    if (val & DEV_RST) {                    /* Reset */
		USB_DEVINTCLR = 0x000FFFFF;
		USB_DEVINTEN  = DEV_STAT_INT | (0xFF<<1);
		EVENT_USB_Device_Reset();
    }
    if (val & DEV_CON_CH) {                 /* Connect change */
		if (USB_VBUS_GetStatus())
		{
			USB_DeviceState = DEVICE_STATE_Powered;
			EVENT_USB_Device_Connect();
		}
		else
		{
			USB_DeviceState = DEVICE_STATE_Unattached;
			EVENT_USB_Device_Disconnect();
		}
    }
    if (val & DEV_SUS_CH) {                 /* Suspend/Resume */
      if (val & DEV_SUS) {                  /* Suspend */
		USB_DeviceState = DEVICE_STATE_Suspended;
		EVENT_USB_Device_Suspend();
      } else {                              /* Resume */
		if (USB_ConfigurationNumber)
		  USB_DeviceState = DEVICE_STATE_Configured;
		else
		  USB_DeviceState = DEVICE_STATE_Powered; //(USB_Device_IsAddressSet()) ? DEVICE_STATE_Configured : DEVICE_STATE_Powered;

		EVENT_USB_Device_WakeUp();
      }
    }
    return;
  }
  
  
	#if !defined(NO_SOF_EVENTS)
	if (status_reg & FRAME_INT)
	{
		USB_DEVINTCLR = FRAME_INT;
		EVENT_USB_Device_StartOfFrame();
		//SOFIRQCount++;
	}
	#endif



#if USB_ERROR_EVENT
  /* NO error interrupt anymore, below code can be used
  as example to get error status from command engine. */
  /* Error Interrupt */
  if (status_reg & ERR_INT) 
  {
    WrCmd(CMD_RD_ERR_STAT);
    val = RdCmdDat(DAT_RD_ERR_STAT);
    USB_Error_Event(val);
  }
#endif

  if (status_reg & (0xFF<<1)) {
	/* if any of the EP0 through EP7 is set, or bit 1 through 9 on status_reg */
    for (n = 0; n < USB_EP_NUM; n++) {      /* Check All Endpoints */
	  if ((status_reg>>1) & (1 << n)) {
        m = n >> 1;
		/* clear EP interrupt by sending cmd to the command engine. */
		WriteCommand(CMD_SEL_EP_CLRI(n));
		val = ReadCommandData(DAT_SEL_EP_CLRI(n));
        if ((n & 1) == 0) {                 /* OUT Endpoint */
          if (n == 0) {                     /* Control OUT Endpoint */
            if (val & EP_SEL_STP) {         /* Setup Packet */
                Endpoint_flags[m].setup = 1;
            }
          }
          Endpoint_flags[m].out = 1;
        } else {                            /* IN Endpoint */
          Endpoint_flags[m].in = 1;
        }
      }
    }
  }
}


#if 0
	if (USB_INT_HasOccurred(USB_INT_EORSTI) && USB_INT_IsEnabled(USB_INT_EORSTI))
	{
		USB_INT_Clear(USB_INT_EORSTI);

		USB_DeviceState         = DEVICE_STATE_Default;
		USB_ConfigurationNumber = 0;

		USB_INT_Clear(USB_INT_SUSPI);
		USB_INT_Disable(USB_INT_SUSPI);
		USB_INT_Enable(USB_INT_WAKEUPI);


	}
#endif
