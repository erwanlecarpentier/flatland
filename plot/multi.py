
"""
Plot the reached loes and computational costs mns and standard deviation for
a bunch of different groups of simulations.
Set the correct paths and run this script.

edit: 26/09/2017
"""

from matplotlib.font_manager import FontProperties
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys

BLUE = '#333399';
ORANGE = '#ff6600';
GREEN = '#00cc66';
BROWN = '#511606';
GREY = '#b8b8b8';

PL0 = '#ff0000';
PL1 = '#6c0d0d';
PL2 = '#400073';
PL3 = '#03007e';
PL4 = '#0045ff';
PL5 = '#6699ff';

def plot_error_bar(ax, rng, mean, stddev, cl, mrk, fcc):
	l = ax.errorbar(rng, mean, color=cl, yerr=stddev)
	l = ax.errorbar(rng, mean, color=cl, yerr=stddev, fmt=mrk, markersize=8, markerfacecolor=fcc)
	v = np.copy(stddev)
	up = np.add(mean, +v)
	dw = np.add(mean, -v)
	ax.fill_between(rng, up, dw, color=cl, alpha=0.1)
	return l

def extract(path, failure_probability_range, take_log):
	lomns = []
	lostd = []
	cpmns = []
	cpstd = []
	camns = []
	castd = []
	for fp in failure_probability_range:
		p =	 path + fp + ".csv"
		data = pd.read_csv(p,sep = ',')
		lo = data["score"]
		cp = data["computational_cost"]
		ca = data["nb_calls"]
		if take_log:
			lo = np.log(lo)
			cp = np.log(cp)
			ca = np.log(ca)
		lomns.append(lo.mean())
		lostd.append(lo.std())
		cpmns.append(cp.mean())
		cpstd.append(cp.std())
		camns.append(ca.mean())
		castd.append(ca.std())
	return [lomns, lostd, cpmns, cpstd, camns, castd]

def plot(path, args, color, mrk, fcc):
	fp_range        = args[0]
	fp_range_val = args[1]
	take_log        = args[2]
	ax1             = args[3]
	ax2             = args[4]
	ax3             = args[5]
	[lo_mea, lo_std, cp_mea, cp_std, ca_mea, ca_std] = extract(path,fp_range,take_log)
	l = plot_error_bar(ax1, fp_range_val, lo_mea, lo_std, color, mrk, fcc)
	l = plot_error_bar(ax2, fp_range_val, cp_mea, cp_std, color, mrk, fcc)
	l = plot_error_bar(ax3, fp_range_val, ca_mea, ca_std, color, mrk, fcc)
	return l

# Variables --------------------------------------------------------------------

# Take the log of the data or not
take_log = False

# Different failure probabilities (wrt the files names)
#fp_range = ["000", "005", "010", "015", "020", "025", "030", "035", "040", "045", "050", "055", "060", "065", "070", "075", "080", "085", "090", "095", "1"]
fp_range = ["0", "5", "10", "15", "20", "25", "30", "35", "40", "45", "50"]

# Same with real values for plotting
#fp_range_val = [0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.]
fp_range_val = [0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5]

# Plot -------------------------------------------------------------------------

plt.close('all')

f, (ax1, ax2, ax3) = plt.subplots(3, 1, sharex=True)

root_path = 'data/';
#root_path = 'data/backup/discrete/short_plc';
args = [fp_range, fp_range_val, take_log, ax1, ax2, ax3]
#l1 = plot(root_path + '1_b0', args, PL1, 's', PL1)
#l2 = plot(root_path + '1_b1', args, PL2, '^', PL2)
#l3 = plot(root_path + '1_b2', args, PL3, 'o', 'none')
#l4 = plot(root_path + '1_b3', args, PL4, 's', 'none')
#l5 = plot(root_path + '1_b4', args, PL5, '^', 'none')
l0 = plot(root_path + 'uct',    args, PL0, 'o', PL0)


# Label ------------------------------------------------------------------------

font = FontProperties()
font.set_family('serif')
font.set_style('normal')
font.set_size('large') # medium
font.set_weight('light')

labsz = 15
xlab = 'Transition misstep probability'
ylab1 = 'Loss (time steps to the goal)'
ylab1_log = 'Log of the loss\n(time steps to the goal)'
ylab2 = 'Computational cost (ms)'
ylab2_log = 'Log of the\ncomputational cost (ms)'
ylab3 = 'Number of call'
ylab3_log = 'Log of the\nnumber of call'

ax1.set_xlabel(xlab, fontproperties=font)
if take_log:
	ax1.set_ylabel(ylab1_log, fontproperties=font)
else:
	ax1.set_ylabel(ylab1, fontproperties=font)

ax2.set_xlabel(xlab, fontproperties=font)
if take_log:
	ax2.set_ylabel(ylab2_log, fontproperties=font)
else:
	ax2.set_ylabel(ylab2, fontproperties=font)

ax3.set_xlabel(xlab, fontproperties=font)
if take_log:
	ax3.set_ylabel(ylab3_log, fontproperties=font)
else:
	ax3.set_ylabel(ylab3, fontproperties=font)

# Legend -----------------------------------------------------------------------
'''
l = [l0,l1,l2,l3,l4,l5]
lg = ['Vanilla UCT', 'Plain OLUCT', 'State-modality OLUCT', 'State-variance OLUCT', 'State-distance OLUCT', 'Outcome-variance OLUCT']
#ax1.legend(l, lg, numpoints=1, loc='upper left', prop=font)
#ax2.legend(l, lg, numpoints=1, loc='upper left', prop=font)
ax3.legend(l, lg, numpoints=1, loc='upper left', prop=font)
'''
plt.show()
