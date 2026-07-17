/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Implementation of Socket 370 (PGA370) machines.
 *
 * Authors: Miran Grca, <mgrca8@gmail.com>
 *
 *          Copyright 2016-2025 Miran Grca.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <86box/86box.h>
#include <86box/mem.h>
#include <86box/io.h>
#include <86box/rom.h>
#include <86box/pci.h>
#include <86box/device.h>
#include <86box/chipset.h>
#include <86box/hdc.h>
#include <86box/hdc_ide.h>
#include <86box/keyboard.h>
#include <86box/flash.h>
#include <86box/sio.h>
#include <86box/hwm.h>
#include <86box/spd.h>
#include <86box/video.h>
#include "cpu.h"
#include <86box/machine.h>
#include <86box/clock.h>
#include <86box/sound.h>
#include <86box/snd_ac97.h>

/* i440LX */
static const device_config_t s370slm_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "s370slm",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "AMIBIOS 6 (071595) - Revision R1.35",
                .internal_name = "s370slm_135",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/s370slm/3LM1202.rom", "" }
            },
            {
                .name          = "AMIBIOS 6 (071595) - Revision R1.4",
                .internal_name = "s370slm",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/s370slm/3LMA112.ROM", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t s370slm_device = {
    .name          = "Supermicro 370SLM",
    .internal_name = "s370slm",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = s370slm_config
};

int
machine_at_s370slm_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x12, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x14, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440lx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977TF | W83977_AMI | W83977_NO_NVR));
    device_add(&intel_flash_bxt_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&w83781d_device);    /* fans: CPU, Fan 2, Chassis; temperatures: unused, CPU, unused */
    hwm_values.temperatures[0] = 0; /* unused */
    hwm_values.temperatures[2] = 0; /* unused */

    return ret;
}

/* i440BX */
static const device_config_t bm6_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "bm6",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v4.51PG - Revision MQ",
                .internal_name = "bm6_JN",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/bm6/BM6_MQ.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision MQ",
                .internal_name = "bm6_MQ",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/bm6/BM6_MQ.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision PQ",
                .internal_name = "bm6_PQ",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/bm6/BM6_PQ.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision QT",
                .internal_name = "bm6_QT",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/bm6/BM6_QT.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision TZ",
                .internal_name = "bm6",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/bm6/BM6_TZ.BIN", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t bm6_device = {
    .name          = "ABIT AB-BM6",
    .internal_name = "bm6",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = bm6_config
};

int
machine_at_bm6_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x11, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x13, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0xF, 256);

    return ret;
}

int
machine_at_awc661r_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/awc661r/c66ila1.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_inst_params(&w83977_device, 1, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add_inst_params(&w83977_device, 2, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x3, 256);

    return ret;
}

int
machine_at_awo671r_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/awo671r/a08139c.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_VIDEO,       2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_inst_params(&w83977_device, 1, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add_inst_params(&w83977_device, 2, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    if (gfxcard[0] == VID_INTERNAL)
        device_add(machine_get_vid_device(machine));
    spd_register(SPD_TYPE_SDRAM, 0x3, 256);

    return ret;
}

int
machine_at_ambx133_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/ambx133/mkbx2vg2.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&gl518sm_2d_device); /* fans: CPUFAN1, CPUFAN2; temperature: CPU */
    hwm_values.fans[1] += 500;
    hwm_values.temperatures[0] += 4;                         /* CPU offset */
    hwm_values.voltages[1] = RESISTOR_DIVIDER(12000, 10, 2); /* different 12V divider in BIOS (10K/2K?) */

    return ret;
}

static const device_config_t cubx_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "cubx",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1005",
                .internal_name = "cubx_1005",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cubx/cubx1005.awd", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1006",
                .internal_name = "cubx_1006",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cubx/cubx1006.awd", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1007",
                .internal_name = "cubx",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cubx/cubx1007.awd", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1008 Beta 004",
                .internal_name = "cubx_1008beta004",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cubx/1008cu.004", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t cubx_device = {
    .name          = "ASUS CUBX",
    .internal_name = "cubx",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = cubx_config
};

