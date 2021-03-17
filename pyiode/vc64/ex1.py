import iode

varfile = "../a"
print("Iode version : ", iode.version())
print("Loading '%s'..."%varfile)
nbvars = iode.wsloadvar(varfile)
print("done")
print("Nb of read vars : %d" % nbvars)
pattern = "C*"
varlist = iode.contents(pattern, 6)
print("List of vars (pattern '{0}') : {1}".format(pattern, varlist))

for var in varlist:
     x = iode.getvector(var, 1)
     print(x)