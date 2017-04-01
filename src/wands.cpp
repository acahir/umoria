// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Wand code

#include "headers.h"
#include "externs.h"

// Wands for the aiming.
void aim() {
    free_turn_flag = true;

    if (inven_ctr == 0) {
        msg_print("But you are not carrying anything.");
        return;
    }

    int j, k;
    if (!find_range(TV_WAND, TV_NEVER, &j, &k)) {
        msg_print("You are not carrying any wands.");
        return;
    }

    int item_val;
    if (!get_item(&item_val, "Aim which wand?", j, k, CNIL, CNIL)) {
        return;
    }

    free_turn_flag = false;

    int dir;
    if (!get_dir(CNIL, &dir)) {
        return;
    }

    if (py.flags.confused > 0) {
        msg_print("You are confused.");
        do {
            dir = randint(9);
        } while (dir == 5);
    }

    inven_type *i_ptr = &inventory[item_val];
    struct player_type::misc *m_ptr = &py.misc;

    int chance = m_ptr->save + stat_adj(A_INT) - (int)i_ptr->level + (class_level_adj[m_ptr->pclass][CLA_DEVICE] * m_ptr->lev / 3);

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
        chance = USE_DEVICE; // Give everyone a slight chance
    }

    if (chance <= 0) {
        chance = 1;
    }

    if (randint(chance) < USE_DEVICE) {
        msg_print("You failed to use the wand properly.");
        return;
    }

    if (i_ptr->p1 < 1) {
        msg_print("The wand has no charges left.");
        if (!known2_p(i_ptr)) {
            add_inscribe(i_ptr, ID_EMPTY);
        }
        return;
    }

    // Discharge the wand

    bool ident = false;

    uint32_t i = i_ptr->flags;
    (i_ptr->p1)--;

    while (i != 0) {
        j = bit_pos(&i) + 1;
        k = char_row;
        int l = char_col;

        // Wands
        switch (j) {
            case 1:
                msg_print("A line of blue shimmering light appears.");
                light_line(dir, char_row, char_col);
                ident = true;
                break;
            case 2:
                fire_bolt(GF_LIGHTNING, dir, k, l, damroll(4, 8), spell_names[8]);
                ident = true;
                break;
            case 3:
                fire_bolt(GF_FROST, dir, k, l, damroll(6, 8), spell_names[14]);
                ident = true;
                break;
            case 4:
                fire_bolt(GF_FIRE, dir, k, l, damroll(9, 8), spell_names[22]);
                ident = true;
                break;
            case 5:
                ident = wall_to_mud(dir, k, l);
                break;
            case 6:
                ident = poly_monster(dir, k, l);
                break;
            case 7:
                ident = hp_monster(dir, k, l, -damroll(4, 6));
                break;
            case 8:
                ident = speed_monster(dir, k, l, 1);
                break;
            case 9:
                ident = speed_monster(dir, k, l, -1);
                break;
            case 10:
                ident = confuse_monster(dir, k, l);
                break;
            case 11:
                ident = sleep_monster(dir, k, l);
                break;
            case 12:
                ident = drain_life(dir, k, l);
                break;
            case 13:
                ident = td_destroy2(dir, k, l);
                break;
            case 14:
                fire_bolt(GF_MAGIC_MISSILE, dir, k, l, damroll(2, 6), spell_names[0]);
                ident = true;
                break;
            case 15:
                ident = build_wall(dir, k, l);
                break;
            case 16:
                ident = clone_monster(dir, k, l);
                break;
            case 17:
                ident = teleport_monster(dir, k, l);
                break;
            case 18:
                ident = disarm_all(dir, k, l);
                break;
            case 19:
                fire_ball(GF_LIGHTNING, dir, k, l, 32, "Lightning Ball");
                ident = true;
                break;
            case 20:
                fire_ball(GF_FROST, dir, k, l, 48, "Cold Ball");
                ident = true;
                break;
            case 21:
                fire_ball(GF_FIRE, dir, k, l, 72, spell_names[28]);
                ident = true;
                break;
            case 22:
                fire_ball(GF_POISON_GAS, dir, k, l, 12, spell_names[6]);
                ident = true;
                break;
            case 23:
                fire_ball(GF_ACID, dir, k, l, 60, "Acid Ball");
                ident = true;
                break;
            case 24:
                i = (uint32_t)(1L << (randint(23) - 1));
                break;
            default:
                msg_print("Internal error in wands()");
                break;
        }
        // End of Wands.
    }

    if (ident) {
        if (!known1_p(i_ptr)) {
            m_ptr = &py.misc;
            // round half-way case up
            m_ptr->exp +=
                    (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;
            prt_experience();

            identify(&item_val);

            // NOTE: this is never read after this, so commenting out. -MRC-
            // i_ptr = &inventory[item_val];
        }
    } else if (!known1_p(i_ptr)) {
        sample(i_ptr);
    }

    desc_charges(item_val);
}
