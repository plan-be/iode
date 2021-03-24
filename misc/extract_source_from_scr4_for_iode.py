from pathlib import Path
from shutil import copyfile, copytree, rmtree

SCR4_DIR = Path('.').absolute()

IODE_DIR = SCR4_DIR / 'iode'
if IODE_DIR.exists():
    rmtree(IODE_DIR)
IODE_DIR.mkdir()

print(IODE_DIR)

# copy subdirs
for subdir in ['bc32', 'vc32', 'vc64']:
    copytree(SCR4_DIR / subdir, IODE_DIR / subdir)

# copy macros.mak
copyfile(SCR4_DIR / 'macros.mak', IODE_DIR / 'macros.mak')

# get source files for scr4iode and s4wo.lib
prefix = ['YY', 'LST', 'SWAP1', 'SWAP2', 'XDR', 'ARGS', 'MAT', 'DIR1', 'DIR2', 'LZH', 'SW_', 'IODE_']
source_files = []
header_files = []
with open(IODE_DIR / 'macros.mak', mode='r') as f:
    for line in f:
        if any(line.startswith(pref) for pref in prefix):
            source_files += line.split('=')[1].split()
        if '.h ' in line and ':' not in line:
            header_files += line.split()

# get source file for s4_app.exe
source_files += ['scr_app.$(O)', 's_wprtf2.$(O)']

# copy all source files to subdir /iode
source_files = [file.replace('.$(O)', '.c') for file in source_files if file.endswith('.$(O)')]
for file in source_files:
    print(f"copy source file {file}")
    copyfile(SCR4_DIR / file, IODE_DIR / file)

# cleanup header_files list
header_files = [file for file in header_files if file.endswith('.h')]
# get extra header files for s4iode.lib
header_files += ['s_ydvlp.h', 'scr4_srv.h', 'sqlvers.h', 's_gzconf.h', 's_gzdefl.h', 's_gzinbl.h', 's_gzinco.h',
                 's_gzinfa.h', 's_gzinfi.h', 's_gzintr.h', 's_gzinut.h', 's_gzlib.h', 's_gztree.h', 's_gzutil.h']
# get extra header files for s4wo.lib 
header_files += ['s_lzh.h', 's_udfld.h', 's_tcwc.h', 's_filein.h', 's_gd.h', 's_gdftt.h', 's_gdftmb.h', 
                 's_gdftl.h', 's_gdftg.h', 's_gdftgr.h', 's_gdfts.h']

# copy all header files to subdir /iode
for file in header_files:
    print(f"copy header file {file}")
    copyfile(SCR4_DIR / file, IODE_DIR / file)


