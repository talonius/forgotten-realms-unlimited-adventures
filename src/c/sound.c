/* CKIT.EXE resident module 1ac2: the sound layer over the AIL drivers.
 *
 * EXE 0x00d02d..0x00e7f3, one Turbo C large-model _TEXT segment starting at
 * byte d of paragraph 1ac2.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 1a75's last retf is at 0x00d02c, and module 1c3f starts at
 * 0x00e7f3 (1c3f:0003) with no stub between.
 *
 * Two devices: [0] plays XMIDI music (.XMI) through an .ADV driver, [1]
 * plays digitised sound (.VOC).  A driver file is read into a memory slot,
 * aligned to a paragraph and registered; each call names the data file,
 * whose extension selects the device.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  A modNNNN_ name is a placeholder
 * that says only which segment the callee lives in.  The 000c callees are a
 * driver API table (6-byte stride); the AIL 2 function beside each is Dark
 * Queen of Krynn's reading of its call shape, which no module defines.
 * sound_card is DQK's name for the +2 field of the config record its
 * config.c defines.
 *
 * 19 of the 21 static helpers are reached with `push cs; call near` ahead of
 * their definitions, which Turbo C gives only a static function.  The other
 * two (xmi_install_timbres, timbre_file_name) are called only after theirs,
 * where a public function is called the same way, so the bytes do not force
 * their `static`.  The two callbacks it hands out are public: each is
 * pushed with a `mov ax,seg`, and timbre_load_cb is far-called as well.
 *
 * -d merges the two "ADV" literals into one (DGROUP 1de6).  _BSS follows
 * the statics' names (Turbo C's symbol-table order), not their declaration
 * order; these names reproduce the shipped ad62..ad91 layout.
 */

struct DriverDesc {                     /* AIL driver description */
    int  min_api;                       /* +00 */
    int  type;                          /* +02 3 = XMIDI, 2 = digital */
    char suffix[4];                     /* +04 timbre file extension */
    void far *names;                    /* +08 */
    int  io;                            /* +0c */
    int  irq;                           /* +0e */
    int  dma;                           /* +10 */
    int  drq;                           /* +12 */
};

struct SoundDev {
    int  kind;                          /* +00 0 = music, 1 = digital */
    int  driver;                        /* +02 driver handle, -1 = none */
    char driver_file[12];               /* +04 */
    char loaded[12];                    /* +10 data file now resident */
    int  count;                         /* +1c sequences or VOC markers */
    int  slot;                          /* +1e memory slot of the driver */
    void far *image;                    /* +20 driver image, para-aligned */
    struct DriverDesc far *desc;        /* +24 */
    char far *data;                     /* +28 the loaded data file */
    int  seq[6];                        /* +2c sequence handles, -1 = none */
    void far *state[6];                 /* +38 sequence state tables */
};

union TimbreKey {
    unsigned w;
    signed char b[2];                   /* patch, bank; bank -1 ends a list */
};

struct TimbreEntry {                    /* one record of the timbre index */
    signed char patch;
    signed char bank;
    long offset;
};

enum {
    EXT_XMI  = 0x4d58,                  /* "XM", the low word of "XMI" */
    EXT_VOC  = 0x4f56,                  /* "VO", the low word of "VOC" */
    DOS_READ = 0x3f00                   /* int 21h AH=3fh */
};