int
machine_at_cubx_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x04, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x07, PCI_CARD_IDE,         2, 3, 4, 1);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&ide_cmd648_ter_qua_onboard_device);
    device_add(ics9xxx_get(ICS9250_08));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0xF, 256);
    device_add(&as99127f_device); /* fans: Chassis, CPU, Power; temperatures: MB, JTPWR, CPU */

    return ret;
}

int
machine_at_cb61_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/cb61/2A69KD4F.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);

    return ret;
}

int
machine_at_prm27i_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/prm27i/27i106.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x11, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x12, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x13, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0xF, 256);

    return ret;
}

int
machine_at_ga6bx7_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/ga6bx7/6BX7.F5",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&intel_flash_bxt_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&w83782d_device); /* fans: Chassis, Power, CPU; temperatures: System, CPU, unused */
    hwm_values.temperatures[2] = 0; /* unused */

    return ret;
}

int
machine_at_ms6153_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/ms6153/W6153IMS.430",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x12, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&w83782d_device); /* fans: Chassis, Power, CPU; temperatures: System, CPU, unused */
    hwm_values.temperatures[2] = 0; /* unused */

    return ret;
}

int
machine_at_65eb_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/65eb/65eb-v2.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&intel_flash_bxt_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);

    return ret;
}

/* i440ZX */
int
machine_at_63a1_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/63a1/63a-q3.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4); /* Integrated Sound? */
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440zx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&intel_flash_bxt_device);
    spd_register(SPD_TYPE_SDRAM, 0x3, 256);

    return ret;
}

int
machine_at_sl65fplus_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sl65fplus/65fp-t3.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      2, 3, 4, 1); /* assumed */
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440zx_device);
    device_add(&piix4e_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&intel_flash_bxt_device);
    spd_register(SPD_TYPE_SDRAM, 0x3, 256);

    return ret;
}

/* SiS 600 */
int
machine_at_7sbb_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/7sbb/sbb12aa2.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1 | FLAG_TRC_CONTROLS_CPURST);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x01, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x11, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x02, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&sis_5600_device);
    device_add(&it8661f_device);
    device_add(&sst_flash_29ee020_device); /* assumed */

    return ret;
}

/* SMSC VictoryBX-66 */
int
machine_at_atc7020bxii_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/atc7020bxii/7020s102.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&slc90e66_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0xF, 256);

    return ret;
}

int
machine_at_m773_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/m773/010504s.rom",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0C, PCI_CARD_SOUND,       4, 3, 0, 0);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&i440bx_device);
    device_add(&slc90e66_device);
    device_add(&it8671f_device);
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x3, 256);
    device_add(&gl520sm_2d_device);  /* fans: CPU, Chassis; temperature: System */
    hwm_values.temperatures[0] += 2; /* System offset */
    hwm_values.temperatures[1] += 2; /* CPU offset */
    hwm_values.voltages[0] = 3300;   /* Vcore and 3.3V are swapped */
    hwm_values.voltages[2] = hwm_get_vcore();

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

/* VIA Apollo Pro */
int
machine_at_vim863s_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/vim863s/V0218SAG.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 0, 0);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x13, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x14, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    if ((sound_card_current[0] == SOUND_INTERNAL) && machine_get_snd_device(machine)->available())
        machine_snd = device_add(machine_get_snd_device(machine));

    device_add(&via_apro_device);
    device_add(&via_vt82c586b_device);
    device_add_params(&fdc37c669_device, (void *) 0);
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);

    return ret;
}

/* VIA Apollo Pro 133 */
int
machine_at_acorp693ap_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/acorp693ap/693APV11.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(ics9xxx_get(ICS9250_18));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    return ret;
}

static const device_config_t m6vch_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "m6vch",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 09/05/2000",
                .internal_name = "m6vch_2000",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vch/vch0905f.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 05/17/2001",
                .internal_name = "m6vch",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vch/vch0517b.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t m6vch_device = {
    .name          = "Biostar M6VCH",
    .internal_name = "m6vch",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = m6vch_config
};

