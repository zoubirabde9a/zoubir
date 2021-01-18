/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */
#include "audio.h"

internal void
InitializeAudio(audio_state *AudioState)
{
    AudioState->MasterVolume = V2(1.f, 1.f);
}

internal playing_sound *
PlaySound(app_state *AppState, asset_id ID)
{
    assets *Assets = &AppState->Assets;
    audio_state *AudioState = &AppState->AudioState;
    playing_sound *Result = 0;
    loaded_audio *Audio = GetAudio(Assets, ID);
    if (Audio)
    {
        Assert(AudioState->PlayingSoundsCount < ArrayCount(AudioState->PlayingSounds));
        playing_sound *NewSound =
            &AudioState->PlayingSounds[AudioState->PlayingSoundsCount++];
        NewSound->Audio = Audio;
        NewSound->CurrentVolume = NewSound->TargetVolume =
            V2(1.f, 1.f);
        NewSound->DeltaVolume = V2(0.f, 0.f);
        NewSound->DeltaSample = 1.f;
        NewSound->SamplesPlayed = 0;
        Result = NewSound;
    }

    return Result;
}

internal void
ChangeVolume(playing_sound *Sound, float FadeInDurationInSeconds,
             v2 Volume)
{
    if (FadeInDurationInSeconds <= 0.f)
    {
        Sound->CurrentVolume = Sound->TargetVolume = Volume;
        Assert(Sound->DeltaVolume.X == 0.f);
        Assert(Sound->DeltaVolume.Y == 0.f);
    }
    else
    {
        Sound->TargetVolume = Volume;
        float OneOverFade = 1.f / FadeInDurationInSeconds;
        Sound->DeltaVolume =
            (Sound->TargetVolume - Sound->CurrentVolume) *
            OneOverFade;
    }
}

internal void
ChangePitch(playing_sound *Sound, float DeltaSample)
{
    Sound->DeltaSample = DeltaSample;
}

