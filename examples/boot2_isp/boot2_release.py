import sys,os,shutil
import hashlib, sys, binascii, struct
import time
import subprocess
import re
import zipfile
import argparse

git_cmd=r'git describe --tags '#--dirty
tag_standard="boot2_bl602_v1.0.0"

release_list = [
[r'make clean;make CHIP=bl602            BOARD=bl602dk CONFIG_DEBUG=n',"bl602","release"],
[r'make clean;make CHIP=bl602            BOARD=bl602dk CONFIG_DEBUG=y',"bl602","debug"],
[r'make clean;make CHIP=bl702            BOARD=bl702dk CONFIG_DEBUG=n',"bl702","release"],
[r'make clean;make CHIP=bl702            BOARD=bl702dk CONFIG_DEBUG=y',"bl702","debug"],
[r'make clean;make CHIP=bl702l CPU_ID=m0 BOARD=bl702ldk CONFIG_DEBUG=n',"bl702l","release"],
[r'make clean;make CHIP=bl702l CPU_ID=m0 BOARD=bl702ldk CONFIG_DEBUG=y',"bl702l","debug"],
[r'make clean;make CHIP=bl808  CPU_ID=m0 BOARD=bl808dk CONFIG_DEBUG=n',"bl808","release"],
[r'make clean;make CHIP=bl808  CPU_ID=m0 BOARD=bl808dk CONFIG_DEBUG=y',"bl808","debug"],
[r'make clean;make CHIP=bl606p CPU_ID=m0 BOARD=bl606pdk CONFIG_DEBUG=n',"bl606p","release"],
[r'make clean;make CHIP=bl606p CPU_ID=m0 BOARD=bl606pdk CONFIG_DEBUG=y',"bl606p","debug"],
[r'make clean;make CHIP=bl616  CPU_ID=m0 BOARD=bl616dk CONFIG_ANTI_ROLLBACK=y CONFIG_DEBUG=n',"bl616","release"],
[r'make clean;make CHIP=bl616  CPU_ID=m0 BOARD=bl616dk CONFIG_ANTI_ROLLBACK=y CONFIG_DEBUG=y',"bl616","debug"],
[r'make clean;make CHIP=bl616d CPU_ID=ap BOARD=bl616ddk CONFIG_DEBUG=n',"bl616d","release"],
[r'make clean;make CHIP=bl616d CPU_ID=ap BOARD=bl616ddk CONFIG_DEBUG=y',"bl616d","debug"],
[r'make clean;make CHIP=bl616l BOARD=bl616ldk CONFIG_DEBUG=n',"bl616l","release"],
[r'make clean;make CHIP=bl616l BOARD=bl616ldk CONFIG_DEBUG=y',"bl616l","debug"],
]

def zipDir(dirpath,outFullName):
    """
    Compress the specified folder
    :param dirpath: Path of the target folder
    :param outFullName: Save path of the compressed file + xxxx.zip
    :return: None
    """
    zip = zipfile.ZipFile(outFullName,"w",zipfile.ZIP_DEFLATED)
    for path,dirnames,filenames in os.walk(dirpath):
        # Remove the target and its path, and only compress the files and folders under the target folder.
        fpath = path.replace(dirpath,'')

        for filename in filenames:
            zip.write(os.path.join(path,filename),os.path.join(fpath,filename))
    zip.close()

def recreate_release_dir(dir):
    if os.path.exists(dir):
        shutil.rmtree(dir)#Delete and then rebuild.
        os.makedirs(dir)
    else:
        os.makedirs(dir)

def boot2_release(cmd, ver):
    print("Executing command:", cmd)

    process = subprocess.Popen(
        f"{cmd[0]} CONFIG_BOOT2_VER={ver}",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=True,
        text=True
    )

    output, err = process.communicate()

    if process.returncode != 0:
        print("Build error:", err.strip())
        return False

    print(output.strip())

    target_dir = f'boot2_isp_{cmd[1]}_v{ver}'
    for root, dirs, files in os.walk("./build/build_out", topdown=False):
        for file in files:
            if file.endswith(".bin") and cmd[1] in file and "boot2_isp" in file:
                src = os.path.join(root, file)
                dest = os.path.join(
                    target_dir,
                    f'boot2_{cmd[1]}_isp_{cmd[2]}_v{ver}.bin'
                )
                shutil.copy(src, dest)
                print(f"Copied: {src} -> {dest}\\n")
                return