extern int  far AIL_startup(void);                           /* 000c:06b9 startup */
extern void far AIL_shutdown(char far *msg);                  /* 000c:06fd shutdown */
extern int  far AIL_register_timer(void (far *fn)(void));           /* 000c:0762 register_timer */
extern void far AIL_release_timer_handle(int timer);                      /* 000c:080e release_timer_handle */
extern void far AIL_start_timer(int timer);                      /* 000c:0870 start_timer */
extern void far AIL_set_timer_divisor(int timer, unsigned d);          /* 000c:0992 set_timer_divisor */
extern int  far AIL_register_driver(void far *image);                /* 000c:09e2 register_driver */
extern struct DriverDesc far *far AIL_describe_driver(int drv);      /* 000c:0a9a describe_driver */
extern int  far AIL_detect_device(int drv, int io, int irq, int dma, int drq); /* 000c:0ac0 detect_device */
extern void far AIL_init_driver(int drv, int io, int irq, int dma, int drq); /* 000c:0ac6 init_driver */
extern void far AIL_play_VOC_file(int drv, void far *voc, int marker); /* 000c:0bb7 play_VOC_file */
extern int  far AIL_VOC_playback_status(int drv);                        /* 000c:0bbd VOC_playback_status */
extern void far AIL_start_digital_playback(int drv);                        /* 000c:0bc3 start_digital_playback */
extern void far AIL_stop_digital_playback(int drv);                        /* 000c:0bc9 stop_digital_playback */
extern void far AIL_pause_digital_playback(int drv);                        /* 000c:0bcf pause_digital_playback */
extern void far AIL_resume_digital_playback(int drv);                        /* 000c:0bd5 resume_digital_playback */
extern void far AIL_set_digital_playback_volume(int drv, int volume);            /* 000c:0bdb set_digital_playback_volume */
extern unsigned far AIL_state_table_size(int drv);                    /* 000c:0bf3 state_table_size */
extern int  far AIL_register_sequence(int drv, void far *file, int n,
                             void far *state, void far *ctl); /* 000c:0bf9 register_sequence */
extern void far AIL_release_sequence_handle(int drv, int seq);               /* 000c:0bff release_sequence_handle */
extern unsigned far AIL_default_timbre_cache_size(int drv);                    /* 000c:0c05 default_timbre_cache_size */
extern void far AIL_define_timbre_cache(int drv, void far *cache, unsigned size); /* 000c:0c0b define_timbre_cache */
extern unsigned far AIL_timbre_request(int drv, int seq);           /* 000c:0c11 timbre_request */
extern void far AIL_install_timbre(int drv, int bank, int patch, void far *data); /* 000c:0c17 install_timbre */
extern void far AIL_start_sequence(int drv, int seq);               /* 000c:0c2f start_sequence */
extern void far AIL_stop_sequence(int drv, int seq);               /* 000c:0c35 stop_sequence */
extern void far AIL_resume_sequence(int drv, int seq);               /* 000c:0c3b resume_sequence */
extern int  far AIL_sequence_status(int drv, int seq);               /* 000c:0c41 sequence_status */
extern void far AIL_set_relative_volume(int drv, int seq, int percent, int ms); /* 000c:0c53 set_relative_volume */

extern void far timer_unhook(void);                              /* 107e:007f */
extern void far timer_rehook(void);                              /* 107e:0047 */
extern void far timer_tick(void);                              /* 00d7:0009 */
extern char far path_default_ext(char far *path, char far *ext, char force); /* 17a1:0264 */
extern void far path_name(char far *path, char far *out);        /* 17d3:02b1 */
extern void far path_ext(char far *path, char far *out);         /* 17d3:02ea */
extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern long far file_seek(int fd, long pos, int whence);      /* 00d7:2705 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern long far file_read(int fd, char far *buf, long n);        /* 1336:000e */
extern char far slot_open(char far *name, int slot);             /* 1336:025c */
extern void far slot_release(char far *name);                    /* 1336:0708 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern long far slot_used(int slot);                             /* 1336:0921 */
extern char far slot_reserve(int slot, long n);                  /* 1336:0a04 */
extern int  far arg_hex(char far *name, int dflt); /* 18de:10c1, RTLink vector into section 6 */
extern char far archive_open(char disk, char far *name, int flags,
                             char (far *fn)());                  /* 0e23:01eb */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)());                     /* 0e23:03fa */
extern char far key_pressed(void);                               /* 0da7:009a */
extern void far key_flush(void);                                 /* 0da7:00bd */

struct Config {                 /* config.c's record                      */
    int video;                  /* +0  0 VGA, 1 EGA, 2 Tandy 16-colour  */
    int sound_card;             /* +2                                   */
    unsigned char rest[0x46];
};

