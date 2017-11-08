
"""
Plot the reached loes and computational costs mns and standard deviation for
a bunch of different groups of simulations.
Set the correct paths and run this script.

edit: 26/09/2017
"""

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys

BLUE = '#333399';
ORNG = '#ff6600';
GREE = '#00cc66';
GREY = '#b8b8b8';

def plot_error_bar(ax, rng, mean, stddev, cl):
	l = ax.errorbar(rng, mean, color=cl, yerr=stddev)
	l = ax.errorbar(rng, mean, color=cl, yerr=stddev, fmt='o')
	v = np.copy(stddev)
	up = np.add(mean, +v)
	dw = np.add(mean, -v)
	ax.fill_between(rng, up, dw, color=cl, alpha=0.1)
	return l

def extract(path, take_log):
	lomns = []
	lostd = []
	cpmns = []
	cpstd = []
	camns = []
	castd = []
	for fp in fp_range:
		data = pd.read_csv(path,sep = ',')
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

# Variables --------------------------------------------------------------------

labsz = 15
xlab = 'Transition misstep probability'
ylab1 = 'Loss (time steps to the goal)'
ylab1_log = 'Log of the loss\n(time steps to the goal)'
ylab2 = 'Computational cost (ms)'
ylab2_log = 'Log of the\ncomputational cost (ms)'
ylab3 = 'Number of call'
ylab3_log = 'Log of the\nnumber of call'
leg1 = 'Vanilla UCT'
#leg2 = 'Plain OLUCT'
#leg3 = 'SDC-OLUCT'

# Take the log of the data or not
take_log = True

# Different failure probabilities (wrt the files names)
#fp_range = ["000", "005", "010", "015", "020", "025", "030", "035", "040", "045", "050", "055", "060", "065", "070", "075", "080", "085", "090", "095", "1"]
fp_range = ["000", "005", "010", "015", "020", "025", "030", "035", "040", "045", "050"]

# Same with real values for plotting
#fp_range_values = [0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.]
fp_range_values = [0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5]

# Initialisation ---------------------------------------------------------------

path = "data/test.csv"
[lo1_mns, lo1_std, cp1_mns, cp1_std, ca1_mns, ca1_std] = extract(path, take_log)

plt.close('all')
f, (ax1, ax2, ax3) = plt.subplots(3, 1, sharex=True)

### Plot -----------------------------------------------------------------------

# Plot 1 - loss

l1 = plot_error_bar(ax1,fp_range_values,lo1_mns,lo1_std,BLUE)

ax1.set_xlabel(xlab,{'fontsize':labsz})
if take_log:
	ax1.set_ylabel(ylab1_log,{'fontsize':labsz})
else:
	ax1.set_ylabel(ylab1,{'fontsize':labsz})
ax1.legend([l1],[leg1],numpoints=1,loc='upper left')

# Plot 2 - computational cost

l1 = plot_error_bar(ax2,fp_range_values,cp1_mns,cp1_std,BLUE)

ax2.set_xlabel(xlab,{'fontsize':labsz})
if take_log:
	ax2.set_ylabel(ylab2_log,{'fontsize':labsz})
else:
	ax2.set_ylabel(ylab2,{'fontsize':labsz})
ax2.legend([l1],[leg1],numpoints=1,loc='upper left')

# Plot 3 - number of call

l1 = plot_error_bar(ax3,fp_range_values,ca1_mns,ca1_std,BLUE)

ax3.set_xlabel(xlab,{'fontsize':labsz})
if take_log:
	ax3.set_ylabel(ylab3_log,{'fontsize':labsz})
else:
	ax3.set_ylabel(ylab3,{'fontsize':labsz})
ax3.legend([l1],[leg1],numpoints=1,loc='upper left')

plt.show()

