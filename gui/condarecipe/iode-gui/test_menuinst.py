import sys
from pathlib import Path
from menuinst.api import install, remove

def create_shortcuts():
    nonadmin = Path(sys.prefix) / ".nonadmin"
    menu_dir = Path(__file__).parent
    shortcut = menu_dir / "menu.json"
    has_nonadmin =nonadmin.exists()
    for mode in ["user"]: #, "system"]:
        if mode=="user":
            open(nonadmin, 'a').close()
        install(str(shortcut))
        if nonadmin.exists():
            nonadmin.unlink()
    if has_nonadmin:
        open(nonadmin, 'a').close()

def remove_shortcuts():
    nonadmin = Path(sys.prefix) / ".nonadmin"
    menu_dir = Path(__file__).parent
    shortcut = menu_dir / "menu.json"
    has_nonadmin =nonadmin.exists()
    for mode in ["user"]: #, "system"]:
        if mode=="user":
            open(nonadmin, 'a').close()
        remove(str(shortcut))
        if nonadmin.exists():
            nonadmin.unlink()
    if has_nonadmin:
        open(nonadmin, 'a').close()


if __name__ == "__main__":
    create_shortcuts()
    # remove_shortcuts()
