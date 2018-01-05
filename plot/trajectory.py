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
GREY = '#4d4d4d';

cworldpath = os.path.join(CURDIR, '../config/world.cfg')
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
	xn = "x_rect" + str(i);
	yn = "y_rect" + str(i);
	wn = "w_rect" + str(i);
	hn = "h_rect" + str(i);
	w = world_config[wn];
	h = world_config[hn];
	x = world_config[xn] - w / 2;
	y = world_config[yn] - h / 2;
	r = mpatches.Rectangle((x,y),w,h,edgecolor='none',facecolor=GREY)
	ax.add_patch(r)

# Parse circles
for i in range(world_config.nb_circles):
	xn = "x_circ" + str(i);
	yn = "y_circ" + str(i);
	rn = "r_circ" + str(i);
	x = world_config[xn];
	y = world_config[yn];
	r = world_config[rn];
	c = mpatches.Circle((x,y),r,edgecolor='none',facecolor=GREY)
	ax.add_patch(c)

# Plot start -------------------------------------------------------------------

xs = world_config.initial_state_x;
ys = world_config.initial_state_y;
g = mpatches.Circle((xs,ys),0.1,edgecolor='none',facecolor=RED)
ax.add_patch(g)

# Plot goal --------------------------------------------------------------------

for i in range(world_config.nb_wp):
	xn = "x_wp" + str(i);
	yn = "y_wp" + str(i);
	rn = "r_wp" + str(i);
	x = world_config[xn];
	y = world_config[yn];
	r = world_config[rn];
	c = mpatches.Circle((x,y),r,edgecolor='none',facecolor=GREEN)
	ax.add_patch(c)

# Plot trajectory --------------------------------------------------------------

traj_path = "data/trajectory.csv";
data = pd.read_csv(traj_path,sep = ',');
x = data["x"];
y = data["y"];
lbsize = 24
ax.tick_params(axis='x', labelsize=lbsize)
ax.tick_params(axis='y', labelsize=lbsize)
ax.plot(x,y,color=LIGHTBLUE);

plt.show()

