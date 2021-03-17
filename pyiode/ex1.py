import iode

print("IODE version : {1} ", iode.version())
iode.wsloadvar("../a")
iode.contents("C*", 6)
x = iode.getvector(b"CLI_GHG_BE", 1)
print(x)