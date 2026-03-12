#pragma once

void standby_tick();   // Call every loop iteration when in standby
void standby_reset();  // Reset animation state (called on wake)

void confuse_anim_reset();  // Reset confuse animation state (difficulty 8)
void confuse_anim_tick();   // Advance confuse animation one tick (difficulty 8)
