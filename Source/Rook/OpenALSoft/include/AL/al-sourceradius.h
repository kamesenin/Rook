#ifndef AL_AL_SOURCE_RADIUS_H
#define AL_AL_SOURCE_RADIUS_H

/*****************************************************************************/

/*
 * This extension allows any mono source to be changed to be a "large" 
 * source with a radius.
 *
 * The source has a raised cosine shape.
 */
#define AL_EXT_SOURCE_RADIUS "AL_EXT_SOURCE_RADIUS"

/*
 * This tag can be used with alSourcef() to set the source radius. Units
 * are consistent with the coordinate system in use. The value must be 
 * at least zero. Use a value of zero to reset to a point source.
 */
#define AL_SOURCE_RADIUS 0x1031

/*****************************************************************************/

#endif /* AL_AL_SOURCE_RADIUS_H */
