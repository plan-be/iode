import glob
import re


files = glob.glob('**/*.cpp', recursive=True) + glob.glob('**/*.h', recursive=True)

connections_list = []
for filepath in files:
    print(f"parsing file {filepath}")
    f = open(filepath, "r")
    text = f.read()
    connections = re.findall(r"connect\((.*)\);", text)
    for connection in connections:
        lst = connection.replace("&", "").split(",")
        if(len(lst) > 1):
            connections_list.append(f"{lst[1].strip()} ({lst[0]}) -> {lst[-1].strip()}")

connections_list.sort()
print()
print("\n".join(connections_list))