int
machine_at_m6vch_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_SOUND,       3, 4, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t m6vci_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "m6vci",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 10/19/2000",
                .internal_name = "m6vci_10192000",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vci/VCI1019F.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 01/11/2001",
                .internal_name = "m6vci_01112001",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vci/VCI0111F.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 05/21/2001",
                .internal_name = "m6vci",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vci/VCI0521B.BIN", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t m6vci_device = {
    .name          = "Biostar M6VCI",
    .internal_name = "m6vci",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = m6vci_config
};

int
machine_at_m6vci_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t p6bap_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "p6bap",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 1.3 06/30/2000 (Kobian/Mercury KOB 693a FSX)",
                .internal_name = "kob693afsx",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6bap/bapa13k.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 1.4a 08/13/2001",
                .internal_name = "p6bap",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6bap/bapa14a.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 06/30/2000 (Matsonic MS-7117C)",
                .internal_name = "ms7117c",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6bap/042.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 10/30/2001 (Matsonic MS-7117C)",
                .internal_name = "ms7117c_autumn2001",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6bap/011030.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t p6bap_device = {
    .name          = "ECS P6BAP-A+",
    .internal_name = "p6bap",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = p6bap_config
};

int
machine_at_p6bap_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0a, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0b, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0c, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0d, PCI_CARD_SOUND,       4, 3, 2, 1); /* assumed */
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);  /* Rebranded as ET82C693A */
    device_add(&via_vt82c596b_device); /* Rebranded as ET82C696B */
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&gl520sm_2d_device);  /* fans: CPU, Chassis; temperature: System */
    hwm_values.temperatures[0] += 2; /* System offset */
    hwm_values.temperatures[1] += 2; /* CPU offset */
    hwm_values.voltages[0] = 3300;   /* Vcore and 3.3V are swapped */
    hwm_values.voltages[2] = hwm_get_vcore();

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_p6bapme_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/p6bapme/bapme14a.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0a, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0b, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0c, PCI_CARD_SOUND,       4, 3, 2, 1); /* assumed */
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);  /* Rebranded as ET82C693A */
    device_add(&via_vt82c596b_device); /* Rebranded as ET82C696B */
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&gl520sm_2d_device);  /* fans: CPU, Chassis; temperature: System */
    hwm_values.temperatures[0] += 2; /* System offset */
    hwm_values.temperatures[1] += 2; /* CPU offset */
    hwm_values.voltages[0] = 3300;   /* Vcore and 3.3V are swapped */
    hwm_values.voltages[2] = hwm_get_vcore();

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t p6vaa_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "p6vaa",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.0c 12/07/2000 (Matsonic MS-7127C)",
                .internal_name = "p6vaa",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vaa/pvaa11a.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.0f 06/15/2001 (Kobian/Mercury KOB 693a FCSX)",
                .internal_name = "kob693afcsx",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vaa/693a10f.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.1a 08/07/2001",
                .internal_name = "p6vaa",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vaa/pvaa11a.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t p6vaa_device = {
    .name          = "ECS P6VAA",
    .internal_name = "p6vaa",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = p6vaa_config
};

int
machine_at_p6vaa_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(&sst_flash_39sf020_device);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_p6vam_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/p6vam/vam10f.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(&sst_flash_39sf020_device);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t ms6153va_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "ms6153va",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 2.00 (LG IBM OEM)",
                .internal_name = "lgibmvt693a200",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6153va/ms6153.200", "" }
            },
            {
                .name          = "Award Modular BIOS v4.51PG - Revision 5.1",
                .internal_name = "ms6153va",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6153va/w6153vms.510", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t ms6153va_device = {
    .name          = "MSI MS-6153VA",
    .internal_name = "ms6153va",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = ms6153va_config
};

int
machine_at_ms6153va_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x12, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c596b_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&winbond_flash_w29c020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 512);
    device_add(&w83782d_device); /* fans: Chassis, Power, CPU; temperatures: System, CPU, unused */
    hwm_values.temperatures[2] = 0; /* unused */

    return ret;
}

int
machine_at_sl63avp_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sl63avp/63avp-e5.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c596b_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);

    return ret;
}

int
machine_at_sl65fvb_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sl65fvb/65fvb-n3.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 0, 0, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c596b_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);

    return ret;
}

