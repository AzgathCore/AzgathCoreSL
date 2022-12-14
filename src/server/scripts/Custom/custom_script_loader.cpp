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

// This is where scripts' loading functions should be declared:
void AddSC_custom_npcs();
void AddSC_custom_player_script();
void AddSC_cs_world_chat();
void AddSC_solocraft();
void AddSC_azgath_legit_quest_bypass();
void AddSC_XpWeekend();
void AddSC_heirloom_mount_tempfix();
void AddSC_azgath_gift_mount();

// ADM declaration begin
// ADM declaration end

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddCustomScripts()
{
    AddSC_custom_npcs();
    AddSC_custom_player_script();
    AddSC_cs_world_chat();
	AddSC_solocraft();
	AddSC_azgath_legit_quest_bypass();
	AddSC_XpWeekend();
	AddSC_heirloom_mount_tempfix();
	AddSC_azgath_gift_mount();

    // ADM call begin
    // ADM call end
}