def get_release_ver(chip):
    """
    get tag version, eg: boot2_bl602_v1.0.1
    """
    chip_tag_prefix = f"boot2_{chip}_v"
    git_cmd = f"git tag | grep '{chip_tag_prefix}' | sort | tail -n 1"
    print(f"get release ver for {chip}")
    output,err = subprocess.Popen(git_cmd,stdout=subprocess.PIPE,shell=True).communicate()
    if err==None:
        release_name = bytes.decode(output).strip()
        if not release_name or not release_name.startswith(chip_tag_prefix):
            print(f"No valid tag found for {chip}, release failed!")
            sys.exit(1)
        ver = release_name[len(chip_tag_prefix):]
        return ver
    else:
        print(f"get ver error for {chip}, please push correct tag")
        sys.exit(1)

def get_latest_boot2_tag():
    """
    get latest boot2_* tag info (tag, chip, ver)
    eg: boot2_{chip}_v{ver}
    """
    git_cmd = "git tag | grep '^boot2_' | sort | tail -n 1"
    print("get latest boot2 tag")
    try:
        output = subprocess.check_output(git_cmd, shell=True, text=True).strip()
    except subprocess.CalledProcessError:
        print("no boot2 tag found")
        return None, None, None

    if not output:
        print("no boot2 tag found")
        return None, None, None

    m = re.match(r'^boot2_([a-z0-9]+)_v(.+)$', output, re.IGNORECASE)
    if not m:
        print(f"tag '{output}' does not match expected pattern 'boot2_{'{chip}'}_v{{ver}}'")
        return output, None, None

    chip = m.group(1).lower()
    ver = m.group(2)
    return output, chip, ver


if __name__ == '__main__' :
    failed_list = []

    # normalize args to lower-case
    cli_args = [arg.lower() for arg in sys.argv[1:]]

    if cli_args:
        # User supplied chips on command line - keep previous behaviour
        filtered_release_list = [x for x in release_list if x[1].lower() in cli_args]
        if not filtered_release_list:
            print(f"Error: No release entry found for chip(s): {', '.join(sys.argv[1:])}")
            sys.exit(1)

        # get release ver for each chip specified
        chip_ver_map = {}
        for chip in list(set([x[1].lower() for x in filtered_release_list])):
            ver = get_release_ver(chip)
            print(f"{chip} version: {ver}")
            chip_ver_map[chip] = ver

    else:
        # No args: determine chip and version from latest boot2 tag
        tag, chip, ver = get_latest_boot2_tag()
        if not tag or not chip or not ver:
            print("Cannot determine chip/version from tags. Provide chip name as argument or ensure tags exist.")
            sys.exit(1)
        print(f"Detected tag {tag} -> chip={chip}, ver={ver}")

        # filter release_list for this chip
        filtered_release_list = [x for x in release_list if x[1].lower() == chip]
        if not filtered_release_list:
            print(f"No build entries for chip '{chip}' in release_list")
            sys.exit(1)

        chip_ver_map = {chip: ver}

    # prepare output dirs for each filtered entry using determined version
    for x in filtered_release_list:
        chip = x[1].lower()
        ver = chip_ver_map[chip]
        recreate_release_dir('boot2_isp_' + x[1] + '_v' + ver )

    # perform builds
    for x in filtered_release_list:
        chip = x[1].lower()
        ver = chip_ver_map[chip]
        if boot2_release(x,ver) == False:
            failed_list.append(x)

    if len(failed_list) == 0:
        print("release success!")
    else:
        print("release failed! failed list:")
        for x in failed_list:
            print('-',x[1],x[2])