int
machine_at_sy7vba133_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sy7vba133/7vba2aa7.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

/* VIA Apollo Pro 133A */
int
machine_at_6via90ap_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/6via90ap/90ap10.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(ics9xxx_get(ICS9250_18));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine)); /* ALC100P identified on similar Acorp boards (694TA, 6VIA90A1) */

    return ret;
}

static const device_config_t cuv4xcm_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "cuv4xcme",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1001 (ASUS CUV4X-CME)",
                .internal_name = "cuv4xcme",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xcm/cvcme101.awd", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1002 Beta 002 (ASUS CUV4X-CME)",
                .internal_name = "cuv4xcme_beta",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xcm/1002.002", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1004 (ASUS CUV4X-CM)",
                .internal_name = "cuv4xcm",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xcm/CUCM1004.AWD", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1004 (Medion MED 2001)",
                .internal_name = "med2001_1004",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xcm/1004MED.MED", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1006A (Medion MED 2001)",
                .internal_name = "med2001_1006a",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xcm/1006A.MED", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1009 (Medion MED 2001)",
                .internal_name = "med2001",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xcm/1009MED.MED", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t cuv4xcm_device = {
    .name          = "ASUS CUV4X-CM and CUV4X-CME",
    .internal_name = "cuv4xcm",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = cuv4xcm_config
};

int
machine_at_cuv4xcm_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x04, PCI_CARD_SOUTHBRIDGE, 4, 1, 2, 3);
    pci_register_slot(0x05, PCI_CARD_SOUND,       4, 0, 0, 0);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device);
    device_add(ics9xxx_get(ICS9250_18));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0xF, 1024);
    device_add(&as99127f_device); /* fans: Chassis, CPU, Power; temperatures: MB, JTPWR, CPU */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t cuv4xls_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "cuv4xls",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1004",
                .internal_name = "cuv4xls",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xls/1004ls.awd", "" }
            },
            {
                .name          = "Award Medallion BIOS v6.0 - Revision 1005 Beta 001",
                .internal_name = "cuv4xls_1005beta001",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/cuv4xls/1005ls.001", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t cuv4xls_device = {
    .name          = "ASUS CUV4X-LS",
    .internal_name = "cuv4xls",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = cuv4xls_config
};

int
machine_at_cuv4xls_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x04, PCI_CARD_SOUTHBRIDGE, 4, 1, 2, 3);
    pci_register_slot(0x05, PCI_CARD_SOUND,       3, 0, 0, 0);
    pci_register_slot(0x06, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x07, PCI_CARD_NORMAL,      2, 3, 0, 0);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x14, PCI_CARD_NORMAL,      4, 0, 0, 0);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device);
    device_add(ics9xxx_get(ICS9250_18));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0xF, 1024);
    device_add(&as99127f_device); /* fans: Chassis, CPU, Power; temperatures: MB, JTPWR, CPU */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t m6vcf_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "m6vcf",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 08/28/2000",
                .internal_name = "m6vcf_0828",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcf/vcf0828f.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 12/04/2000 (Fujitsu-Siemens OEM)",
                .internal_name = "m6vcf_fsc",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcf/vcf1204a.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 02/10/2001",
                .internal_name = "m6vcf_0210",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcf/vcf0210f.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 08/07/2001 (with logo)",
                .internal_name = "m6vcf",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcf/vcf0807b.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 08/07/2001 (without logo)",
                .internal_name = "m6vcf_nologo",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcf/vcf0807f.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t m6vcf_device = {
    .name          = "Biostar M6VCF",
    .internal_name = "m6vcf",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = m6vcf_config
};

int
machine_at_m6vcf_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_SOUND,       3, 4, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t m6vcg_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "m6vcg",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 01/10/2001 (Fujitsu-Siemens Euroline 850)",
                .internal_name = "m6vcg_fsc",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcg/vcg01103.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 03/13/2001",
                .internal_name = "m6vcg_0313",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcg/vcg0313f.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 05/15/2001 (Amptron Vega V6941)",
                .internal_name = "m6vcg_vegav6941",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcg/94v0515f.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 07/11/2001 (with logo)",
                .internal_name = "m6vcg",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcg/vcg0711b.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 07/11/2001 (without logo)",
                .internal_name = "m6vcg_nologo",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/m6vcg/vcg0711f.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t m6vcg_device = {
    .name          = "Biostar M6VCG",
    .internal_name = "m6vcg",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = m6vcg_config
};