internal void
OutputAudio(audio_state *AudioState,
            app_sound_output_buffer *SoundBuffer, memory_arena *Arena)
{
    u32 SampleCount = Align4(SoundBuffer->SampleCount);
    u32 SampleCount4 = SampleCount / 4;
    
    __m128 *Channel0 = AllocateArray(Arena, SampleCount4, __m128, 16);
    __m128 *Channel1 = AllocateArray(Arena, SampleCount4, __m128, 16);
    float SecondsPerSample =
        1.f / (float)SoundBuffer->SamplesPerSecond;
#define OutputChannelCount 2

//    ZeroArray(Channel0, SampleCount, float);
//    ZeroArray(Channel1, SampleCount, float);

    __m128 Zero4Wide = _mm_set1_ps(0.f);

    __m128 MaxS16 = _mm_set1_ps(32767.f);
    __m128 MinS16 = _mm_set1_ps(-32768.f);
    {
        __m128 *Dest0 = Channel0;
        __m128 *Dest1 = Channel1;
        for(u32 SampleIndex = 0;
            SampleIndex < SampleCount4;
            SampleIndex++)
        {
            _mm_store_ps((float *)Dest0++, Zero4Wide);
            _mm_store_ps((float *)Dest1++, Zero4Wide);
///            *Dest0++ = Zero4Wide;
//            *Dest1++ = Zero4Wide;
        }
    }
    
    for(u32 PlayingIndex = 0;
        PlayingIndex < AudioState->PlayingSoundsCount;
        )
    {
        bool32 SoundFinished = false;
        playing_sound *ThisSound = &AudioState->PlayingSounds[PlayingIndex];
        loaded_audio *LoadedAudio = ThisSound->Audio;
        u32 Channels = LoadedAudio->Channels;
        
//        i16 *Src = Data + (i32)ThisSound->SamplesPlayed * Channels;
        u32 ThisSoundSampleCount = LoadedAudio->SampleCount;

        v2 Volume = ThisSound->CurrentVolume;
        v2 DeltaVolume =
            ThisSound->DeltaVolume * SecondsPerSample;
        float DeltaSample = ThisSound->DeltaSample;

        float *Dest0 = (float *)Channel0;
        float *Dest1 = (float *)Channel1;

        Assert(ThisSound->SamplesPlayed >= 0.f);

        u32 SamplesToMix = SampleCount;
        float FloatSampleRemainingInSound =
            (ThisSoundSampleCount -
             RoundFloatToI32(ThisSound->SamplesPlayed)) /
            DeltaSample;
        u32 SamplesRemainingInSound =
            RoundFloatToU32(FloatSampleRemainingInSound);
        if (SamplesToMix >= SamplesRemainingInSound)
        {
            SamplesToMix = SamplesRemainingInSound;
            SoundFinished = true;
        }

        bool32 VolumeEnded[OutputChannelCount] = {};
        for(u32 ChannelIndex = 0;
            ChannelIndex < ArrayCount(VolumeEnded);
            ChannelIndex++)
        {
            if (DeltaVolume.Data[ChannelIndex] != 0.f)
            {
                float Diff =
                    ThisSound->TargetVolume.Data[ChannelIndex] -
                    Volume.Data[ChannelIndex];
                u32 VolumeSampleCount =
                    (u32)((Diff / DeltaVolume.Data[ChannelIndex]) + 0.5f);
                if (SamplesToMix > VolumeSampleCount)
                {
                    SamplesToMix = VolumeSampleCount;
                    VolumeEnded[ChannelIndex] = true;
                }
            }                                   
        }

        //TODO(zoubir): handle stereo
        float SamplePosition = ThisSound->SamplesPlayed;
        for(u32 LoopIndex = 0;
            LoopIndex < SamplesToMix;
            LoopIndex++)
        {            
            u32 SampleIndex = FloorFloatToU32(SamplePosition);
            float Frac = SamplePosition - (float)SampleIndex;
            float Sample0 = (float)LoadedAudio->Samples[0][SampleIndex];
            float Sample1 = (float)LoadedAudio->Samples[0][SampleIndex + 1];
            float SampleValue =
                Lerp(Sample0, Frac, Sample1);

            *Dest0++ += AudioState->MasterVolume.Data[0] *
                Volume.Data[0] * SampleValue;
            *Dest1++ += AudioState->MasterVolume.Data[1] *
                Volume.Data[1] * SampleValue;
            
            Volume += DeltaVolume;
            SamplePosition += DeltaSample;
        }
        ThisSound->CurrentVolume = Volume;
        // TODO(zoubir): not correct yet, need to truncate
        // the loop
        
        for(u32 ChannelIndex = 0;
            ChannelIndex < ArrayCount(VolumeEnded);
            ChannelIndex++)
        {
            if (VolumeEnded[ChannelIndex])
            {
                ThisSound->CurrentVolume.Data[ChannelIndex] =
                    ThisSound->TargetVolume.Data[ChannelIndex];
                ThisSound->DeltaVolume.Data[ChannelIndex] = 0.f;
            }
        }
        
        ThisSound->SamplesPlayed = SamplePosition;
        
        if (SoundFinished)
        {
            AudioState->PlayingSounds[PlayingIndex] =
                AudioState->PlayingSounds[(AudioState->PlayingSoundsCount--) - 1];
        }
        else
        {
            PlayingIndex++;
        }
    }

    __m128 *Source0 = Channel0;
    __m128 *Source1 = Channel1;

    __m128i *SampleOut = (__m128i *)SoundBuffer->Samples;
    for(u32 SampleIndex = 0;
        SampleIndex < SampleCount4;
        SampleIndex++)
    {
//    __m128 MaxS16 = _mm_set1_ps(32767.f);
//    __m128 MinS16 = _mm_set1_ps(-32768.f);

        __m128i L = _mm_cvtps_epi32(*Source0++);
        __m128i R = _mm_cvtps_epi32(*Source1++);

        __m128i LR0 = _mm_unpacklo_epi32(L, R);
        __m128i LR1 = _mm_unpackhi_epi32(L, R);
                
        __m128i S01 = _mm_packs_epi32(LR0, LR1);
        
        *SampleOut++ = S01;
    }

}


