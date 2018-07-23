import sys

with open("rootkit.ko","rb") as rootkit:
    with open("rootkit_hex.txt", "w") as f:
        f.write(rootkit.read().encode("hex"))