extern struct Config config;                                     /* b392 */

static int driver_load(struct SoundDev far *dev, char far *path);
static int driver_read(struct SoundDev far *dev, char far *path);
static int driver_align(struct SoundDev far *dev);
static int driver_start(struct SoundDev far *dev);
static int voc_init(struct SoundDev far *dev);
static int voc_release(struct SoundDev far *dev);
static int voc_load(struct SoundDev far *dev, char far *path);
static int voc_stop(struct SoundDev far *dev, char wait);
static int voc_count_markers(struct SoundDev far *dev);
static int xmi_init(struct SoundDev far *dev);
static int xmi_release(struct SoundDev far *dev);
static int xmi_alloc_states(struct SoundDev far *dev);
static int xmi_alloc_cache(struct SoundDev far *dev);
static int xmi_install_timbres(struct SoundDev far *dev, unsigned n, int fd);
static int timbre_file_name(struct SoundDev far *dev, char far *out);
static void far *timbre_find(int fd, char far *key);
static int xmi_load(struct SoundDev far *dev, char far *path);
static int xmi_stop(struct SoundDev far *dev, char wait);
static int xmi_register(struct SoundDev far *dev);
char far timbre_load_cb(int fd);
static int file_load(char far *path, void far *buf, long limit);
char far file_load_cb(int fd);
static void far *slot_alloc(long n, int slot);

static int driver_type[2] = { 3, 2 };                            /* 1ce6 */
static struct SoundDev devices[2] = { 0 };                       /* 1cea */
static long data_ext[2] = { 0x00494d58L, 0x00434f56L };         /* 1d8a "XMI", "VOC" */
static void (far *timer_fn)(void) = timer_tick;                /* 1d92 */

static unsigned tlen;                                            /* ad62 */
static struct SoundDev far *tdev;                                /* ad64 */
static int timer_id;                                             /* ad68 */
static void far *load_buf;                                       /* ad6a */
static unsigned timbre_buf[16];                                  /* ad6e */
static long load_limit;                                          /* ad8e */

int far sound_init(char far *music_drv, char far *voc_drv)       /* 1ac2:000d */
{
    short started;
    struct SoundDev far *dev;
    register int rc;
    register int missing;

    rc = 0;
    started = 0;
    missing = 0;
    if (timer_fn)
        timer_unhook();
    AIL_startup();
    if (timer_fn) {
        timer_id = AIL_register_timer(timer_fn);
        AIL_set_timer_divisor(timer_id, 0x4000);
        AIL_start_timer(timer_id);
    }
    if (music_drv && *music_drv) {
        path_default_ext(music_drv, "ADV", 1);
        dev = &devices[0];
        if (dev->image == 0) {
            dev->kind = 0;
            if ((rc = driver_load(dev, music_drv)) == 0) {
                rc = xmi_init(dev);
                started += rc == 0 ? 1 : 0;
            } else if (rc == -2)
                missing = 1;
        }
    }
    if (!rc || missing) {
        if (voc_drv && *voc_drv) {
            path_default_ext(voc_drv, "ADV", 1);
            dev = &devices[1];
            if (dev->image == 0) {
                dev->kind = 1;
                if ((rc = driver_load(dev, voc_drv)) == 0) {
                    rc = voc_init(dev);
                    started += rc == 0 ? 1 : 0;
                } else if (rc == -2)
                    missing += 2;
            }
        }
    }
    return missing ? -missing : started;
}

void far sound_shutdown(void)                                    /* 1ac2:0182 */
{
    struct SoundDev far *dev;
    register int i;

    for (i = 0; i < 2; i++) {
        dev = &devices[i];
        if (dev->image) {
            switch (dev->kind) {
            case 0:
                if (dev->loaded[0])
                    xmi_stop(dev, 0);
                xmi_release(dev);
                break;
            case 1:
                if (dev->loaded[0])
                    voc_stop(dev, 0);
                voc_release(dev);
                break;
            }
            dev->data = 0;
        }
    }
    if (timer_fn)
        AIL_release_timer_handle(timer_id);
    AIL_shutdown(0);
    for (i = 0; i < 2; i++)
        slot_release(dev->driver_file); /* dev stays devices[1] (1ac2:023f) */
    if (timer_fn)
        timer_rehook();
}

