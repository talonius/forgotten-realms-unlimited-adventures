/* CKIT.EXE overlay 19 module 2d91: the game clock and resting in camp.
 * Advancing the clock with its carries, effects running out as time passes,
 * the rest-time menu and its display, and the rest loop: healing,
 * memorizing and scribing spells while the party rests, and the roll for
 * an encounter that interrupts it.
 *
 * EXE 0x06eff0..0x06ff6e, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 2d91, the first module of RTLink section 19 (payload
 * 0x6eff0, link segment 2d91).  Its last byte is byte d of paragraph 2e88;
 * the lone retf at 0x06ff6e (2e88:000e) is left to module 2e88, and nothing
 * found reaches it.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions
 * are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 19.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and ovl19_2e88_NNNN names a routine of
 * section 19's module 2e88.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.
 *
 * This is the v1.0 build (CKIT.EXE 1.0, file 0x6ef60..0x6fec8); the
 * addresses above are v1.2's.  v1.0's encounter_roll takes its roll count
 * as encounter_ticks minus the area's interval (v1.2 divides) and never
 * takes the interval back off encounter_ticks inside the loop.
 *
 * _DATA is DGROUP 8f30..8fa3: seven stored literals; -d stores "%d" as the
 * tail of "0%d" and ":" as the tail of "Rest Time:".  _BSS is fx_live at
 * d450..d494.
 *
 * A time is seven counters, smallest first.  time_add carries each of the
 * first six into the next when it reaches its time_units entry; the last
 * wraps and ages every party member.  The clock keeps its counters as bytes
 * at cursor_obj + 5, the rest time as words in rest_time.
 */

struct Effect {                 /* an effect on a party member            */
    unsigned char kind;         /* +0                                     */
    char pad_1[1];              /* +1  not read here                      */
    unsigned duration;          /* +2  0: does not run out                */
    char pad_4[2];              /* +4  not read here                      */
    struct Effect far *next;    /* +6                                     */
};

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4  not read here                      */
    unsigned char type;         /* +28 'I': spells are in more's chain    */
    char pad_29[0xc];           /* +29 not read here                      */
    unsigned char slots[4];     /* +35 [0] 'I' chain length; [1..3] spell */
    char pad_39[1];             /* +39 not read here                      */
    struct Item far *more;      /* +3a                                    */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    struct Effect far *effects; /* +4                                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x46];           /* +c   not read here                     */
    unsigned age;               /* +52  years                             */
    char pad_54[0x2a];          /* +54  not read here                     */
    char study;                 /* +7e  study_hour counts it down to 0    */
    char pad_7f[0x47];          /* +7f  not read here                     */
    unsigned char spells[0x8d]; /* +c6  bit 7: still to memorize          */
    unsigned char known[0x2b];  /* +153 one bit per spell, from 1         */
    char active;                /* +17e 0: memorize_one skips the member  */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    char pad_0[2];              /* +0  not read here                      */
    unsigned char level;        /* +2                                     */
    char pad_3[0x13];           /* +3  not read here                      */
};

struct Event {                  /* 0x14 bytes, not read here              */
    char pad_0[0x14];
};

struct Encounter {              /* one row an area                        */
    unsigned ticks;             /* +0  rest minutes between rolls         */
    unsigned char event;        /* +2  from 1, a event_table row             */
    unsigned char chance;       /* +3  percent; 0x80 up: tried every call */
};

struct Area {                   /* what cur_area_rec points at                */
    char pad_0[0x2e];           /* +0  not read here                      */
    struct Encounter rest_enc_wide[0x3c]; /* +2e the count is not bounded here      */
};