int
machine_at_m6vcg_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_ca64en_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/ca64en/ca64end.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x14, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x11, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    return ret;
}

static const device_config_t mb694a_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "mb694a",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 2.0e",
                .internal_name = "mb694a",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/mb694a/mb20e.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 2.0b 02/13/2001 (Kobian/Mercury KOB 694X FSX)",
                .internal_name = "kob694xfsxfeb13",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/mb694a/kob20b.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 2.0 08/13/2001 (Kobian/Mercury KOB 694X FSX)",
                .internal_name = "kob694xfsx",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/mb694a/kob20.BIN", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t mb694a_device = {
    .name          = "ECS MB694A",
    .internal_name = "mb694a",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = mb694a_config
};

int
machine_at_mb694a_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    int is_kobfeb132001 = !strcmp(device_get_config_bios("bios"), "kob694xfsxfeb13");
    fn                  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);

    if (is_kobfeb132001)
        device_add(&via_vt82c686b_device);
    else
        device_add(&via_vt82c686a_device);

    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);

    if (is_kobfeb132001) {
        hwm_values.temperatures[0] += 2;
        hwm_values.temperatures[1] += 2;
        hwm_values.temperatures[2] = 0;
    } else {
        device_add(&via_vt82c686_hwm_device);
        hwm_values.temperatures[0] += 2;
        hwm_values.temperatures[1] += 2;
        hwm_values.temperatures[2] = 0;
    }

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t p6vap_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "p6vap",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.5b 08/13/2001",
                .internal_name = "p6vap",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vap/vapa15b.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 06/01/2000 (Matsonic MS-7157C)",
                .internal_name = "ms7157c",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vap/71570601.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 12/28/2000 (Matsonic MS-7157C)",
                .internal_name = "ms7157c_dec2000",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vap/71571228.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t p6vap_device = {
    .name          = "ECS P6VAP-A+",
    .internal_name = "p6vap",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = p6vap_config
};

int
machine_at_p6vap_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_SOUND,       4, 3, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c596b_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    device_add(&gl520sm_2d_device);  /* fans: CPU, Chassis; temperature: System */
    hwm_values.temperatures[0] += 2; /* System offset */
    hwm_values.temperatures[1] += 2; /* CPU offset */
    hwm_values.voltages[0] = 3300;   /* Vcore and 3.3V are swapped */
    hwm_values.voltages[2] = hwm_get_vcore();

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_p6vapme_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/p6vapme/vapme15d.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0D, PCI_CARD_SOUND,       4, 3, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c596b_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    device_add(&gl520sm_2d_device);  /* fans: CPU, Chassis; temperature: System */
    hwm_values.temperatures[0] += 2; /* System offset */
    hwm_values.temperatures[1] += 2; /* CPU offset */
    hwm_values.voltages[0] = 3300;   /* Vcore and 3.3V are swapped */
    hwm_values.voltages[2] = hwm_get_vcore();

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t p6vxa_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "p6vxa",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 2.0 11/09/2000 (Matsonic MS-7177C)",
                .internal_name = "ms7177c",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vxa/msvxa21.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 2.1 06/15/2001",
                .internal_name = "p6vxa",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/p6vxa/vxa21.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t p6vxa_device = {
    .name          = "ECS P6VXA",
    .internal_name = "p6vxa",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = p6vxa_config
};

int
machine_at_p6vxa_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_p6vxm_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/p6vxm/vxm21a.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t j694as_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "j694as",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PGN - Revision A06",
                .internal_name = "j694as_a06",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/j694as/694ASA06.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision A13",
                .internal_name = "j694as_a13",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/j694as/694ASA13.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision A15",
                .internal_name = "j694as",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/j694as/694ASA15.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision B13 (Bona/Lex BN694)",
                .internal_name = "bn694",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/j694as/BN694B13.BIN", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t j694as_device = {
    .name          = "Jetway J-694AS",
    .internal_name = "j694as",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = j694as_config
};

