import os
import io
import libconf
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import pandas as pd
import numpy as np

CURDIR = os.path.abspath(os.path.dirname(__file__))
BLACK = '#000000'
RED = '#d85040'
GREEN = '#00cc66'
LIGHTBLUE = '#6699ff'
GREY = '#4d4d4d'

def plot_waypoint(i, world_config, ax) :
	x = world_config["x_wp" + str(i)]
	y = world_config["y_wp" + str(i)]
	r = world_config["r_wp" + str(i)]
	c = mpatches.Circle((x,y),r,edgecolor='none',facecolor=GREEN)
	ax.add_patch(c)

def plot_rfield(i, world_config, ax) :
	traj = pd.read_csv("data/rfield" + str(i) + ".csv", sep = ',')
	x = traj["x"]
	y = traj["y"]
	tb = world_config["tbirth_rf" + str(i)]
	td = world_config["tdeath_rf" + str(i)]
	sig = world_config["sigma_rf" + str(i)]
	mag = world_config["magnitude_rf" + str(i)]
	#ax.plot(x,y,color=LIGHTBLUE) # trajectory
	for j in range(0,td,1) :
		a = (j+5)/((td+5)*1.5)
		c = mpatches.Circle((x[j],y[j]),3.*sig, fc='none', ec=LIGHTBLUE,alpha=a)
		ax.add_patch(c)
		if (j == 0):
			ax.annotate('t = %d' %tb, xy=(x[j],y[j]))
		if (j == td-1):
			ax.annotate('t = %d' %td, xy=(x[j],y[j]))

mainpath = os.path.join(CURDIR, '../config/main.cfg')
with io.open(mainpath) as g:
	main_config = libconf.load(g)
cworldpath = os.path.join(CURDIR, '.' + main_config.world_path)
with io.open(cworldpath) as f:
	world_config = libconf.load(f)

# Create figure and axes
fig,ax = plt.subplots(1)
ax.set_xlim([0,world_config.xsize])
ax.set_ylim([0,world_config.ysize])

# Plot world -------------------------------------------------------------------

# Parse rectangles
for i in range(world_config.nb_rectangles):
	w = world_config["w_rect" + str(i)]
	h = world_config["h_rect" + str(i)]
	x = world_config["x_rect" + str(i)] - w / 2
	y = world_config["y_rect" + str(i)] - h / 2
	r = mpatches.Rectangle((x,y),w,h,edgecolor='none',facecolor=GREY)
	ax.add_patch(r)

# Parse circles
for i in range(world_config.nb_circles):
	x = world_config["x_circ" + str(i)]
	y = world_config["y_circ" + str(i)]
	r = world_config["r_circ" + str(i)]
	c = mpatches.Circle((x,y),r,edgecolor='none',facecolor=GREY)
	ax.add_patch(c)

# Plot start -------------------------------------------------------------------

xs = world_config.initial_state_x
ys = world_config.initial_state_y
g = mpatches.Circle((xs,ys),0.02,edgecolor='none',facecolor=RED)
ax.add_patch(g)

# Plot reward ------------------------------------------------------------------

if world_config.reward_model_selector == 0 : # Heatmap reward model
	for i in range(world_config.nb_rfield) :
		plot_rfield(i, world_config, ax)
else: # Waypoints reward model
	for i in range(world_config.nb_waypoints) :
		plot_waypoint(i, world_config, ax)

# Plot trajectory --------------------------------------------------------------

data = pd.read_csv("data/trajectory.csv", sep = ',')
x = data["x"]
y = data["y"]
lbsize = 24
ax.tick_params(axis='x', labelsize=lbsize)
ax.tick_params(axis='y', labelsize=lbsize)
ax.plot(x,y,color=RED)
ax.annotate('Start t = 0', xy=(x[0],y[0]), xytext=(x[0]+0.02,y[0]+0.02), color=RED)
eot = x.size - 1
ax.annotate('End t = %d' %eot, xy=(x[eot],y[eot]), xytext=(x[eot]+0.02,y[eot]+0.02), color=RED)

plt.show()