struct Geo {
    char x;                     /* +0  d414                               */
    char y;                     /* +1  d415                               */
};

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern unsigned char far game_key_get(void);                        /* 0d48:0052 */
extern char far screen_sync(void);                               /* 0a75:0362 */
extern unsigned char far hi_byte(int w);                         /* 0a75:036d */
extern unsigned char far lo_byte(int w);                         /* 0a75:037b */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far msg_pause(void);                                 /* 06c9:0953 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim); /* 083a:1fb3 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                                struct Effect far *fx);          /* 2e88:00d8 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides); /* 2e88:15ee */
extern char far fx_heal(struct Hero far *hero, char a, char b); /* 2e88:224c */
extern void far party_display(void far *node);        /* 18de:1067, vector to overlay 4 1c86:023a */
extern unsigned char far cell_zone_at(char x, char y); /* 18de:1355, vector to overlay 13 26e3:0b87 */
extern void far events_run(char kind);             /* 18de:152b, vector to overlay 4 1cff:29dd */
extern char far event_test(char kind, char n);     /* 18de:1535, vector to overlay 4 1cff:24c1 */
extern char far item_casts(struct Item far *item); /* 18de:1acb, vector to overlay 12 26e3:4055 */
extern void far item_spend(struct Hero far *hero, struct Item far *item,
                             char spell);            /* 18de:1cbf, vector to overlay 12 26e3:408c */
extern void far cast_announce(struct Hero far *hero, char far *fmt,
                             char spell);            /* 18de:1cc9, vector to overlay 12 26e3:4235 */

extern struct SpellInfo spell_info[];                            /* 264a */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *rest_items;                                     /* 3c50 */
extern unsigned time_units[7];                                   /* 61a3 */
extern unsigned char bit_mask[8];                                /* 6711 */
extern char in_service_event;                                           /* 671c */
extern struct Event far *cur_event;                               /* a7e4 */
extern struct Event far *saved_event;                               /* a7e8 */
extern struct Event far *event_table;                               /* a7f0 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern int view_mode;                                            /* b421 */
extern int saved_view_mode;                                            /* b433 */
extern char resting;                                             /* b4eb */
extern unsigned rest_time[7];                                    /* b729 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char no_study;                                            /* be68 */
extern unsigned heal_ticks;                                      /* c34e */
extern char year_rolled;                                         /* c354 */
extern unsigned encounter_ticks;                                 /* ca1b */
extern char menu_escaped;                                        /* ca2a */
extern unsigned char study_wait[];                               /* cfde */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern struct Geo geo;                                           /* d414 */
extern struct Area far *cur_area_rec;                                /* d41c */

static char fx_live[0x45];                                       /* d450 */

void far clock_set(char a, char b, char c, char d, char e, char f, short year) /* 3d91:0000 */
{
    cursor_obj[5] = a;
    cursor_obj[6] = b;
    cursor_obj[7] = c;
    cursor_obj[8] = d;
    cursor_obj[9] = e;
    cursor_obj[10] = f;
    cursor_obj[11] = lo_byte(year);
    cursor_obj[12] = hi_byte(year);
}

void far effects_tick(unsigned char n, short unit)               /* 3d91:0055 */
{
    short k;
    unsigned char i;
    unsigned char step;
    unsigned short total;
    struct Hero far *hero;
    struct Effect far *next;
    struct Effect far *fx;

    if (view_mode != 2)
        mem_fill(fx_live, sizeof fx_live, 1);
    else {
        i = 1;
        do
            i++;
        while (fx_live[i] == 0 && cursor_obj[0x20] >= i);
        if (fx_live[i] == 0)
            return;
    }
    total = n;
    for (k = unit; k > 1; k = k - 1)
        total *= time_units[k - 1];
    while (total > 0) {
        if (total > 10)
            step = 10;
        else
            step = total;
        for (i = 1, hero = node_list; hero != 0; i++, hero = hero->next) {
            if (fx_live[i] != 0) {
                fx_live[i] = 0;
                fx = hero->effects;
                while (fx != 0) {
                    if (fx->duration == 0)
                        fx = fx->next;
                    else if (fx->duration > step) {
                        fx->duration -= step;
                        fx_live[i] = 1;
                        fx = fx->next;
                    } else {
                        next = fx->next;
                        fx_remove(hero, fx->kind, fx);
                        fx_live[i] = 1;
                        fx = next;
                    }
                }
            }
        }
        if (total > 10)
            total -= 10;
        else
            total = 0;
    }
}

