/* CKIT.EXE overlay 14 module 2ccd: a party member's class-derived values.
 * The spell slots each class grants at its level and the spells those
 * slots make known; the to-hit base, the highest level, the value at +ab
 * and the class bits; the saving throws; the thief skills, from a table by
 * thief level adjusted by race and dexterity; and whether a member is human.
 *
 * EXE 0x061f1c..0x062b45, one Turbo C large-model _TEXT segment starting at
 * byte c of paragraph 2ccd (load-based 3ccd), the last module of RTLink
 * section 14 (payload 0x5dce0, link segment 28aa, parent section 13).  The
 * first c bytes of paragraph 2ccd are the tail of charview.c (module 28aa);
 * the lone retf at 2ccd:000c that follows, a one-byte routine nothing
 * reaches, is placed here by the run's rule.  Eleven zero bytes follow to
 * the section's end at 0x062b50.
 * Section 14's table files all 11 fixup sites in the range under frame 2ccd.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.  Section 15 holds other
 * bytes at link segment 28aa.
 *
 * A bare stored seg:off in this file means section 14.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address.  Callee and variable names are placeholders chosen
 * from use.  Section 14 has no child section, so code outside the section
 * reaches this module only through vector stubs: seven of the section's 31
 * vectors land here, 18de:19c7 -> 000d, 1a21 -> 0155, 196d -> 019a,
 * 1729 -> 05c5, 19ef -> 0898, 173d -> 0a0f, 1a17 -> 0c1f.
 *
 * DQK's herostat.c (dqk 893e699, overlay 11 module 304e) reworked: its
 * first seven routines are this module's first seven, in the same order,
 * on FRUA's Hero record (the slots one 3 x 9 array at +163, the abilities
 * at +70, race a byte).  cleric_slots has no +19b test (its row index t
 * is always 0) and does not clear or seed the slots itself; spell_slots
 * has no case for class 1 (FRUA's Knight), its case 0 leaves the slots to
 * cleric_slots, and its case 5 reads one fixed row (mu_spell_row) with no
 * 0xff test and no slots[2][0] seed; class_update has no word_19b/race
 * increment of to_hit_base (DQK's byte_87 at +87); thief_skills tests
 * the race penalty with && where DQK computes & (with & this module
 * bakes 4 of 8); saving_throws finds the protective item by bit 7 of
 * +38 (DQK: > 0x80; that bakes 7 of 8) and
 * adds the CON bonus for it alone, with no race test.  DQK's experience
 * routine (304e:0e9c) has no counterpart here; is_human is FRUA's own.
 * thief_skills uses <stdlib.h>'s inline abs(), so the file is CRLF.
 *
 * The module has no _DATA and no _BSS.  Every table it reads is declared
 * extern: class_spell_rows, save_table and the class tables from 61b1 lie apart from
 * each other and from charview.c's _DATA.
 */

#include <stdlib.h>

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28  index into item_table             */
    char pad_29[9];             /* +29  not read here                     */
    char readied;               /* +32                                    */
    char pad_33;                /* +33  not read here                     */
    char cursed;                /* +34                                    */
    char pad_35[3];             /* +35  not read here                     */
    unsigned char special;      /* +38  bit 7: an effect when readied     */
};

struct ItemType {               /* 0x10 bytes an item type                */
    char pad_0[0xd];            /* +0   not read here                     */
    unsigned char classes;      /* +d   tested against Hero.classes       */
    char pad_e[2];              /* +e   not read here                     */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0   0, 1, 2: slots[0], [1], [2]       */
    unsigned char level;        /* +2   spell level, from 1               */
    char pad_3[0x13];           /* +3   not read here                     */
};