int far sound_play(char far *file, register int index, int volume) /* 1ac2:026a */
{
    short i;
    long ext;
    struct SoundDev far *dev;
    char ext_buf[6];
    char name[12];
    register int rc;

    rc = 0;
    path_ext(file, ext_buf);
    ext = *(long *)ext_buf;
    for (i = 0; i < 2; i++)
        if (data_ext[i] == ext)
            break;
    if (i >= 2)
        return -10;
    if ((dev = &devices[i]) == 0)
        return 0;
    path_name(file, name);
    switch ((int)ext) {
    case EXT_XMI:
        if (str_cmp(name, dev->loaded)) {
            if (dev->loaded[0])
                xmi_stop(dev, 0);
            dev->count = int_min(xmi_load(dev, file), 6);
            if (dev->count > 0)
                str_copy(dev->loaded, name);
            else
                rc = -3;
        }
        if (!rc && index >= 0 && dev->count > index && dev->seq[index] != -1) {
            AIL_set_relative_volume(dev->driver, dev->seq[index], volume, 0);
            AIL_start_sequence(dev->driver, dev->seq[index]);
        } else if (!rc)
            rc = -5;
        break;
    case EXT_VOC:
        if (str_cmp(name, dev->loaded)) {
            if (dev->loaded[0])
                voc_stop(dev, 0);
            dev->count = int_min(voc_load(dev, file), 0x7ffe);
            if (dev->count > 0)
                str_copy(dev->loaded, name);
            else
                rc = -3;
        }
        if (!rc && index >= 0 && dev->count > index) {
            AIL_play_VOC_file(dev->driver, dev->data, index);
            AIL_set_digital_playback_volume(dev->driver, volume);
            AIL_start_digital_playback(dev->driver);
        } else if (!rc)
            rc = -5;
        break;
    default:
        rc = -10;
    }
    return rc;
}

int far sound_volume(char far *file, int index, int volume)      /* 1ac2:04d0 */
{
    long ext;
    struct SoundDev far *dev;
    char ext_buf[6];
    char name[12];
    register int rc;
    register int i;

    rc = 0;
    path_ext(file, ext_buf);
    ext = *(long *)ext_buf;
    for (i = 0; i < 2; i++)
        if (data_ext[i] == ext)
            break;
    if (i >= 2)
        return -10;
    if ((dev = &devices[i]) == 0)
        return 0;
    path_name(file, name);
    switch ((int)ext) {
    case EXT_XMI:
        if (str_cmp(name, dev->loaded))
            break;
        if (!rc && index >= 0 && dev->count > index && dev->seq[index] != -1)
            AIL_set_relative_volume(dev->driver, dev->seq[index], volume, 0);
        else if (!rc)
            rc = -5;
        break;
    case EXT_VOC:
        if (str_cmp(name, dev->loaded))
            break;
        if (!rc && index >= 0 && dev->count > index)
            AIL_set_digital_playback_volume(dev->driver, volume);
        else if (!rc)
            rc = -5;
        break;
    default:
        rc = -10;
    }
    return rc;
}

int far sound_wait(char far *file, int index, char interruptible) /* 1ac2:0625 */
{
    long ext;
    short seq;
    struct SoundDev far *dev;
    char name[12];
    char ext_buf[6];
    register int i;
    register int rc;

    rc = 0;
    path_name(file, name);
    path_ext(file, ext_buf);
    ext = *(long *)ext_buf;
    for (i = 0; i < 2; i++)
        if (data_ext[i] == ext)
            break;
    if (i >= 2)
        return -10;
    if ((dev = &devices[i]) == 0)
        return 0;
    if (str_cmp(name, dev->loaded))
        return -3;
    if (index < 0 || dev->count <= index)
        return -5;
    switch ((int)ext) {
    case EXT_XMI:
        seq = dev->seq[index];
        if (interruptible) {
            while (AIL_sequence_status(dev->driver, seq) == 1)
                if (key_pressed()) {
                    AIL_stop_sequence(dev->driver, seq);
                    key_flush();
                    break;
                }
        } else
            while (AIL_sequence_status(dev->driver, seq) == 1)
                ;
        break;
    case EXT_VOC:
        if (interruptible) {
            while (AIL_VOC_playback_status(dev->driver) == 2)
                if (key_pressed()) {
                    AIL_stop_digital_playback(dev->driver);
                    key_flush();
                    break;
                }
        } else
            while (AIL_VOC_playback_status(dev->driver) == 2)
                ;
        break;
    default:
        rc = -10;
    }
    return rc;
}

