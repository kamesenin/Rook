#ifndef AL_ALC_DEFAULT_FILTER_ORDER_H
#define AL_ALC_DEFAULT_FILTER_ORDER_H

/*
 * Extension name. 
 */
#define ALC_EXT_DEFAULT_FILTER_ORDER "ALC_EXT_DEFAULT_FILTER_ORDER"

/*
 * This attribute can be used to suggest a particular order
 * of filter during context creation. 1 indicates -6dB/oct filters, 
 * 2 indicates -12dB/oct. 
 * 
 * It can also be used with alcGetIntegerv() to find out what 
 * order filters are in use.
 */
#define ALC_DEFAULT_FILTER_ORDER 0x1100


#endif /* AL_ALC_DEFAULT_FILTER_ORDER_H */