int
machine_at_j694as_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(ics9xxx_get(ICS9250_18)); /* assumed */
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t j694cs_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "j694cs",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PGN - Revision A06",
                .internal_name = "j694cs_a06",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/j694cs/694CSA06.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision A09",
                .internal_name = "j694cs_a09",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/j694cs/694CSA09.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision A11",
                .internal_name = "j694cs",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/j694cs/694CSA11.BIN", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t j694cs_device = {
    .name          = "Jetway J-694CS",
    .internal_name = "j694cs",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = j694cs_config
};

int
machine_at_j694cs_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(ics9xxx_get(ICS9250_18)); /* assumed */
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t jetwayva4_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "jetwayva4",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PGN - Revision A02",
                .internal_name = "jetwayva4_a02",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/jetwayva4/VA4A02.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PGN - Revision A03",
                .internal_name = "jetwayva4_a03",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/jetwayva4/VA4A03.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision A09",
                .internal_name = "jetwayva4",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/jetwayva4/VA4A09.BIN", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t jetwayva4_device = {
    .name          = "Jetway VA4",
    .internal_name = "jetwayva4",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = jetwayva4_config
};

int
machine_at_jetwayva4_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(ics9xxx_get(ICS9250_18)); /* assumed */
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t jetwayvm4_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "jetwayvm4",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PGN - Revision A02",
                .internal_name = "jetwayvm4_a02",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/jetwayvm4/VM4A02.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PGN - Revision A03",
                .internal_name = "jetwayvm4_a03",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/jetwayvm4/VM4A03.BIN", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision A09",
                .internal_name = "jetwayvm4",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/jetwayvm4/VM4A09.BIN", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t jetwayvm4_device = {
    .name          = "Jetway VM4",
    .internal_name = "jetwayvm4",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = jetwayvm4_config
};

int
machine_at_jetwayvm4_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(ics9xxx_get(ICS9250_18)); /* assumed */
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

static const device_config_t ls694xpro_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "ls694xpro",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PGN - Revision 07",
                .internal_name = "ls694xlite",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ls694xpro/694pro07.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision B1",
                .internal_name = "ls694xpro",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ls694xpro/94prob1.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t ls694xpro_device = {
    .name          = "Lucky Star 694X PRO",
    .internal_name = "ls694xpro",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = ls694xpro_config
};

int
machine_at_ls694xpro_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(ics9xxx_get(ICS9250_18)); /* assumed */
    device_add(&sst_flash_39sf020_device);
    device_add(&via_vt82c686_hwm_device); /* fans: CPU1, Chassis; temperatures: CPU, System, unused */
    spd_register(SPD_TYPE_SDRAM, 0x7, 256);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    return ret;
}

static const device_config_t ms6318_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "ms6318",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.1",
                .internal_name = "ms6318_110",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6318/w6318vms.110", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.2",
                .internal_name = "ms6318",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6318/w6318vms.120", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.3 (Medion MED 2000)",
                .internal_name = "ms6318_130",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6318/ms6318.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.8 (HP Pavilion A7xx)",
                .internal_name = "ms6318_180",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6318/med2000v2.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.9 (HP Pavilion A8xx)",
                .internal_name = "ms6318_190",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6318/med2000.bin", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 2.02 (HP Medion 2000A)",
                .internal_name = "ms6318_202",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6318/ms6318hp.bin", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t ms6318_device = {
    .name          = "MSI MS-6318",
    .internal_name = "ms6318",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = ms6318_config
};

int
machine_at_ms6318_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0C, PCI_CARD_SOUND,       3, 4, 1, 2);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));
    device_add(&vt1611a_device); /* for both VIA and CT5880 options */

    return ret;
}

