import os
import io
import libconf
import matplotlib.pyplot as plt

CURDIR = os.path.abspath(os.path.dirname(__file__))

cworldpath = os.path.join(CURDIR, '../config/cworld.cfg')
with io.open(cworldpath) as f:
	config = libconf.load(f)

print(config.xsize)