int far sound_stop(char far *file, int index)                    /* 1ac2:07b0 */
{
    long ext;
    short seq;
    struct SoundDev far *dev;
    char name[12];
    char ext_buf[6];
    register int i;
    register int rc;

    rc = 0;
    path_name(file, name);
    path_ext(file, ext_buf);
    ext = *(long *)ext_buf;
    for (i = 0; i < 2; i++)
        if (data_ext[i] == ext)
            break;
    if (i >= 2)
        return -10;
    if ((dev = &devices[i]) == 0)
        return 0;
    if (str_cmp(name, dev->loaded))
        return -3;
    if (index < 0 || dev->count <= index)
        return -5;
    switch ((int)ext) {
    case EXT_XMI:
        seq = dev->seq[index];
        if (AIL_sequence_status(dev->driver, seq) == 1)
            AIL_stop_sequence(dev->driver, seq);
        break;
    case EXT_VOC:
        if (AIL_VOC_playback_status(dev->driver) == 2)
            AIL_stop_digital_playback(dev->driver);
        break;
    default:
        rc = -10;
    }
    return rc;
}

int far sound_pause(char far *file, register int index)          /* 1ac2:08e0 */
{
    short i;
    long ext;
    struct SoundDev far *dev;
    char ext_buf[6];
    char name[12];
    register int rc;

    rc = 0;
    path_ext(file, ext_buf);
    ext = *(long *)ext_buf;
    for (i = 0; i < 2; i++)
        if (data_ext[i] == ext)
            break;
    if (i >= 2)
        return -10;
    if ((dev = &devices[i]) == 0)
        return 0;
    path_name(file, name);
    switch ((int)ext) {
    case EXT_XMI:
        if (!rc && index >= 0 && dev->count > index && dev->seq[index] != -1) {
            if (AIL_sequence_status(dev->driver, dev->seq[index]) == 1)
                AIL_stop_sequence(dev->driver, dev->seq[index]);
        } else if (!rc)
            rc = -5;
        break;
    case EXT_VOC:
        if (!rc && index >= 0 && dev->count > index)
            AIL_pause_digital_playback(dev->driver);
        else if (!rc)
            rc = -5;
        break;
    default:
        rc = -10;
    }
    return rc;
}

int far sound_resume(char far *file, int index)                  /* 1ac2:0a10 */
{
    long ext;
    struct SoundDev far *dev;
    char ext_buf[6];
    char name[12];
    register int rc;
    register int i;

    rc = 0;
    path_ext(file, ext_buf);
    ext = *(long *)ext_buf;
    for (i = 0; i < 2; i++)
        if (data_ext[i] == ext)
            break;
    if (i >= 2)
        return -10;
    if ((dev = &devices[i]) == 0)
        return 0;
    path_name(file, name);
    switch ((int)ext) {
    case EXT_XMI:
        if (str_cmp(name, dev->loaded)) {
            rc = -6;
            break;
        }
        if (!rc && index >= 0 && dev->count > index && dev->seq[index] != -1)
            AIL_resume_sequence(dev->driver, dev->seq[index]);
        else if (!rc)
            rc = -5;
        break;
    case EXT_VOC:
        if (str_cmp(name, dev->loaded)) {
            rc = -6;
            break;
        }
        if (!rc && index >= 0 && dev->count > index)
            AIL_resume_digital_playback(dev->driver);
        else if (!rc)
            rc = -5;
        break;
    default:
        rc = -10;
    }
    return rc;
}

