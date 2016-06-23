#pragma once

/**
 * Known Nao models for which code is distinguishable.
 */
enum NaoVersion
{
        nao_unset = 0,

        nao_v3 = 3,
        nao_v4 = 4,

        nao_latest = nao_v4,
};

/**
 * The version of the Nao. Is initially set to nao_unset and determined
 * by calling determineNaoVersion().
 */
extern NaoVersion naoVersion;

/**
 * Determine the version of the nao and set naoVersion accordingly.
 * @return      Version of this Nao.
 */
NaoVersion determineNaoVersion(void);

