import os
import io
import libconf
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import pandas as pd
import numpy as np

CURDIR = os.path.abspath(os.path.dirname(__file__))
BLACK = '#000000';
RED = '#d85040';
GREEN = '#00cc66';
LIGHTBLUE = '#6699ff';

cworldpath = os.path.join(CURDIR, '../config/cworld.cfg')
with io.open(cworldpath) as f:
	config = libconf.load(f)

# Create figure and axes
fig,ax = plt.subplots(1)
ax.set_xlim([0,config.xsize])
ax.set_ylim([0,config.ysize])

# Plot world -------------------------------------------------------------------

# Parse rectangles
for i in range(config.nb_rectangles):
	n_wdth = "dr" + str(i) + "0";
	n_hght = "dr" + str(i) + "1";
	n_cx   = "cr" + str(i) + "0";
	n_cy   = "cr" + str(i) + "1";
	wdth = config[n_wdth];
	hght = config[n_hght];
	cx = config[n_cx] - wdth / 2;
	cy = config[n_cy] - hght / 2;
	r = mpatches.Rectangle((cx,cy),wdth,hght,edgecolor='none',facecolor=BLACK, alpha = 0.5)
	ax.add_patch(r)

# Parse circles
for i in range(config.nb_circles):
	n_radius = "r" + str(i);
	n_cx   = "cc" + str(i) + "0";
	n_cy   = "cc" + str(i) + "1";
	radius = config[n_radius];
	cx = config[n_cx];
	cy = config[n_cy];
	c = mpatches.Circle((cx,cy),radius,edgecolor='none',facecolor=BLACK, alpha = 0.5)
	ax.add_patch(c)

# Plot goal --------------------------------------------------------------------

xg = config.xgoal;
yg = config.ygoal;
rg = config.rgoal;
g = mpatches.Circle((xg,yg),rg,edgecolor='none',facecolor=GREEN, alpha = 0.5)
ax.add_patch(g)

# Plot trajectory --------------------------------------------------------------

traj_path = "data/trajectory.csv";
data = pd.read_csv(traj_path,sep = ',');
x = data["x"];
y = data["y"];
ax.plot(x,y,color=RED);

plt.show()