static int driver_load(struct SoundDev far *dev, char far *path) /* 1ac2:0b59 */
{
    register int rc;

    rc = 0;
    dev->driver = -1;
    switch (dev->kind) {
    case 0:
        dev->slot = 0x16;
        break;
    case 1:
        dev->slot = 0x17;
        break;
    default:
        rc = -1;
    }
    if ((rc = driver_read(dev, path)) == 0)
        rc = driver_start(dev);
    return rc;
}

static int driver_read(struct SoundDev far *dev, char far *path) /* 1ac2:0bbd */
{
    register int rc;

    rc = 0;
    path_name(path, dev->driver_file);
    if (!slot_open(dev->driver_file, dev->slot)) {
        if ((rc = file_load(path, slot_base(dev->slot), -1L)) == 0) {
            if (slot_alloc(load_limit - slot_used(dev->slot), dev->slot))
                rc = driver_align(dev);
            else
                rc = -9;
        } else
            rc = -3;
    } else
        rc = -7;
    return rc;
}

static int driver_align(struct SoundDev far *dev)                /* 1ac2:0c73 */
{
    long pad;
    long used;
    char far *base;
    char far *image;
    register int rc;

    rc = 0;
    image = base = slot_base(dev->slot);
    if ((pad = (long)image & 0xf) != 0) {
        pad = 16 - pad;
        if (slot_reserve(dev->slot, pad)) {
            used = slot_used(dev->slot);
            image = huge_add(image, pad);
            huge_move(base, image, used);
        } else
            rc = -9;
    }
    if (!rc)
        dev->image = image;
    return rc;
}

static int driver_start(struct SoundDev far *dev)                /* 1ac2:0d2d */
{
    struct DriverDesc far *desc;
    register int rc;

    rc = 0;
    if (dev == 0)
        return -7;
    dev->driver = AIL_register_driver(dev->image);
    if (dev->driver == -1)
        rc = 0;                         /* and desc is stored unset below */
    else {
        desc = AIL_describe_driver(dev->driver);
        if ((config.sound_card == 3 || config.sound_card == 4) && dev->kind == 0)
            desc->io = arg_hex("RB", desc->io);
        if ((config.sound_card == 2 || config.sound_card == 4) && dev->kind == 1)
            desc->io = arg_hex("SB", desc->io);
        if ((config.sound_card == 2 || config.sound_card == 4) && dev->kind == 1)
            desc->irq = arg_hex("SI", desc->irq);
        if (desc->type != driver_type[dev->kind])
            rc = -7;
        else if (!(AIL_detect_device(dev->driver, desc->io, desc->irq, desc->dma, desc->drq)
                   || dev->kind == 1 && desc->irq == 7
                      && (desc->irq = 5,
                          AIL_detect_device(dev->driver, desc->io, desc->irq,
                                       desc->dma, desc->drq))))
            rc = -2;
        else
            AIL_init_driver(dev->driver, desc->io, desc->irq, desc->dma, desc->drq);
    }
    if (!rc)
        dev->desc = desc;
    return rc;
}

static int voc_init(struct SoundDev far *dev)                    /* 1ac2:0ed2 */
{
    register int rc;

    rc = 0;
    if (dev == 0 || dev->kind != 1)
        return -7;
    dev->data = slot_alloc(0x78beL, dev->slot);
    if (dev->data == 0)
        rc = -9;
    return rc;
}

static int voc_release(struct SoundDev far *dev)                 /* 1ac2:0f20 */
{
    register int rc;

    rc = 0;
    if (dev == 0 || dev->kind != 1)
        rc = -7;
    return rc;
}

static int voc_load(struct SoundDev far *dev, char far *path)    /* 1ac2:0f3f */
{
    short markers;

    markers = 0;
    if (dev->image)
        if (file_load(path, dev->data, 0x78beL) == 0)
            markers = voc_count_markers(dev);
    return markers;
}

