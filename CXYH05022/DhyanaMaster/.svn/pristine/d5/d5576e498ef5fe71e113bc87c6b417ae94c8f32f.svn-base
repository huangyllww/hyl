# ****************************************************************************
# *
# *
# * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
# *
# * HYGON is granting you permission to use this software (the Materials)
# * pursuant to the terms and conditions of your Software License Agreement
# * with HYGON.  This header does *NOT* give you permission to use the Materials
# * or any rights under HYGON's intellectual property.  Your use of any portion
# * of these Materials shall constitute your acceptance of those terms and
# * conditions.  If you do not agree to the terms and conditions of the Software
# * License Agreement, please do not use any portion of these Materials.
# *
# * CONFIDENTIALITY:  The Materials and all other information, identified as
# * confidential and provided to you by HYGON shall be kept confidential in
# * accordance with the terms and conditions of the Software License Agreement.
# *
# * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
# * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
# * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
# * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
# * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
# * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
# * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
# * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
# * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
# * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
# * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
# * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
# * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
# *
# * HYGON does not assume any responsibility for any errors which may appear in
# * the Materials or any other related information provided to you by HYGON, or
# * result from use of the Materials or any related information.
# *
# * You agree that you will not reverse engineer or decompile the Materials.
# *
# * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
# * further information, software, technical information, know-how, or show-how
# * available to you.  Additionally, HYGON retains the right to modify the
# * Materials at any time, without notice, and is not obligated to provide such
# * modified Materials to you..
# ******************************************************************************

import struct
import configparser
import os
import stat
import xml.etree.ElementTree as ET
import argparse
import shutil
import sys

version = '3.1'

apcb_type = [0x60, 0x68]
configurations = None
gAlignSize = 0x100
soc_ids = ['0xBC090200', '0xBC090201', '0xBC090300']
modifiable_fw = [
    0x04,  # 'PSP' ,'IMAGE_ENTRY','PspNv','PSP Non Volatile data'
    0x22,  # 'PSP' ,'IMAGE_ENTRY','PspTokenUnlockData', 'PSP token unlock data'
	0x49,  # 'PSP' ,'IMAGE_ENTRY','PspSecureFW', 'Psp Secure Firmware'
	0x5e,  # 'PSP' ,'IMAGE_ENTRY','TpcmFw', 'Tpcm Firmware'
    0x60,  # 'BIOS','IMAGE_ENTRY','Apcb','Agesa PSP Customization Block'
    0x63,  # 'BIOS','IMAGE_ENTRY','ApobNv','APOB NV Copy'
    0x67,  # 'BIOS','IMAGE_ENTRY','CoreMceData','core machine check exception data'
    0x68,  # 'BIOS','IMAGE_ENTRY','Apcb','Agesa PSP Customization Block'
]

extend_fw = None
tpcm_file_list = {'PspSecureFw.bin': '0x49', 'PspTpcmFw.bin': '0x5e'}
csv_file_list = {'PspSecureFw.bin': '0x49'}
ftpm_file_list = {'PspSecureFw.bin': '0x49'}
socx_shared_fw = None
socx_shared_file_list = {'PspSecureFwData.bin': '0x38'}

