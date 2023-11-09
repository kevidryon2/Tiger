import sys

while True:
    try:
        ipt = input()
    except (EOFError):
        break
    try:
        f = open(ipt)
        print(ipt)
        f.close()
    except (IsADirectoryError):
        continue
