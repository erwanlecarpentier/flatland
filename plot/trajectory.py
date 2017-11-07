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
	world_config = libconf.load(f)
mainpath = os.path.join(CURDIR, '../config/main.cfg')
with io.open(mainpath) as g:
	main_config = libconf.load(g)

# Create figure and axes
fig,ax = plt.subplots(1)
ax.set_xlim([0,world_config.xsize])
ax.set_ylim([0,world_config.ysize])

# Plot world -------------------------------------------------------------------

# Parse rectangles
for i in range(world_config.nb_rectangles):
	n_wdth = "dr" + str(i) + "x";
	n_hght = "dr" + str(i) + "y";
	n_cx   = "cr" + str(i) + "x";
	n_cy   = "cr" + str(i) + "y";
	wdth = world_config[n_wdth];
	hght = world_config[n_hght];
	cx = world_config[n_cx] - wdth / 2;
	cy = world_config[n_cy] - hght / 2;
	r = mpatches.Rectangle((cx,cy),wdth,hght,edgecolor='none',facecolor=BLACK, alpha = 0.5)
	ax.add_patch(r)

# Parse circles
for i in range(world_config.nb_circles):
	n_radius = "r" + str(i);
	n_cx   = "cc" + str(i) + "x";
	n_cy   = "cc" + str(i) + "y";
	radius = world_config[n_radius];
	cx = world_config[n_cx];
	cy = world_config[n_cy];
	c = mpatches.Circle((cx,cy),radius,edgecolor='none',facecolor=BLACK, alpha = 0.5)
	ax.add_patch(c)

# Plot start -------------------------------------------------------------------

xs = main_config.initial_state_x;
ys = main_config.initial_state_y;
g = mpatches.Circle((xs,ys),0.03,edgecolor='none',facecolor=BLACK)
ax.add_patch(g)

# Plot goal --------------------------------------------------------------------

xg = world_config.xgoal;
yg = world_config.ygoal;
rg = world_config.rgoal;
g = mpatches.Circle((xg,yg),rg,edgecolor='none',facecolor=GREEN, alpha = 0.5)
ax.add_patch(g)

# Plot trajectory --------------------------------------------------------------

traj_path = "data/trajectory.csv";
data = pd.read_csv(traj_path,sep = ',');
x = data["x"];
y = data["y"];
ax.plot(x,y,color=LIGHTBLUE);

plt.show()