gPspLayout = {
    'combo_dir_base': 0,
    'soc1_psp_dir_level_1_base': 0,
    'soc1_psp_dir_level_1_size': 0,
    'soc1_bios_dir_level_1_base': 0,
    'soc1_bios_dir_level_1_size': 0,
    'soc2_psp_dir_level_1_base': 0,
    'soc2_psp_dir_level_1_size': 0,
    'soc2_bios_dir_level_1_base': 0,
    'soc2_bios_dir_level_1_size': 0,
    'soc3_psp_dir_level_1_base': 0,
    'soc3_psp_dir_level_1_size': 0,
    'soc3_bios_dir_level_1_base': 0,
    'soc3_bios_dir_level_1_size': 0,
    'soc1_psp_dir_level_2_base': 0,
    'soc1_psp_dir_level_2_size': 0,
    'soc1_bios_dir_level_2_base': 0,
    'soc1_bios_dir_level_2_size': 0,
    'soc2_psp_dir_level_2_base': 0,
    'soc2_psp_dir_level_2_size': 0,
    'soc2_bios_dir_level_2_base': 0,
    'soc2_bios_dir_level_2_size': 0,
    'soc3_psp_dir_level_2_base': 0,
    'soc3_psp_dir_level_2_size': 0,
    'soc3_bios_dir_level_2_base': 0,
    'soc3_bios_dir_level_2_size': 0,
    'socx_shared_base': 0,
    'socx_shared_size': 0,
    'fv_bb_size': 0,
    'fv_main_base': 0,
    'fv_main_size': 0
}


def aligned_size(org_size, alignment):
    """
    align file with given alignment size
    :param org_size: original file size
    :param alignment: size for align
    :return:aligned size
    """
    return org_size + (((alignment) - ((org_size) & ((alignment) - 1))) & ((alignment) - 1))


def dirs_size(xml_file):
    """
    scan psp layout xml file to get psp directory size of level1 and level2, bios directory level1 and level2
    :param xml_file:psp layout xml file
    :return: [psp_leve1_size, psp_level2_size, bios_level1_size, bios_level2_size]
    """
    psp_dirs_size = [0x1000, 0x1000]
    bios_dirs_size = [0x1000, 0x1000]
    root = ET.parse(xml_file).getroot()
    for psp in root.iter('PSP_DIR'):
        spi_block_size = int(psp.attrib['SpiBlockSize'], base=16)
        psp_level = int(psp.attrib['Level'], base=16)

        for image_entry in psp.findall('IMAGE_ENTRY'):
            file_name = image_entry.attrib['File']
            if file_exists(file_name):
                if 'Size' in image_entry.attrib.keys():
                    # use the image size defined in xml 
                    # align with spi_block_size
                    psp_dirs_size[psp_level - 1] = aligned_size(psp_dirs_size[psp_level - 1], spi_block_size)
                    psp_dirs_size[psp_level -1] += aligned_size(int(image_entry.attrib['Size'], base=16), spi_block_size)
                else:
                    if int(image_entry.attrib['Type'], base=16) in modifiable_fw:
                        psp_dirs_size[psp_level - 1] = aligned_size(psp_dirs_size[psp_level - 1], spi_block_size)
                        psp_dirs_size[psp_level - 1] += aligned_size(os.path.getsize(file_name),
                                                                    spi_block_size)  # align in spi block size
                    else:
                        psp_dirs_size[psp_level - 1] += aligned_size(os.path.getsize(file_name),
                                                                    gAlignSize)  # align in global gAlignSize
            else:
                sys.exit('{} not exists, please check'.format(image_entry.attrib['File']))
        psp_dirs_size[psp_level - 1] = aligned_size(psp_dirs_size[psp_level - 1], spi_block_size)


    for bios in root.iter('BIOS_DIR'):
        spi_block_size = int(psp.attrib['SpiBlockSize'], base=16)
        bios_level = int(bios.attrib['Level'], base=16)

        for image_entry in bios.findall('IMAGE_ENTRY'):
            file_name = image_entry.attrib['File']
            file_type = int(image_entry.attrib['Type'], base=16)
            if file_exists(file_name) or (file_type in apcb_type):
                if 'Size' in image_entry.attrib.keys():
                    # use the image size defined in xml
                    # align with spi_block_size
                    bios_dirs_size[bios_level - 1] = aligned_size(bios_dirs_size[bios_level - 1], spi_block_size)
                    bios_dirs_size[bios_level - 1] += aligned_size(int(image_entry.attrib['Size'], base=16), spi_block_size)
                else:
                    if int(image_entry.attrib['Type'], base=16) in modifiable_fw:
                        bios_dirs_size[bios_level - 1] = aligned_size(bios_dirs_size[bios_level - 1], spi_block_size)
                        bios_dirs_size[bios_level - 1] += aligned_size(os.path.getsize(file_name),
                                                                       spi_block_size)  # align in spi block size
                    else:
                        bios_dirs_size[bios_level - 1] += aligned_size(os.path.getsize(file_name),
                                                                       gAlignSize)  # align in global gAlignSize
            else:
                sys.exit('{} not exists, please check'.format(image_entry.attrib['File']))
        bios_dirs_size[bios_level - 1] = aligned_size(bios_dirs_size[bios_level - 1], spi_block_size)
    return psp_dirs_size + bios_dirs_size


