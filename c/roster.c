/* CKIT.EXE overlay 04 module 1c86: the party roster panel and the info line.
 * view_refresh does nothing while adventure_exit is set; otherwise it redraws
 * the view behind the panel: the big picture when cursor_obj says one is up
 * and redrawn asks for it, else the first-person view through 18de:1495 in
 * view mode 4, and in view mode 3 unless cursor_obj[0x24] is 1.
 * party_rows redraws the roster lines of the highlighted member and of
 * previous_highlighted_character, the member marked last time, and clears previous_highlighted_character;
 * party_display draws the whole roster, a name, AC and HP line per member
 * under a heading.  Both set the panel rectangle panel_x1..panel_y2, its
 * bottom row counting the members.  party_info_display writes the three info
 * rows, 0x0d to 0x0f, except in view mode 3: the time of day, the party's
 * position when cursor_obj[0x1a] is 0, the compass letter's glyph, or an
 * error box naming the facing when the letter is none of E, N, S and W,
 * and "camping" or "searching".  party_key_select stores cur_node in previous_highlighted_character
 * whatever the key, and steps cur_node to the next member (keys 0x84 and
 * 0x85) or the previous one (0x87 and 0x88), wrapping at the ends.
 *
 * EXE 0x02db30..0x02e2cf, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 1c86, the first module of RTLink section 4 (payload
 * 0x2db30, link segment 1c86; a child of section 3).  Its last 15 bytes lie
 * in paragraph 1cff, whose module starts at 1cff:000f.  Built with Turbo C
 * 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped order; bake
 * consumes them in that order.  Section 5's 1c86 link segment holds other
 * bytes.
 *
 * A bare stored seg:off in this file means section 4.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a mod18de_ callee is an RTLink
 * vector stub whose target is named beside it, and big_redraw is section
 * 3's, reached by a direct far call because section 3 is section 4's
 * parent.  Every routine here is the target of a section 4 vector stub:
 * 0000 of 18de:1053, 0064 of 18de:1071, 023a of 18de:1067, 0406 of
 * 18de:105d, 06b6 of 18de:10a3; section 4 also far-calls each of them
 * directly.
 *
 * _DATA is eleven literals, DGROUP 5ae6..5b3d.  No _BSS is declared: the
 * panel rectangle panel_x1..panel_y2 is addressed only in this module, but
 * its gap b3e8..b3eb is not addressed here, so it is declared extern, as
 * in DQK.
 *
 * THIS IS THE v1.0 BUILD (CKIT.EXE "Version 1.0 March 19,1993"): the
 * addresses above are v1.2's; v1.0 places the module at file 0x02db90
 * (2c8c:0000, _DATA 5ae6, ten literals to 5b28).  Against v1.2, v1.0's
 * party_info_display has no default case in the compass switch: a facing whose
 * letter is none of E, N, S and W draws nothing and shows no error box
 * (so there is no "Facing %d/8 chr '%c'" literal).
 */

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[0x5c];           /* +4   not read here                     */
    char name[0x11];            /* +60                                    */
    char pad_71[0x110];         /* +71  not read here                     */
    unsigned char ac;           /* +181 60 is AC 0                        */
    char pad_182[9];            /* +182 not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct Geo {                    /* the party's place and facing           */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416, indexes dir_names           */
};

extern void far wild_pic_open(void);   /* 18de:14a9, vector to overlay 13 26e3:17e2 */
extern void far big_redraw(void);                                /* 1bbc:03a5 */
extern void far cursor_show(void);   /* 18de:149f, vector to overlay 8 2673:0006 */
extern void far view_draw(char x, char y, unsigned char dir);
                                      /* 18de:1495, vector to overlay 13 26e3:0bdb */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far character_name_show(struct Hero far *h, char col, char row, char poss);
                                                                 /* 03d0:1689 */
extern void far show_ac(struct Hero far *h, char col, char row); /* 03d0:06f9 */
extern void far show_hp(struct Hero far *h, char col, char row, char hilite,
                        char max);                               /* 03d0:0789 */
extern void far two_digits(unsigned char n, char far *buf);
                                      /* 18de:14b3, vector to overlay 19 2d91:041c */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */

extern char dir_names[8][3];                                     /* 5fb2 */
extern char byte_6726;                                           /* 6726 */
extern unsigned char panel_x1;                                   /* b3e6 */
extern unsigned char panel_y1;                                   /* b3e7 */
extern unsigned char panel_x2;                                   /* b3ec */
extern unsigned char panel_y2;                                   /* b3ed */
extern struct Hero far *cur_node;                                /* b3dd */
extern char redrawn;                                             /* b3f3 */
extern int view_mode;                                            /* b421 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char picture_hides_roster;                                           /* b761 */
extern char adventure_exit;                                           /* bcf6 */
extern struct Hero far *node_list;                               /* d129 */
extern struct Hero far *previous_highlighted_character;                               /* d12d */
extern struct Geo geo;                                           /* d414 */

void far view_refresh(void)                                      /* 2c86:0000 */
{
    if (!adventure_exit) {
        if (cursor_obj[0x22] == 0 && cursor_obj[0x24] == 1 && redrawn) {
            wild_pic_open();
            big_redraw();
            cursor_show();
            redrawn = 0;
        } else if (view_mode == 4 || (view_mode == 3 && cursor_obj[0x24] != 1)) {
            view_draw(geo.x, geo.y, geo.facing);
            redrawn = 0;
        }
    }
}