struct Stat {
    unsigned char base;         /* +0                                     */
    unsigned char cur;          /* +1                                     */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    struct Item far *equipped[13]; /* +c                                     */
    char pad_40[0x18];          /* +40  not read here                     */
    unsigned char race;         /* +58  5 human                           */
    char pad_59[0x17];          /* +59  not read here                     */
    struct Stat stats[6];       /* +70  str int wis dex con cha           */
    char pad_7c[3];             /* +7c  not read here                     */
    unsigned char to_hit_base;  /* +7f  best of attack_scores_by_level over classes     */
    char pad_80[3];             /* +80  not read here                     */
    unsigned char saves[5];     /* +83  lowest of save_table, from 20       */
    char pad_88;                /* +88  not read here                     */
    unsigned char hit_dice;     /* +89  highest class_level level           */
    char pad_8a;                /* +8a  not read here                     */
    unsigned char thief[8];     /* +8b                                    */
    char pad_93[0xa];           /* +93  not read here                     */
    unsigned char level[7];     /* +9d  one per class                     */
    unsigned char old_level[7]; /* +a4  one per class                     */
    unsigned char attack_rate;  /* +ab                                    */
    char pad_ac[0xb];           /* +ac  not read here                     */
    unsigned char class_mask;   /* +b7  sum of class_mask_bits bits              */
    char pad_b8[0x9b];          /* +b8  not read here                     */
    unsigned char known[0x10];  /* +153 one bit per spell, from 1         */
    unsigned char slots[3][9];  /* +163 per spell level, by kind          */
};

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern char far old_class_abilities_available(struct Hero far *h);                   /* 03d0:2100 */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern void far item_equip_toggle(struct Item far *item);            /* 28aa:19bf */

extern unsigned char class_spell_rows[];        /* 139d  a spell_slots_by_level row by class   */
#define mu_spell_row (class_spell_rows[5])   /* the magic-user's row         */
extern unsigned char spell_slots_by_level[][29][9];                          /* 14f4 */
extern unsigned char save_table[][22][5];                          /* 1916 */
extern struct SpellInfo spell_info_table[];                      /* 2660 */
#define spell_info (spell_info_table - 1)     /* spell numbers start at 1 */
extern unsigned char attack_scores_by_level[][22];                             /* 61b1 */
extern unsigned char class_mask_bits[];                                 /* 624b */
extern unsigned char thief_skill_base[][8];                              /* 627d */
extern char thief_race_adjust[][8];                                       /* 630d */
extern char thief_dex_adjust[][5];                                       /* 633d */
extern unsigned char bit_mask[8];                                /* 6711 */
extern struct Hero far *cur_node;                                /* b3dd */
extern struct ItemType far *item_table;                          /* b40a */

void far thief_skills(struct Hero far *h);
void far saving_throws(struct Hero far *h);

/* The translated Pascal unit's empty initialization block (DKK classcal.pas). */
void far herostat_unit_init(void)                                     /* 3ccd:000c */
{
}

void far cleric_slots(struct Hero far *h)                        /* 3ccd:000d */
{
    unsigned char i;
    unsigned char lvl;
    short t;

    lvl = class_level(h, 0);
    if (lvl > 0) {
        t = 0;
        if (lvl > 29)
            lvl = 29;
        for (i = 1; i <= 7; i++)
            if (class_spell_rows[t] == 0xff)
                h->slots[0][i - 1] = 0;
            else
                h->slots[0][i - 1] = spell_slots_by_level[class_spell_rows[t]][lvl - 1][i - 1];
        if (h->stats[2].cur > 12 && h->slots[0][0] > 0)
            h->slots[0][0]++;
        if (h->stats[2].cur > 13 && h->slots[0][0] > 0)
            h->slots[0][0]++;
        if (h->stats[2].cur > 14 && h->slots[0][1] > 0)
            h->slots[0][1]++;
        if (h->stats[2].cur > 15 && h->slots[0][1] > 0)
            h->slots[0][1]++;
        if (h->stats[2].cur > 16 && h->slots[0][2] > 0)
            h->slots[0][2]++;
        if (h->stats[2].cur > 17 && h->slots[0][3] > 0)
            h->slots[0][3]++;
        if (h->stats[2].cur < 17)
            h->slots[0][5] = 0;
        if (h->stats[2].cur < 18)
            h->slots[0][6] = 0;
    }
}