def file_exists(file):
    """
    check whether file exists
    :param file: file path and name
    :return:True - file exists, False = not exist
    """
    return os.path.exists(file)


def get_configurations(config_file):
    """
    get psp and bios layout from configuration file
    :param config_file: configuration file
    :return: psp layout info
    """
    config = configparser.RawConfigParser()
    config.read(config_file)
    return config


def parse_arg():
    """
    get input parameters
    :return: arguments
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', help='configuration file', type=str, required=True)
    parser.add_argument('--securemode', help='es: eng-sample cpu, mp:security cpu', type=str, choices=['es', 'mp'],
                        required=True)
    parser.add_argument('--tpcm', help='tpcm supported', action='store_true')
    parser.add_argument('--ftpm', help='ftpm supported', action='store_true')
    parser.add_argument('--csv', help='csv supported', action='store_true')
    parser.add_argument('--version', action='version', version='PspLayoutEditorV3.py v%s' % (version))
    args = parser.parse_args()
    return args


def generate_fch_firmware(args):
    """
    Generate fch firmware binary. This binary locates at offset 0x20000 in bios.
    firmware_val[0] : OEM_SIG  0x55AA55AA
    firmware_val[3] : XHCI firmware base
    firmware_val[5] : PSP Directory pointer or combo pointer
    firmware_val[6] : 0xffffffff
    firmware_val[7] : BIOS directory pointer of SOC0
    firmware_val[8] : BIOS directory pointer of SOC1
    firmware_val[9] : BIOS directory pointer of SOC2
    others          : not defined
    """
    fch_binary_fmt = 'I' * 18
    s = struct.Struct(fch_binary_fmt)
    firmware_val = [
        0x55aa55aa,                                 # offset-0 OEM_SIG is short for oem signature
        0,                                          # None
        0,                                          # None
        0xff021000,                                 # offset-3 xhci firmware base
        0,                                          # offset-4
        gPspLayout['combo_dir_base'],               # offset-5 combo header pointer
        0xffffffff,                                 # offset-6 None
        gPspLayout['soc1_bios_dir_level_1_base'],   # offset-7 SOC1 bios level1 dir pointer
        gPspLayout['soc2_bios_dir_level_1_base'],   # offset-8 SOC2 bios level1 dir pointer
        gPspLayout['soc3_bios_dir_level_1_base'],   # offset-9 SOC3 bios level1 dir pointer
        0xffffffff,                                 # offset-10 None
        0xffffffff,                                 # offset-11 None
        0xffffffff,                                 # offset-12 None
        0xffffffff,                                 # offset-13 None
        0xffffffff,                                 # offset-14 None
        0xffffffff,                                 # offset-15 None
        0xffffffff,                                 # offset-16 spi mode
        0xffffffff                                  # offset-17 spi speed
    ]
        
    print('generate fch binary:')
    for i in range(len(firmware_val)):
        if 5<= i <= 9:
            firmware_val[i] |= 0xFF000000
        print('{:02d} offset {:02d}: |0x{:08x}|'.format(i, (i * 4), firmware_val[i]))
        print(' ' * 14 + '-' * 12)
    fch_firmware_bin = configurations.get('file_path', 'fch_firmware_bin')
    if os.path.exists(fch_firmware_bin):
        os.chmod(fch_firmware_bin, stat.S_IRWXU)
        os.remove(fch_firmware_bin)
    with open(fch_firmware_bin, 'wb') as fd:
        tmp = s.pack(*firmware_val)
        fd.write(tmp)


def update_layout(soc_index):
    """
    update psp layout xml file with given base and size data
    :param soc_index:
    :return:None
    """
    psp_data_output = configurations.get('file_path', 'psp_data_output_soc{}'.format(soc_index))
    psp_dir1_base = gPspLayout['soc{}_psp_dir_level_1_base'.format(soc_index)]
    psp_dir1_size = gPspLayout['soc{}_psp_dir_level_1_size'.format(soc_index)]
    psp_dir2_base = gPspLayout['soc{}_psp_dir_level_2_base'.format(soc_index)]
    psp_dir2_size = gPspLayout['soc{}_psp_dir_level_2_size'.format(soc_index)]
    bios_dir1_base = gPspLayout['soc{}_bios_dir_level_1_base'.format(soc_index)]
    bios_dir1_size = gPspLayout['soc{}_bios_dir_level_1_size'.format(soc_index)]
    bios_dir2_base = gPspLayout['soc{}_bios_dir_level_2_base'.format(soc_index)]
    bios_dir2_size = gPspLayout['soc{}_bios_dir_level_2_size'.format(soc_index)]

    try:
        # update directory address in xml files
        os.chmod(psp_data_output, stat.S_IRWXU)
        tree = ET.parse(psp_data_output)
        root = tree.getroot()
        for psp_dir in root.iter('PSP_DIR'):
            # update psp directory level 1 and level 2 base address in xml file
            if psp_dir.attrib['Level'] == '1':
                psp_dir.set('Base', hex(psp_dir1_base).rstrip('L'))
                psp_dir.set('Size', hex(psp_dir1_size).rstrip('L'))

                for psp_level_2_point_entry in psp_dir.findall('POINT_ENTRY'):
                    if psp_level_2_point_entry.attrib['Type'] == "0x40":
                        psp_level_2_point_entry.set('Address', hex(psp_dir2_base).rstrip('L'))
                        pub_key_size = psp_level_2_point_entry.attrib['Size']

                for psp_level_2_point_entry in psp_dir.findall('POINT_ENTRY'):
                    if psp_level_2_point_entry.attrib['Type'] == "0x1":
                        psp_level_2_point_entry.set('Address',
                                                    hex(psp_dir2_base + int(pub_key_size, base=16)).rstrip('L'))
            elif psp_dir.attrib['Level'] == '2':
                psp_dir.set('Base', hex(psp_dir2_base).rstrip('L'))
                psp_dir.set('Size', hex(psp_dir2_size).rstrip('L'))
            else:
                pass

        for bios_dir in root.iter('BIOS_DIR'):
            if bios_dir.attrib['Level'] == '1':
                bios_dir.set('Base', hex(bios_dir1_base).rstrip('L'))
                bios_dir.set('Size', hex(bios_dir1_size).rstrip('L'))
                for point_entry in bios_dir.findall('POINT_ENTRY'):
                    if point_entry.attrib['Type'] == '0x70':  # update bios level 2 directory
                        point_entry.set('Address', hex(bios_dir2_base).rstrip('L'))
            elif bios_dir.attrib['Level'] == '2':
                bios_dir.set('Base', hex(bios_dir2_base).rstrip('L'))
                bios_dir.set('Size', hex(bios_dir2_size).rstrip('L'))
            else:
                pass
        # update to file
        tree.write(psp_data_output)
    except:
        raise Exception('Failed to update psp data layout xml files')


def update_combo_template():
    """
    """
    psp_data_template_combo = configurations.get('file_path', 'psp_data_template_combo')
    psp_data_output_combo = configurations.get('file_path', 'psp_data_output_combo')
    if os.path.exists(psp_data_output_combo):
        os.chmod(psp_data_output_combo, stat.S_IRWXU)
        os.remove(psp_data_output_combo)
    shutil.copy(psp_data_template_combo, psp_data_output_combo)
    print('update: {}'.format(psp_data_output_combo))
    try:
        # update combo xml
        os.chmod(psp_data_output_combo, stat.S_IRWXU)
        tree = ET.parse(psp_data_output_combo)
        root = tree.getroot()
        combo_dir = root.find('COMBO_DIR')
        combo_dir.set('Base', hex(gPspLayout['combo_dir_base']).rstrip('L'))  # set combo directory base address

        # three entries
        combo_entries = root.findall('COMBO_DIR/COMBO_ENTRY')
        for i, val in enumerate(combo_entries):
            # translate to memory address
            combo_entries[2 - i].set('Address', hex(gPspLayout['soc{}_psp_dir_level_1_base'.format(i + 1)] | 0xFF000000).rstrip('L'))
            combo_entries[2 - i].set('Id', soc_ids[i])

        tree.write(psp_data_output_combo)
    except:
        raise Exception('Failed to udpate combo xml template')


def update_psp_data_template():
    """
    update psp layout
    :return:None
    """
    for soc in range(1, 4):
        update_layout(soc)


def get_whole_layout():
    """
    get configurations from psplayout.cfg, and initialize all directory info for soc0,
    soc1 and soc2
    :return: None
    """
    global gPspLayout
    soc1_dirs = dirs_size(configurations.get('file_path', 'psp_data_output_soc1'))
    print('soc 1 fw size [psp-L1, psp-L2, bios-L1, bios-L2]: {}'.format(soc1_dirs))
    soc2_dirs = dirs_size(configurations.get('file_path', 'psp_data_output_soc2'))
    print('soc 2 fw size [psp-L1, psp-L2, bios-L1, bios-L2]: {}'.format(soc2_dirs))
    soc3_dirs = dirs_size(configurations.get('file_path', 'psp_data_output_soc3'))
    print('soc 3 fw size [psp-L1, psp-L2, bios-L1, bios-L2]: {}'.format(soc3_dirs))
    print('total size: 0x{:x}'.format(sum(soc1_dirs + soc2_dirs + soc3_dirs)))

    ######################
    # SOC0  PSP LEVEL 1  #
    ######################
    # SOC0 BIOS LEVEL 1  #
    ######################
    # SOC1 PSP  LEVEL 1  #
    ######################
    # SOC1 BIOS LEVEL 1  #
    ######################
    # SOC2 PSP  LEVEL 1  #
    ######################
    # SOC2 BIOS LEVEL 1  #
    ######################
    # SOC0  PSP LEVEL 2  #
    ######################
    # SOC0 BIOS LEVEL 2  #
    ######################
    # SOC1 PSP  LEVEL 2  #
    ######################
    # SOC1 BIOS LEVEL 2  #
    ######################
    # SOC2 PSP  LEVEL 2  #
    ######################
    # SOC2 BIOS LEVEL 2  #
    ######################
    gPspLayout['combo_dir_base'] = int(configurations.get('multi_soc', 'combo_dir_base'), base=16)
    gPspLayout['soc1_psp_dir_level_1_size'] = soc1_dirs[0]
    gPspLayout['soc1_psp_dir_level_2_size'] = soc1_dirs[1]
    gPspLayout['soc1_bios_dir_level_1_size'] = soc1_dirs[2]
    gPspLayout['soc1_bios_dir_level_2_size'] = soc1_dirs[3]
    gPspLayout['soc2_psp_dir_level_1_size'] = soc2_dirs[0]
    gPspLayout['soc2_psp_dir_level_2_size'] = soc2_dirs[1]
    gPspLayout['soc2_bios_dir_level_1_size'] = soc2_dirs[2]
    gPspLayout['soc2_bios_dir_level_2_size'] = soc2_dirs[3]
    gPspLayout['soc3_psp_dir_level_1_size'] = soc3_dirs[0]
    gPspLayout['soc3_psp_dir_level_2_size'] = soc3_dirs[1]
    gPspLayout['soc3_bios_dir_level_1_size'] = soc3_dirs[2]
    gPspLayout['soc3_bios_dir_level_2_size'] = soc3_dirs[3]
    gPspLayout['soc1_psp_dir_level_1_base'] = gPspLayout['combo_dir_base'] + 0x1000
    gPspLayout['soc1_bios_dir_level_1_base'] = gPspLayout['soc1_psp_dir_level_1_base'] + gPspLayout[
        'soc1_psp_dir_level_1_size']
    gPspLayout['soc2_psp_dir_level_1_base'] = gPspLayout['soc1_bios_dir_level_1_base'] + gPspLayout[
        'soc1_bios_dir_level_1_size']
    gPspLayout['soc2_bios_dir_level_1_base'] = gPspLayout['soc2_psp_dir_level_1_base'] + gPspLayout[
        'soc2_psp_dir_level_1_size']
    gPspLayout['soc3_psp_dir_level_1_base'] = gPspLayout['soc2_bios_dir_level_1_base'] + gPspLayout[
        'soc2_bios_dir_level_1_size']
    gPspLayout['soc3_bios_dir_level_1_base'] = gPspLayout['soc3_psp_dir_level_1_base'] + gPspLayout[
        'soc3_psp_dir_level_1_size']
    gPspLayout['soc1_psp_dir_level_2_base'] = gPspLayout['soc3_bios_dir_level_1_base'] + gPspLayout[
        'soc3_bios_dir_level_1_size']
    gPspLayout['soc1_bios_dir_level_2_base'] = gPspLayout['soc1_psp_dir_level_2_base'] + gPspLayout[
        'soc1_psp_dir_level_2_size']
    gPspLayout['soc2_psp_dir_level_2_base'] = gPspLayout['soc1_bios_dir_level_2_base'] + gPspLayout[
        'soc1_bios_dir_level_2_size']
    gPspLayout['soc2_bios_dir_level_2_base'] = gPspLayout['soc2_psp_dir_level_2_base'] + gPspLayout[
        'soc2_psp_dir_level_2_size']
    gPspLayout['soc3_psp_dir_level_2_base'] = gPspLayout['soc2_bios_dir_level_2_base'] + gPspLayout[
        'soc2_bios_dir_level_2_size']
    gPspLayout['soc3_bios_dir_level_2_base'] = gPspLayout['soc3_psp_dir_level_2_base'] + gPspLayout[
        'soc3_psp_dir_level_2_size']
    print(gPspLayout)


def pre_update_psp_data_template(args):
    """
    pre-process xml by given input parameters
    :param args:
    :return:
    """
    global extend_fw
    bios_fv_bb_entry_type = '0x62'
    bios_fv_main_entry_type = '0x71'
    
    if args.tpcm:
        extend_fw = tpcm_file_list
        socx_shared_fw = socx_shared_file_list
    elif args.csv or args.ftpm:
        extend_fw = ftpm_file_list
        socx_shared_fw = socx_shared_file_list
    else:
        extend_fw = None
        socx_shared_fw = None
    # build fv_main entry node
    fv_main_attrib = dict()
    fv_main_attrib['Type'] = bios_fv_main_entry_type
    fv_main_attrib['Address'] = configurations.get('multi_soc', 'fv_main_base')
    fv_main_attrib['Size'] = configurations.get('multi_soc', 'fv_main_size')
    fv_main_node = ET.Element('POINT_ENTRY', fv_main_attrib)
    fv_main_node.tail = '\n' + ' ' * 4

    fv_bb_addr = configurations.get('multi_soc', 'fv_bb_addr')
    fv_bb_dest = configurations.get('multi_soc', 'fv_bb_dest')
    fv_bb_size = configurations.get('multi_soc', 'fv_bb_size')

    for soc in range(1, 4):
        xml_input = configurations.get('file_path', 'psp_data_soc{}'.format(soc))
        xml_output = configurations.get('file_path', 'psp_data_output_soc{}'.format(soc))
        if file_exists(xml_output):
            os.chmod(xml_output, stat.S_IRWXU)
            os.remove(xml_output)
        shutil.copy(xml_input, xml_output)

        try:
            # update directory address in xml files
            os.chmod(xml_output, stat.S_IRWXU)
            tree = ET.parse(xml_output)
            root = tree.getroot()

            # get firmware path in current xml
            fw_path = ''
            for image_entry in root.iter('IMAGE_ENTRY'):
                if 'AgesaModulePkg' in image_entry.attrib['File']:
                    fw_path = image_entry.attrib['File']
                    break
            print(fw_path)
            fw_path = '\\'.join(fw_path.split('\\')[:-1])
            for psp_dir in root.iter('PSP_DIR'):
                if psp_dir.attrib['Level'] == '1':
                    if extend_fw is not None:
                        for (file_name, file_type) in extend_fw.items():
                            extend_fw_attrib = dict()
                            extend_fw_attrib['File'] = fw_path + '\\' + file_name
                            extend_fw_attrib['Type'] = file_type
                            ext_node = ET.Element('IMAGE_ENTRY', extend_fw_attrib)
                            ext_node.tail = '\n' + ' ' * 4
                            psp_dir.insert(-1, ext_node)
                    if socx_shared_fw is not None:
                        for (file_name, file_type) in socx_shared_fw.items():
                            socx_shared_fw_attrib = dict()
                            socx_shared_fw_attrib['Address'] = configurations.get('multi_soc', 'socx_shared_base')
                            socx_shared_fw_attrib['Size'] = configurations.get('multi_soc', 'socx_shared_size')
                            fw_path=fw_path.replace('FW_SECURE_MODE', 'DNMcm_' + args.securemode.upper())
                            fw_path=fw_path.replace('SOC{}'.format(soc), 'ZP')   #fixme: SOCX_SHARED
                            socx_shared_fw_attrib['File'] = fw_path + '\\' + file_name
                            socx_shared_fw_attrib['Type'] = file_type
                            socx_shared_node = ET.Element('POINT_ENTRY', socx_shared_fw_attrib)
                            socx_shared_node.tail = '\n' + ' ' * 4
                            psp_dir.insert(-1, socx_shared_node)
                else:
                    pass
            for bios_dir in root.iter('BIOS_DIR'):
                # update fv_bb
                for point_entry in bios_dir.findall('POINT_ENTRY'):
                    if point_entry.attrib['Type'] == bios_fv_bb_entry_type:
                        # update bios level 2 directory, fv_bb entry type = 0x62
                        point_entry.set('Address', fv_bb_addr)
                        point_entry.set('Destination', fv_bb_dest)
                        point_entry.set('Size', fv_bb_size)
                        print('FV_BB:{}'.format(point_entry.attrib))
                if args.tpcm:
                    # add fv_main point entry if tpcm is supported
                    bios_dir.append(fv_main_node)

            for image_entry in root.iter('IMAGE_ENTRY'):
                print(image_entry.attrib['File'])
                image_entry.set('File', image_entry.attrib['File'].replace('FW_SECURE_MODE',
                                                                           'DNMcm_' + args.securemode.upper()))
            # update to file
            tree.write(xml_output)
        except:
            raise Exception('Failed to update psp data layout xml files')


def main():
    global configurations
    args = parse_arg()
    print(vars(args))
    configurations = get_configurations(args.config)
    pre_update_psp_data_template(args)
    get_whole_layout()  # get all psp and bios levels base and size
    generate_fch_firmware(args)  # create fch firmware, support SOC1/SOC2/SOC3
    update_combo_template()  # update combo
    update_psp_data_template()
    print('done')


if __name__ == '__main__':
    main()