static const device_config_t ms6323_config[] = {
    // clang-format off
    {
        .name           = "bios",
        .description    = "BIOS Version",
        .type           = CONFIG_BIOS,
        .default_string = "ms6323",
        .default_int    = 0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = { { 0 } },
        .bios           = {
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.0",
                .internal_name = "ms6323_100",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6323/w6323vms.100", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.1",
                .internal_name = "ms6323",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6323/w6323vms.110", "" }
            },
            {
                .name          = "Award Modular BIOS v6.00PG - Revision 1.5 (Fujitsu-Siemens OEM)",
                .internal_name = "ms6323_150",
                .bios_type     = BIOS_NORMAL,
                .files_no      = 1,
                .local         = 0,
                .size          = 262144,
                .files         = { "roms/machines/ms6323/W6323VF1.150", "" }
            },
            { .files_no = 0 }
        }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

const device_t ms6323_device = {
    .name          = "MSI MS-6323",
    .internal_name = "ms6323",
    .flags         = 0,
    .local         = 0,
    .init          = NULL,
    .close         = NULL,
    .reset         = NULL,
    .available     = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = ms6323_config
};

int
machine_at_ms6323_init(const machine_t *model)
{
    int         ret = 0;
    const char *fn;

    /* No ROMs available */
    if (!device_available(model->device))
        return ret;

    device_context(model->device);
    fn  = device_get_bios_file(machine_get_device(machine), device_get_config_bios("bios"), 0);
    ret = bios_load_linear(fn, 0x000c0000, 262144, 0);
    device_context_restore();

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0C, PCI_CARD_SOUND,       3, 4, 1, 2);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));
    device_add(&vt1611a_device); /* for both VIA and CT5880 options */

    return ret;
}

int
machine_at_p257_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/p257/V1222AG.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x13, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x12, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x08, PCI_CARD_SOUND,       4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_p6v694xa10b_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/p6v694xa10b/A10BV46.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(ics9xxx_get(ICS9250_18));
    device_add(&winbond_flash_w29c020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_p6v694xa10m_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/p6v694xa10m/A10MV24.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0D, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(ics9xxx_get(ICS9250_18));
    device_add(&winbond_flash_w29c020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    device_add(&via_vt82c686_hwm_device);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_cairo5_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/cairo5/08BV.BIN",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x0E, PCI_CARD_NORMAL,      1, 2, 3, 0);
    pci_register_slot(0x0F, PCI_CARD_NORMAL,      2, 3, 0, 1);
    pci_register_slot(0x10, PCI_CARD_NORMAL,      3, 0, 1, 2);
    pci_register_slot(0x11, PCI_CARD_NORMAL,      0, 1, 2, 3);
    pci_register_slot(0x12, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device);
    device_add(&winbond_flash_w29c020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);

    return ret;
}

int
machine_at_sl65jvb_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sl65jvb/65jvb-p1.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c596b_device);
    device_add_params(&w83977_device, (void *) (W83977EF | W83977_AMI | W83977_NO_NVR));
    device_add(&sst_flash_39sf020_device); /* assumed */
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);

    return ret;
}

int
machine_at_sl65kv2_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/sl65kv2/65kv-f82.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686b_device); /* fans: CPU1, CPU2; temperatures: CPU, System, unused */
    device_add(&sst_flash_39sf020_device);
    spd_register(SPD_TYPE_SDRAM, 0x7, 1024);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}

int
machine_at_zta694x_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear("roms/machines/zta694x/a694112e.bin",
                           0x000c0000, 262144, 0);

    if (bios_only || !ret)
        return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x07, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x08, PCI_CARD_NORMAL,      1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL,      2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL,      3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x0C, PCI_CARD_NORMAL,      4, 1, 2, 3);
    pci_register_slot(0x01, PCI_CARD_AGPBRIDGE,   1, 2, 3, 4);

    device_add(&via_apro133a_device);
    device_add(&via_vt82c686a_device);
    device_add(ics9xxx_get(ICS9250_18)); /* assumed */
    device_add(&sst_flash_39sf020_device);
    device_add(&via_vt82c686_hwm_device);
    hwm_values.temperatures[0] += 2; /* CPU offset */
    hwm_values.temperatures[1] += 2; /* System offset */
    hwm_values.temperatures[2] = 0;  /* unused */

    if (sound_card_current[0] == SOUND_INTERNAL)
        device_add(machine_get_snd_device(machine));

    return ret;
}