void far mu_slot_cap(struct Hero far *h)                         /* 3ccd:0155 */
{
    if (h->stats[1].cur < 12)
        h->slots[2][5] = 0;
    if (h->stats[1].cur < 14)
        h->slots[2][6] = 0;
    if (h->stats[1].cur < 16)
        h->slots[2][7] = 0;
    if (h->stats[1].cur < 18)
        h->slots[2][8] = 0;
}

void far spell_slots(struct Hero far *h)                         /* 3ccd:019a */
{
    short j;
    unsigned char lvl;
    unsigned char m;
    unsigned char i;
    unsigned char k;
    struct Item far *p;

    mem_fill(h->slots, 0x1b, 0);        /* +163..+17d */
    for (j = 0; j <= 7; j++) {
        lvl = class_level(h, j);
        if (lvl > 29)
            lvl = 29;
        if (lvl > 0)
            switch (j) {
            case 0:
                cleric_slots(h);
                for (k = 1; k <= 0x7e; k++)
                    if (spell_info[k].kind == 0
                        && h->slots[0][spell_info[k].level - 1] > 0)
                        h->known[(k - 1) / 8] =
                            h->known[(k - 1) / 8] | bit_mask[(k - 1) % 8];
                break;
            case 3:
                if (lvl > 8) {
                    for (i = 1; i <= 4; i++)
                        h->slots[0][i - 1] += spell_slots_by_level[class_spell_rows[3]][lvl - 1][i - 1];
                    for (k = 1; k <= 0x7e; k++)
                        if (spell_info[k].kind == 0
                            && h->slots[0][spell_info[k].level - 1] > 0)
                            h->known[(k - 1) / 8] |= bit_mask[(k - 1) % 8];
                }
                break;
            case 4:
                if (lvl > 7) {
                    for (i = 1; i <= 4; i++)
                        h->slots[1][i - 1] += spell_slots_by_level[class_spell_rows[4]][lvl - 1][i - 1];
                    for (i = 5; i <= 8; i++) {
                        m = i - 4;
                        h->slots[2][m - 1] += spell_slots_by_level[class_spell_rows[4]][lvl - 1][i - 1];
                    }
                }
                for (k = 1; k <= 0x7e; k++)
                    if (spell_info[k].kind == 1
                        && h->slots[1][spell_info[k].level - 1] > 0
                        || spell_info[k].kind == 2
                        && h->slots[2][spell_info[k].level - 1] > 0) {
                        lvl = spell_info[k].level;
                        if (h->slots[1][lvl - 1] > 0)
                            h->known[(k - 1) / 8] =
                                h->known[(k - 1) / 8] | bit_mask[(k - 1) % 8];
                    }
                break;
            case 5:
                for (i = 1; i <= 9; i++) {
                    h->slots[2][i - 1] += spell_slots_by_level[mu_spell_row][lvl - 1][i - 1];
                    mu_slot_cap(h);
                }
            }
    }
    for (p = h->items; p != 0; p = p->next)
        if (p->special == 0x81 && p->readied != 0) {
            h->slots[2][4] += h->slots[2][4];
            break;
        }
}