static int voc_stop(struct SoundDev far *dev, char wait)         /* 1ac2:0f8d */
{
    register int rc;

    rc = 0;
    if (dev == 0)
        return -7;
    if (wait)
        while (AIL_VOC_playback_status(dev->driver) == 2)
            ;
    AIL_stop_digital_playback(dev->driver);
    return rc;
}

static int voc_count_markers(struct SoundDev far *dev)           /* 1ac2:0fca */
{
    unsigned short markers;
    char far *p;
    long len;

    markers = 0;
    if (dev->data == 0)
        return -8;
    p = huge_add(dev->data, 0x14L); /* the header's data offset */
    len = *(int far *)p;
    p = huge_add(dev->data, len);
    while (*p != 0 && markers < 0x7ffe) {
        if (*p == 4)                    /* VOC block type 4: marker */
            markers++;
        p = huge_add(p, 1L);
        len = *(long far *)p & 0xffffffL;
        p = huge_add(p, len + 3);
    }
    return markers;
}

static int xmi_init(struct SoundDev far *dev)                    /* 1ac2:10a1 */
{
    register int rc;
    register int i;

    rc = 0;
    if (dev == 0 || dev->kind != 0)
        return -7;
    for (i = 0; i < 6; i++) {
        dev->seq[i] = -1;
        dev->state[i] = 0;
    }
    dev->data = slot_alloc(0x2148L, dev->slot);
    if (dev->data == 0)
        rc = -9;
    if (!rc && (rc = xmi_alloc_states(dev)) == 0)
        rc = xmi_alloc_cache(dev);
    return rc;
}

static int xmi_release(struct SoundDev far *dev)                 /* 1ac2:1147 */
{
    register int rc;

    rc = 0;
    dev = dev;                          /* the lone les bx at 1ac2:114d */
    return rc;
}

static int xmi_alloc_states(struct SoundDev far *dev)            /* 1ac2:1155 */
{
    long size;
    register int i;
    register int rc;

    rc = 0;
    if (dev == 0)
        return -7;
    size = AIL_state_table_size(dev->driver);
    for (i = 0; i < 6; i++)
        if ((dev->state[i] = slot_alloc(size, dev->slot)) == 0) {
            rc = -9;
            break;
        }
    return rc;
}

static int xmi_alloc_cache(struct SoundDev far *dev)             /* 1ac2:11c9 */
{
    void far *cache;
    register int rc;
    register unsigned size;

    rc = 0;
    if (dev == 0)
        return -7;
    if ((size = AIL_default_timbre_cache_size(dev->driver)) != 0) {
        cache = slot_alloc((long)size, dev->slot);
        if (cache)
            AIL_define_timbre_cache(dev->driver, cache, size);
        else
            rc = -9;
    }
    return rc;
}

static int xmi_install_timbres(struct SoundDev far *dev, unsigned n, int fd) /* 1ac2:1234 */
{
    unsigned short request;
    void far *timbre;
    union TimbreKey key;
    register int rc;
    register int seq;

    rc = 0;
    if (dev == 0 || n < 0 || n >= 6)
        return 0;
    seq = dev->seq[n];
    key.w = request = AIL_timbre_request(dev->driver, seq);
    while (!rc && key.b[1] != -1) {
        if ((timbre = timbre_find(fd, key.b)) != 0) {
            AIL_install_timbre(dev->driver, key.b[1], key.b[0], timbre);
            key.w = request = AIL_timbre_request(dev->driver, seq);
        } else
            rc = -4;
    }
    return rc;
}

static int timbre_file_name(struct SoundDev far *dev, char far *out) /* 1ac2:12c7 */
{
    char far *p;
    register int i;
    register int rc;

    rc = 0;
    if (dev == 0)
        return -7;
    str_copy(out, "INSTR");
    for (p = out; *p; p++)
        ;
    i = 0;
    *p++ = '.';
    for (; i < 4; i++, p++)
        if ((*p = dev->desc->suffix[i]) == 0)
            break;
    if (*p)
        *p = 0;
    return rc;
}