void far time_add(unsigned far *t)                               /* 3d91:01e7 */
{
    short i;
    struct Hero far *hero;

    for (i = 0; i <= 6; i++) {
        if (t[i] >= time_units[i]) {
            if (i != 6) {
                t[i + 1]++;
                t[i] = t[i] - time_units[i];
            } else {
                for (hero = node_list; hero != 0; hero = hero->next)
                    hero->age++;
                t[6] = t[6] - time_units[6];
                year_rolled = 1;
            }
        }
    }
}

void far rest_carry(void)                                        /* 3d91:028e */
{
    time_add(rest_time);
    if (rest_time[5] > 0) {
        rest_time[4] += time_units[4] * rest_time[5];
        rest_time[5] = 0;
        if (rest_time[4] > 99)
            rest_time[4] = 99;
    }
}

void far clock_advance(unsigned char n, short unit)              /* 3d91:02bf */
{
    unsigned t[7];
    short i;
    unsigned char k;

    for (i = 0; i <= 6; i++)
        t[i] = cursor_obj[i + 5];
    for (k = 1; k <= n; k++) {
        t[unit]++;
        time_add(t);
    }
    for (i = 0; i <= 6; i++)
        cursor_obj[i + 5] = t[i];
    effects_tick(n, unit);
}

void far rest_take(unsigned char amount, short unit)             /* 3d91:0350 */
{
    short j;
    short k;

    if (rest_time[4] != 0 || rest_time[3] != 0 || rest_time[2] != 0 ||
        rest_time[1] != 0) {
        while (rest_time[unit] < amount) {
            for (j = unit + 1; rest_time[j] == 0 && j < 5; j = j + 1)
                ;
            if (j == 5) {
                mem_zero(rest_time, sizeof rest_time);
                amount = 0;
            } else
                for (k = j; unit + 1 <= k; k--) {
                    rest_time[k]--;
                    rest_time[k - 1] += time_units[k - 1];
                }
        }
        rest_time[unit] -= amount;
        rest_carry();
    }
}

void far two_digits(unsigned char n, char far *buf)              /* 3d91:041c */
{
    if (n < 10)
        str_format(buf, "0%d", n);
    else
        str_format(buf, "%d", n);
}

void far rest_show(short sel)                                    /* 3d91:0459 */
{
    short i;
    int attr[7];
    unsigned char row;
    char buf[42];

    for (i = 0; i <= 6; i++)
        attr[i] = 7;
    attr[sel] = 11;
    status_printf(1, 0x11, 7, 0, "Rest Time:");
    row = 11;
    for (i = 4; i >= 3; i--) {
        two_digits(rest_time[i], buf);
        status_printf(row + 1, 0x11, attr[i], 0, buf);
        status_printf(row + 3, 0x11, 7, 0, ":");
        row += 3;
    }
    two_digits(rest_time[2] * 10 + rest_time[1], buf);
    status_printf(row + 1, 0x11, attr[2], 0, buf);
}

char far rest_menu(void)                                         /* 3d91:054e */
{
    char go;
    unsigned char key;
    unsigned char code;
    short sel;

    ui_mode = 1;
    go = 0;
    sel = 2;
    do {
        rest_show(sel);
        hotkey_reset(6);
        key = menu_run(empty_text, rest_items, 1, 0);
        if (menu_escaped) {
            code = key;
            switch (code) {
            case 0x88:
                key = 4;
                break;
            case 0x84:
                key = 5;
                break;
            case 0x82:
            case 0x86:
                key = 0xff;
                break;
            default:
                key = 6;
            }
        }
        switch (key) {                  /* rest_items: Rest Days Hours */
        case 0:                         /* Mins Add Subtract Exit      */
            go = 1;
            break;
        case 1:
            sel = 4;
            break;
        case 2:
            sel = 3;
            break;
        case 3:
            sel = 2;
            break;
        case 4:
            if (sel == 2)
                rest_time[1] += 5;
            else
                rest_time[sel]++;
            rest_carry();
            break;
        case 5:
            if (sel == 2)
                rest_take(5, 1);
            else
                rest_take(1, sel);
            rest_carry();
            break;
        }
    } while (key != 6 && key != 0);
    return go;
}