void far class_update(struct Hero far *h)                        /* 3ccd:05c5 */
{
    struct Hero far *saved;
    short j;
    unsigned char c;
    unsigned char lvl;
    short l;

    h->to_hit_base = 0;
    for (j = 0; j <= 6; j++) {
        lvl = class_level(h, j);
        if (lvl > 21)
            c = 21;
        else
            c = lvl;
        if (attack_scores_by_level[j][c] > h->to_hit_base)
            h->to_hit_base = attack_scores_by_level[j][c];
        if (h->hit_dice < lvl)
            h->hit_dice = lvl;
        switch (j) {
        case 1:
        case 2:
        case 3:
            if (lvl > 6)
                h->attack_rate = 3;
            if (lvl > 12)
                h->attack_rate = 4;
            break;
        case 4:
            if (lvl > 7)
                h->attack_rate = 3;
            if (lvl > 14)
                h->attack_rate = 4;
            break;
        }
    }
    spell_slots(h);
    saving_throws(h);
    if (h->level[6] > 0)
        thief_skills(h);
    h->class_mask = 0;
    for (j = 0; j <= 6; j++)
        if (h->level[j] > 0 || h->old_level[j] > 0 && h->old_level[j] < h->hit_dice)
            h->class_mask += class_mask_bits[j];
    for (l = 0; l <= 12; l++)
        if (h->equipped[l] != 0
            && (item_table[h->equipped[l]->type].classes & h->class_mask) == 0
            && h->equipped[l]->cursed == 0 && h->equipped[l]->readied != 0) {
            saved = cur_node;
            cur_node = h;
            item_equip_toggle(h->equipped[l]);
            cur_node = saved;
        }
    if (old_class_abilities_available(h))
        for (j = 0; j <= 6; j++) {
            lvl = h->old_level[j];
            switch (j) {
            case 2:
            case 3:
                if (lvl >= 13)
                    h->attack_rate = 4;
                else if (lvl >= 7)
                    h->attack_rate = 3;
                break;
            case 4:
                if (lvl >= 15)
                    h->attack_rate = 4;
                else if (lvl >= 8)
                    h->attack_rate = 3;
                break;
            case 6:
                if (lvl > 0)
                    thief_skills(h);
            }
        }
}

void far thief_skills(struct Hero far *h)                        /* 3ccd:0898 */
{
    unsigned char i;
    unsigned char a;
    unsigned char lvl;
    unsigned char save;

    a = 0;
    lvl = class_level(h, 6);
    if (lvl > 18)
        lvl = 18;
    save = lvl;
    for (i = 1; i <= 8; i++) {
        if (thief_race_adjust[h->race][i - 1] < 0
            && abs(thief_race_adjust[h->race][i - 1]) + a > thief_skill_base[lvl - 1][i - 1])
            h->thief[i - 1] = 0;
        else {
            h->thief[i - 1] = thief_skill_base[lvl - 1][i - 1] + thief_race_adjust[h->race][i - 1] + a;
            if (i < 6)
                h->thief[i - 1] = thief_dex_adjust[h->stats[3].cur - 9][i - 1] + h->thief[i - 1];
        }
        lvl = save;
    }
}

void far saving_throws(struct Hero far *h)                       /* 3ccd:0a0f */
{
    short s;
    short j;
    unsigned char t;
    struct Item far *p;
    char flag;
    unsigned char c;
    unsigned char lvl;

    flag = 0;
    for (p = h->items; p != 0 && flag == 0; p = p->next)
        if ((p->special & 0x80) && p->readied != 0) {
            t = p->special & 0x7f;
            flag = t == 6;
        }
    for (s = 0; s <= 4; s++) {
        h->saves[s] = 20;
        for (j = 0; j <= 6; j++) {
            lvl = class_level(h, j);
            if (lvl > 0) {
                if (lvl > 21)
                    c = 21;
                else
                    c = lvl;
                if (save_table[j][c][s] < h->saves[s])
                    h->saves[s] = save_table[j][c][s];
                if (s == 0 && flag != 0)
                    switch (h->stats[4].cur) {
                    case 4:
                    case 5:
                    case 6:
                        h->saves[s]++;
                        break;
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                        h->saves[s] += 2;
                        break;
                    case 11:
                    case 12:
                    case 13:
                        h->saves[s] += 3;
                        break;
                    case 14:
                    case 15:
                    case 16:
                    case 17:
                        h->saves[s] += 4;
                        break;
                    case 18:
                        h->saves[s] += 5;
                    }
                if (s == 0)
                    switch (h->stats[4].cur) {
                    case 19:
                    case 20:
                        h->saves[s]++;
                        break;
                    case 21:
                    case 22:
                        h->saves[s] += 2;
                        break;
                    case 23:
                    case 24:
                        h->saves[s] += 3;
                        break;
                    case 25:
                        h->saves[s] += 4;
                    }
            }
        }
    }
}

char far is_human(struct Hero far *h)                            /* 3ccd:0c1f */
{
    return h->race == 5;
}
