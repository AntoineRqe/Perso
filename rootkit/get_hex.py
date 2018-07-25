import sys
import base64

base = str(sys.argv[1])
src = str(sys.argv[2])
dst = src + "_hex.txt"

base_16=0
base_32=0
base_64=0

if base == '32':
    base_32 = 1
elif base == '64':
    base_64 = 1
else:
    base_16 = 1

print ("src:{}, dst:{}".format(src, dst))
with open(src,"rb") as rootkit:
    with open(dst, "w") as f:
        if base_16 == 1:
            print 'Encoding base 16'
            f.write(rootkit.read().encode("hex"))
        elif base_32 == 1:
            print 'Encoding base 32'
            f.write(base64.b32encode(rootkit.read()))
        elif base_64 == 1:
            print 'Encoding base 64'
            f.write(base64.b64encode(rootkit.read()))