void far heal_check(char show)                                   /* 3d91:0668 */
{
    char healed;
    struct Hero far *hero;

    heal_ticks++;
    if (heal_ticks >= 0x120) {
        healed = 0;
        for (hero = node_list; hero != 0; hero = hero->next)
            if (fx_heal(hero, 1, 0))
                healed = 1;
        if (show)
            rest_show(0);
        status_printf(1, 0x13, 7, 0, "The Whole Party Is Healed");
        if (healed)
            party_display(cur_node);
        msg_pause();
        combat_text_clear();
        heal_ticks = 0;
    }
}

char far memorize_one(struct Hero far *hero, char far *shown)    /* 3d91:070d */
{
    char r;
    char found;
    unsigned char i;

    if (no_study)
        r = 0;
    else {
        found = 0;
        i = 0;
        if (hero->active)
            for (; i <= 0x8c && found == 0; i++)
                if (hero->spells[i] & 0x80)
                    if (*shown)
                        found = spell_info[hero->spells[i] & 0x7f].level;
                    else {
                        hero->spells[i] -= 0x80;
                        rest_show(0);
                        cast_announce(hero, "has memorized", hero->spells[i]);
                        *shown = 1;
                    }
        r = found;
    }
    return r;
}

char far scribe_one(struct Hero far *hero, char far *shown)      /* 3d91:07e0 */
{
    char r;
    unsigned char j;
    unsigned char spell;
    char found;
    struct Item far *item;
    struct Item far *sub;

    r = 0;
    if (!no_study) {
        found = 0;
        for (item = hero->items; item != 0 && found == 0; item = item->next) {
            j = 1;
            if (item_casts(item)) {
                if (item->type == 'I') {
                    for (sub = item->more, tick_slot = 1;
                         item->slots[0] >= (unsigned char)tick_slot;
                         sub = sub->more, tick_slot++)
                        for (j = 1; j <= 3 && found == 0; j++)
                            if (sub->slots[j] > 0x80)
                                if (*shown)
                                    found = spell_info[sub->slots[j] & 0x7f].level;
                                else {
                                    spell = sub->slots[j] & 0x7f;
                                    hero->known[(spell - 1) / 8] =
                                        hero->known[(spell - 1) / 8] |
                                        bit_mask[(spell - 1) % 8];
                                    item_spend(hero, item, spell);
                                    rest_show(0);
                                    cast_announce(hero, "has scribed", spell);
                                    *shown = 1;
                                }
                } else
                    for (j = 1; j <= 3 && found == 0; j++)
                        if (item->slots[j] > 0x80)
                            if (*shown)
                                found = spell_info[item->slots[j] & 0x7f].level;
                            else {
                                spell = item->slots[j] & 0x7f;
                                hero->known[(spell - 1) / 8] =
                                    hero->known[(spell - 1) / 8] |
                                    bit_mask[(spell - 1) % 8];
                                item_spend(hero, item, spell);
                                rest_show(0);
                                cast_announce(hero, "has scribed", spell);
                                *shown = 1;
                            }
            }
        }
        r = found;
    }
    return r;
}

void far study_tick(void)                                        /* 3d91:0a77 */
{
    unsigned char i;
    unsigned char res;
    struct Hero far *hero;
    char shown;

    for (i = 1, hero = node_list; hero != 0; hero = hero->next, i++) {
        if (study_wait[i] > 0)
            study_wait[i]--;
        if (study_wait[i] == 0 && hero->study == 0) {
            shown = 0;
            res = scribe_one(hero, &shown);
            if (res == 0)
                res = memorize_one(hero, &shown);
            study_wait[i] = res * 3;
        }
    }
}

