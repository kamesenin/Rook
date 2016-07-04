#ifndef AL_EFX_DEDICATED_H
#define AL_EFX_DEDICATED_H

/*
 * Extension name. 
 */
#define ALC_EXT_DEDICATED_NAME "ALC_EXT_DEDICATED"

/*
 * Audio rendered to this effect is routed to a subwoofer if one is 
 * present. Otherwise, it is discarded.
 */
#define AL_EFFECT_DEDICATED_LOW_FREQUENCY_EFFECT 0x9000

/*
 * Audio rendered to this effect is routed to a front centre 
 * speaker if one is present. Otherwise, it is rendered to the 
 * front centre using the normal spatialisation logic.
 */
#define AL_EFFECT_DEDICATED_DIALOGUE             0x9001

/*
 * Both dedicated effects support a gain control which will 
 * take any floating point value.
 */
#define AL_DEDICATED_GAIN                        0x0001
#define AL_DEDICATED_DEFAULT_GAIN                1.0f

#endif /* AL_EFX_DEDICATED_H */
