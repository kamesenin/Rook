#ifndef AL_AL_MC_MULAW_H
#define AL_AL_MC_MULAW_H

/*****************************************************************************/

/*
 * This extension indicates that a number of multichannel MULAW
 * formats are available beyond the basic mono/stereo set provided by
 * AL_EXT_MULAW. The list is AL_FORMAT_MONO_MULAW,
 * AL_FORMAT_STEREO_MULAW, AL_FORMAT_QUAD_MULAW, AL_FORMAT_REAR_MULAW,
 * AL_FORMAT_51CHN_MULAW, AL_FORMAT_61CHN_MULAW and
 * AL_FORMAT_71CHN_MULAW. Note that the first two have the same enum
 * values as the formats in AL_EXT_MULAW and describe the same formats.
 *
 * MULAW encodes a sample value into each byte using a non-linear
 * encoding.
 */
#define AL_EXT_MULAW_MCFORMATS "AL_EXT_MULAW_MCFORMATS"

/* 
 * Note that this enum is also declared in AL_EXT_MULAW, although it
 * is known there as AL_FORMAT_MONO_MULAW_EXT.
 */
#define AL_FORMAT_MONO_MULAW            0x10014
/* 
 * Note that this enum is also declared in AL_EXT_MULAW, although it
 * is known there as AL_FORMAT_STEREO_MULAW_FORMAT.
 */
#define AL_FORMAT_STEREO_MULAW          0x10015

/*
 * Quad. Order: front left, front right, back left, back right.
 */
#define AL_FORMAT_QUAD_MULAW            0x10021
/*
 * Rear stereo. Order: back left, back right.
 */
#define AL_FORMAT_REAR_MULAW            0x10022
/*
 * Surround 5.1. Order: front left, front right, front centre, LFE,
 * side/back left, side/back right.
 */
#define AL_FORMAT_51CHN_MULAW           0x10023
/*
 * Surround 6.1. Order: front left, front right, front centre, LFE,
 * side left, side right, back centre.
 */
#define AL_FORMAT_61CHN_MULAW           0x10024
/*
 * Surround 7.1. Order: front left, front right, front centre, LFE,
 * back left, back right, side left, side right.
 */
#define AL_FORMAT_71CHN_MULAW           0x10025

/*****************************************************************************/

/*
 * This extension implies two MULAW formats are available, based on 2D
 * and 3D Ambisonic B-Format. See AL_EXT_BFORMAT for a discussion of
 * the channel numberings and meanings.
 */
#define AL_EXT_MULAW_BFORMAT "AL_EXT_MULAW_BFORMAT"

#define AL_FORMAT_BFORMAT2D_MULAW       0x10031
#define AL_FORMAT_BFORMAT3D_MULAW       0x10032

/*****************************************************************************/

#endif /* AL_AL_MC_MULAW_H */
