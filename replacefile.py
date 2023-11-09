import sys

if (len(sys.argv)<4):
    print("Usage:", sys.argv[0], "<search> <replace> <files>")
    sys.exit(0)

for fname in sys.argv[3:]:
    lines = []
    rlines = []
    print(fname)
    with open(fname, "r") as f:
        try:
            lines = f.readlines()
        except:
            continue
        for l in lines:
            rlines.append(l.replace(sys.argv[1], sys.argv[2]))
    with open(fname, "w") as f:
        f.writelines(rlines)
