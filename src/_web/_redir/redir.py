import glob
import os

for f in glob.glob("*.html"):
    name = os.path.basename(f)
    print "redirect /tonigy/%s http://www.tonigy.com/%s" % (name, name)
