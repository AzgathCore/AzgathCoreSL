/*
 * Copyright 2023 AzgathCore
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITY_SPELLDEFINES_H
#define TRINITY_SPELLDEFINES_H

#include "Define.h"

namespace UF
{
    struct SpellCastVisual;
}

namespace WorldPackets
{
    namespace Spells
    {
        struct SpellCastVisual;
    }
}

struct SpellCastVisual
{
    uint32 SpellXSpellVisualID = 0;
    uint32 ScriptVisualID = 0;

    operator UF::SpellCastVisual() const;
    operator WorldPackets::Spells::SpellCastVisual() const;
};

#endif
