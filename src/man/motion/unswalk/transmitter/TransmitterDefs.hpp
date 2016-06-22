#pragma once

/**
 * the type of what this is actually receiving.
 * DO NOT CHANGE THIS WITHOUT FINISHING ALL THE TODOS IN OFFNAO TRANSMITTER
 * historically used only by the off-nao transmitter.  TODO(jayen) refactor to
 * have separate types for command masks and data masks.
 */
typedef uint64_t OffNaoMask_t;

enum {
   BLACKBOARD_MASK      = 0x0000000000000001ull,
   SALIENCY_MASK        = 0x0000000000000002ull,
   RAW_IMAGE_MASK       = 0x0000000000000004ull,
   PARTICLE_FILTER_MASK = 0x0000000000000008ull,
   ROBOT_FILTER_MASK    = 0x0000000000000010ull,
   INITIAL_MASK         = 0x0000000000000003ull,
   ALL_MASKS            = 0x000000000000003Full,
   LANDMARKS_MASK       = 0x0000000000000020ull,

   COMMAND_MASK         = 0x8000000000000000ull,
   TO_NAO_MASKS         = 0x8000000000000000ull
};