static void far *timbre_find(register int fd, char far *key)     /* 1ac2:1348 */
{
    unsigned far *timbre;
    struct TimbreEntry entry;

    if (fd < 0)
        return 0;
    if (file_seek(fd, 0L, 0) < 0)
        return 0;
    do {
        dos_io(DOS_READ, &entry, fd, 6);
        if (entry.bank == -1)
            return 0;
    } while (key[1] != entry.bank || key[0] != entry.patch);
    if (file_seek(fd, entry.offset, 0) < 0)
        return 0;
    if (dos_io(DOS_READ, &tlen, fd, 2) != 2 || tlen > 0x20)
        return 0;
    timbre = timbre_buf;
    *timbre = tlen;
    if (dos_io(DOS_READ, timbre + 1, fd, tlen - 2) != tlen - 2)
        return 0;
    return timbre;
}

static int xmi_load(struct SoundDev far *dev, char far *path)    /* 1ac2:142f */
{
    register int rc;

    rc = 0;
    if (dev == 0)
        return -7;
    if (file_load(path, dev->data, 0x2148L) == 0)
        rc = xmi_register(dev);
    return rc;
}

static int xmi_stop(struct SoundDev far *dev, char wait)         /* 1ac2:1478 */
{
    register int i;
    register int rc;

    rc = 0;
    if (dev == 0)
        return -7;
    for (i = 0; i < 6; i++) {
        if (dev->seq[i] == -1)
            break;
        if (wait)
            while (AIL_sequence_status(dev->driver, dev->seq[i]) == 1)
                ;
        AIL_stop_sequence(dev->driver, dev->seq[i]);
        AIL_release_sequence_handle(dev->driver, dev->seq[i]);
    }
    return rc;
}

static int xmi_register(struct SoundDev far *dev)                /* 1ac2:1509 */
{
    short rc = 0;
    char timbre_file[80] = "";
    register unsigned i;
    register int n;

    if (dev == 0)
        return -7;
    if (dev->data == 0) {
        rc = -8;
        n = 0;
    } else {
        for (i = 0; !rc && i < 6; i++) {
            dev->seq[i] = AIL_register_sequence(dev->driver, dev->data, i,
                                       dev->state[i], 0);
            if (dev->seq[i] == -1)
                break;
        }
        n = i;
        if (n > 0 && timbre_file_name(dev, timbre_file) == 0) {
            tdev = dev;
            if (!archive_open(0xb1, timbre_file, 0, timbre_load_cb))
                if (!timbre_load_cb(-1))
                    file_open(0x31, timbre_file, 0, timbre_load_cb);
        }
    }
    return n;
}

char far timbre_load_cb(int fd)                                  /* 1ac2:1623 */
{
    register unsigned i;
    register int rc;

    i = rc = 0;
    for (; !rc && i < 6; i++) {
        if (tdev->seq[i] == -1)
            break;
        rc = xmi_install_timbres(tdev, i, fd);
    }
    return rc == 0;
}

static int file_load(char far *path, void far *buf, long limit)  /* 1ac2:166d */
{
    register int rc;

    rc = 0;
    load_limit = limit;
    load_buf = buf;
    file_open(0x31, path, 0, file_load_cb);
    return rc;
}

char far file_load_cb(int fd)                                    /* 1ac2:16ac */
{
    long got;
    long want;
    char ok;

    ok = 0;
    want = load_limit >= 0 ? load_limit : 0x7fffffffL;
    got = file_read(fd, load_buf, want);
    if (got > 0 && got <= load_limit)
        ok = 1;
    else if (got > 0 && load_limit < 0) {
        load_limit = got;
        ok = 1;
    }
    return ok;
}

static void far *slot_alloc(long n, register int slot)           /* 1ac2:174a */
{
    char far *p;
    long used;

    if (n < 0)
        n = -n;
    p = slot_base(slot);
    used = slot_used(slot);
    if (slot_reserve(slot, n))
        p = huge_add(p, used);
    else
        p = 0;
    return p;
}
