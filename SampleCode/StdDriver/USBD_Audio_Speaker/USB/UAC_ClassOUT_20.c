#include "NUC505Series.h"

#include "AudioLib.h"
#include "usbd_audio_20.h"

void UAC_ClassOUT_20(S_AUDIO_LIB* psAudioLib)
{
    uint32_t volatile u32timeout = 0x100000;
    /* To make sure that no DMA is reading the Endpoint Buffer (4-8 & 4-5)*/
    while(1)
    {
        if (!(USBD->DMACTL & USBD_DMACTL_DMAEN_Msk))
            break;

        if (!USBD_IS_ATTACHED())
            break;

        if(u32timeout == 0)
        {
            printf("EPA\t%x\n", USBD->EP[EPA].EPDATCNT);
            printf("EPB\t%x\n", USBD->EP[EPB].EPDATCNT);
            printf("EPC\t%x\n", USBD->EP[EPC].EPDATCNT);
            printf("DMACTL\t%X\n", USBD->DMACTL);
            printf("DMACNT\t%X\n", USBD->DMACNT);
            u32timeout = 0x100000;
        }
        else
            u32timeout--;
    }

    /* Host to device */
    if(CLOCK_SOURCE_ID == ((gUsbCmd.wIndex >> 8) & 0xff))
    {
        if(gUsbCmd.bRequest== FREQ_CONTROL)
        {
            USBD_CtrlOut((uint8_t *)&psAudioLib->m_u32PlaySampleRate, gUsbCmd.wLength);
        }
        USBD_SET_CEP_STATE(USB_CEPCTL_NAKCLR);
#if CONFIG_AUDIO_PLAY
        psAudioLib->m_pfnPlayConfigMaxPayload20( psAudioLib );
        USBD_SET_MAX_PAYLOAD(EPB, psAudioLib->m_u16PlayMaxPayload2_);
        printf("hdP\t%d\n", psAudioLib->m_u32PlaySampleRate);
#endif  // CONFIG_AUDIO_PLAY
    }
    else
        switch (gUsbCmd.bRequest)
        {
        case UAC_SET_CUR:
        {
            switch ((gUsbCmd.wValue & 0xff00) >> 8)
            {
            case MUTE_CONTROL:
                if (PLAY_FEATURE_UNITID == ((gUsbCmd.wIndex >> 8) & 0xff))
                {
                    USBD_CtrlOut((uint8_t *)&psAudioLib->m_u8PlayMute, gUsbCmd.wLength);
                    //printf("hdPm\t%d\n", psAudioLib->m_u8PlayMute);
                }
                /* Status stage */
                USBD_SET_CEP_STATE(USB_CEPCTL_NAKCLR);
                //printf("SET MUTE_CONTROL\n");
                break;

            case VOLUME_CONTROL:
                if (PLAY_FEATURE_UNITID == ((gUsbCmd.wIndex >> 8) & 0xff))
                {
                    if (((gUsbCmd.wValue) & 0xff) == 1)
                    {
                        USBD_CtrlOut((uint8_t *)&psAudioLib->m_i16PlayVolumeL, gUsbCmd.wLength);
                        /* Status stage */
                        USBD_SET_CEP_STATE(USB_CEPCTL_NAKCLR);
                        //printf("hdPl\t0x%04X\n", (uint16_t)psAudioLib->m_i16PlayVolumeL);
                    }
                    else
                    {
                        USBD_CtrlOut((uint8_t *)&psAudioLib->m_i16PlayVolumeR, gUsbCmd.wLength);
                        /* Status stage */
                        USBD_SET_CEP_STATE(USB_CEPCTL_NAKCLR);
                        //printf("hdPr\t0x%04X\n", (uint16_t)psAudioLib->m_i16PlayVolumeR);
                    }
                }
                break;
            default:
                /* STALL control pipe */
                USBD_SET_CEP_STATE(USBD_CEPCTL_STALLEN_Msk);
                break;
            }
            break;
        }
        default:
        {
            USBD->CEPCTL = USBD_CEPCTL_FLUSH_Msk;
            /* Setup error, stall the device */
            USBD_SET_CEP_STATE(USBD_CEPCTL_STALLEN_Msk);
            break;
        }
        }
}