void far study_hour(unsigned char far *count)                    /* 3d91:0b24 */
{
    unsigned char i;
    unsigned char res;
    struct Hero far *hero;
    char shown;

    (*count)++;
    if (*count >= 12) {
        *count = 0;
        for (i = 1, hero = node_list; hero != 0; hero = hero->next, i++) {
            if (hero->study > 0) {
                hero->study--;
                if (hero->study == 0) {
                    shown = 1;
                    res = scribe_one(hero, &shown);
                    if (res == 0)
                        res = memorize_one(hero, &shown);
                    study_wait[i] = res * 3;
                }
            }
        }
    }
}

char far encounter_roll(unsigned char area)                      /* 3d91:0bc4 */
{
    char met;
    unsigned short rolls;

    met = 0;
    encounter_ticks += 5;
    saved_event = cur_event;
    cur_event = &event_table[cur_area_rec->rest_enc_wide[area].event - 1];
    if (cur_area_rec->rest_enc_wide[area].chance >= 0x80 && event_test(cur_area_rec->rest_enc_wide[area].event, 1))
        met = 1;
    else if (cur_area_rec->rest_enc_wide[area].chance > 0 && cur_area_rec->rest_enc_wide[area].event != 0 &&
             cur_area_rec->rest_enc_wide[area].ticks <= encounter_ticks) {
        rolls = encounter_ticks - cur_area_rec->rest_enc_wide[area].ticks;
        while (rolls > 0 && !met) {
            if (fx_roll_dice(1, 100) <= cur_area_rec->rest_enc_wide[area].chance &&
                event_test(cur_area_rec->rest_enc_wide[area].event, 1))
                met = 1;
            rolls--;
        }
    }
    cur_event = saved_event;
    return met;
}

char far camp_rest(char interactive)                             /* 3d91:0d37 */
{
    char result;
    struct Hero far *hero;
    unsigned char i;
    unsigned char area;
    unsigned char ticks;
    unsigned char steps;
    char stop;

    result = 0;
    if (saved_view_mode != 3)
        area = cell_zone_at(geo.x, geo.y);
    else
        area = cell_zone_at(cursor_obj[0x25], cursor_obj[0x26]);
    for (i = 1, hero = node_list; hero != 0; i++, hero = hero->next)
        study_wait[i] = 0;
    mem_fill(fx_live, sizeof fx_live, 1);
    ticks = 0;
    steps = 0;
    if (interactive) {
        text_clear(1, 0x11, 0x26, 0x16);
        rest_show(0);
    }
    resting = 1;
    if (interactive)
        stop = !rest_menu();
    else
        stop = 0;
    while (!stop && (rest_time[4] > 0 || rest_time[3] > 0 || rest_time[2] > 0 ||
                     rest_time[1] > 0)) {
        if (interactive && screen_sync()) {   /* 3d91:0e14 tests al */
            game_key_get();
            rest_show(0);
            if (confirm("Stop Resting?", 1))  /* 183a:044d reads only prompt */
                stop = 1;
            else
                box_draw();
        }
        if (!stop) {
            rest_take(5, 1);
            steps++;
            if (interactive && steps >= 5) {
                rest_show(0);
                steps = 0;
            }
            clock_advance(5, 1);
            heal_check(interactive);
            study_tick();
            study_hour(&ticks);
            if (in_service_event == 0) {
                stop = encounter_roll(area);
                if (stop) {
                    combat_text_clear();
                    rest_show(0);
                    status_printf(1, 0x13, 0xb, 0, "The party's repose is interrupted!");
                    result = 1;
                    msg_pause();
                    combat_text_clear();
                    events_run(cur_area_rec->rest_enc_wide[area].event);
                }
            }
        }
    }
    text_clear(1, 0x11, 0x26, 0x16);
    resting = 0;
    encounter_ticks = 0;
    return result;
}

int far spell_study_level_fallback(int spell)                                     /* 3d91:0f53 */
{
    switch (spell) {
    case 0x39:
        return 4;
    case 0x3d:
        return 3;
    case 0x3b:
        return 2;
    default:
        return 1;
    }
}
