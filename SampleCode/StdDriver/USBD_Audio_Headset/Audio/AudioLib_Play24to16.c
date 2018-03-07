#include "NUC505Series.h"

#include "AudioLib.h"
#include "AudioLib2.h"

#if CONFIG_AUDIO_PLAY
void _UAC_SpkRecvFrom24to16(S_AUDIO_LIB* psAudioLib, int32_t i32Len)
{
    /* executed in USB IRQ */
    
    int32_t i, i32PlayPcmTmpSmplCnt;
    
    int16_t *pi16PlayPcmWorkBuf;
    uint8_t *pu8PlayPcmTmpBuf;
    
    pi16PlayPcmWorkBuf = (int16_t *)psAudioLib->m_pu8PlayPcmWorkBuf;
    pu8PlayPcmTmpBuf   = (uint8_t *)psAudioLib->m_pu8PlayPcmTmpBuf;
    
    i32PlayPcmTmpSmplCnt = psAudioLib->m_i32PlayPcmTmpBufLen / 3;
    
    for ( i = 0; i < psAudioLib->m_i32PlayPcmTmpBufLen; i += 3 )
    {
        pi16PlayPcmWorkBuf[psAudioLib->m_u32PlayPcmWorkBufIdx2++] = (pu8PlayPcmTmpBuf[i+2] << 8) | pu8PlayPcmTmpBuf[i+1];
        
        if ( psAudioLib->m_u32PlayPcmWorkBufIdx2 >= RING_BUF_16CNT )
            psAudioLib->m_u32PlayPcmWorkBufIdx2 = 0;
    }
    
    psAudioLib->m_i32PlayPcmWorkSmplCnt += i32PlayPcmTmpSmplCnt;
    psAudioLib->m_i32PlayPcmTmpBufLen    = i32Len;
}
#endif  // CONFIG_AUDIO_PLAY