void far party_rows(struct Hero far *hl, char hilite)           /* 2c86:0064 */
{
    struct Hero far *p;
    unsigned char col;
    unsigned char row;
    unsigned char band;

    if ((view_mode == 3 && cursor_obj[0x24] == 1) || picture_hides_roster)
        return;
    if (previous_highlighted_character == 0)
        previous_highlighted_character = hl;
    if (view_mode == 0)
        col = 1;
    else
        col = 0x11;
    panel_x1 = col;
    panel_y1 = 4;
    panel_x2 = 0x26;
    panel_y2 = 3;
    row = 4;
    for (p = node_list; p != 0; p = p->next) {
        panel_y2++;
        if (p == previous_highlighted_character || p == hl) {
            if (hilite && p == hl)
                status_printf(col, row, 0xb, 0, "%s", p->name);
            else
                character_name_show(p, col, row, 0);
            if (p->ac <= 50 && p->ac >= 0)      /* >= 0 is compiled, as shipped */
                band = 1;
            else if (p->ac <= 60 && p->ac >= 51)
                band = 2;
            else if (p->ac <= 69 && p->ac >= 61)
                band = 1;
            else
                band = 0;
            show_ac(p, band + 0x20, row);
            if (p->hp > 99)
                band = 0;
            else if (p->hp > 9)
                band = 1;
            else
                band = 2;
            show_hp(p, band + 0x24, row, 0, 0);
        }
        row++;
    }
    if (row < 0xc)
        text_clear(col, row, 0x26, row);
    previous_highlighted_character = 0;
}

void far party_display(struct Hero far *hl)                        /* 2c86:023a */
{
    struct Hero far *p;
    unsigned char col;
    unsigned char row;
    unsigned char band;
    unsigned char v;

    if ((view_mode == 3 && !byte_6726 && cursor_obj[0x24] == 1 && cursor_obj[0x85] == 0)
        || picture_hides_roster)
        return;
    if (view_mode == 0)
        col = 1;
    else
        col = 0x11;
    panel_x1 = col;
    panel_y1 = 4;
    panel_x2 = 0x26;
    panel_y2 = 3;
    row = 2;
    status_printf(col, row, 0xc, 0, "Name");
    status_printf(0x21, row, 0xc, 0, "AC HP");
    row += 2;
    for (p = node_list; p != 0; p = p->next) {
        panel_y2++;
        text_clear(col, row, 0x26, row);
        if (p == hl)
            status_printf(col, row, 0xb, 0, "%s", p->name);
        else
            character_name_show(p, col, row, 0);
        v = p->ac;
        if (v < 0 || v > 69)                /* v < 0 is compiled, as shipped */
            band = 0;
        else if (v > 50 && v <= 60)
            band = 2;
        else
            band = 1;
        show_ac(p, band + 0x20, row);
        v = p->hp;
        if (v > 99)
            band = 0;
        else if (v > 9)
            band = 1;
        else
            band = 2;
        show_hp(p, band + 0x24, row, 0, 0);
        row++;
    }
    if (row < 0xc)
        text_clear(col, row, 0x26, row);
}

void far party_info_display(void)                                       /* 2c86:0406 */
{
    unsigned char x;
    unsigned char y;
    char mins[4];
    char pos[42];
    char info[42];

    if (view_mode == 3)
        return;
    info[0] = 0;
    pos[0] = 0;
    x = geo.x;
    y = geo.y;
    two_digits(cursor_obj[7] * 10 + cursor_obj[6], mins);
    text_clear(0x1a, 0x0d, 0x26, 0x0f);
    if (cursor_obj[8] / 12 == 0) {
        if (cursor_obj[8] == 0)
            str_copy(info, msg_format("12:%s AM", mins));
        else
            str_copy(info, msg_format("%d:%s AM", cursor_obj[8], mins));
    } else if (cursor_obj[8] == 12)
        str_copy(info, msg_format("%d:%s PM", cursor_obj[8], mins));
    else
        str_copy(info, msg_format("%d:%s PM", cursor_obj[8] - 12, mins));
    status_printf(0x1a, 0x0f, 7, 0, info);
    if (cursor_obj[0x1a] == 0) {
        str_copy(pos, msg_format("%d,%d", x, y));
        status_printf(0x1a, 0x0d, 7, 0, pos);
    }
    switch (dir_names[geo.facing][0]) {
    case 'E':
        draw_glyph(0x1f40, 0x1f40, 1, 0x19);
        break;
    case 'N':
        draw_glyph(0x1f40, 0x1f40, 1, 0x16);
        break;
    case 'S':
        draw_glyph(0x1f40, 0x1f40, 1, 0x17);
        break;
    case 'W':
        draw_glyph(0x1f40, 0x1f40, 1, 0x18);
        break;
    }
    str_cat(pos, " ");             /* pos is not shown again, as shipped */
    if (view_mode == 2)
        status_printf(0x1a, 0x0e, 7, 0, "camping");
    else if (cursor_obj[0x19] & 1)
        status_printf(0x1a, 0x0e, 7, 0, "searching");
}

void far party_key_select(unsigned char key)                          /* 2c86:06b6 */
{
    struct Hero far *p;
    register int k;

    previous_highlighted_character = cur_node;
    k = key;
    p = node_list;
    if (p == 0)
        return;
    if (k == 0x87 || k == 0x85 || k == 0x84 || k == 0x88) {
        switch (k) {
        case 0x87:
        case 0x88:
            if (p == cur_node)
                while (p->next != 0)
                    p = p->next;
            else
                while (p != 0 && p->next != cur_node)
                    p = p->next;
            break;
        case 0x84:
        case 0x85:
            if (cur_node->next != 0)
                p = cur_node->next;
            break;
        }
        cur_node = p;
    }
}
