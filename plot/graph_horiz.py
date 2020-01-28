
"""
Plot the reached loes and computational costs mns and standard deviation for
a bunch of different groups of simulations.
Set the correct paths and run this script.

edit: 26/09/2017
"""

from matplotlib.ticker import FuncFormatter
from matplotlib.font_manager import FontProperties
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys
import matplotlib

# Use Type 1 fonts
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

BLUE = '#333399';
ORANGE = '#ff6600';
GREEN = '#00cc66';
BROWN = '#511606';
GREY = '#b8b8b8';

'''
PL0 = '#ff0000';
PL1 = '#6c0d0d';
PL2 = '#400073';
PL3 = '#03007e';
PL4 = '#0045ff';
PL5 = '#6699ff';
'''
PL0 = '#000000';
PL1 = '#00e673';
PL2 = '#804dff';
PL3 = '#ff6600';
PL4 = '#ee1a7a';
PL5 = '#6699ff';

def plot_error_bar(ax, rng, mean, stddev, cl, mrk, fcc, lw, ls, take_log):
	if take_log:
		ax.set_yscale('log')
	l = ax.errorbar(rng, mean, color=cl, yerr=stddev, fmt=mrk, linewidth=lw, linestyle=ls, markersize=8, markerfacecolor=fcc)
	v = np.copy(stddev)
	up = np.add(mean, +v)
	dw = np.add(mean, -v)
	ax.fill_between(rng, up, dw, color=cl, alpha=0.05)
	return l

def plot_relative_means(ax, rng, mean, stddev, cl, mrk, fcc, lw, ls, oluct_lo_mea):
	values = [(a - b)/b for a, b in zip(mean, oluct_lo_mea)]
	l = ax.plot(rng, values, color=cl, marker=mrk, linewidth=lw, linestyle=ls, markersize=8, markerfacecolor=fcc)
	return l

def extract(path, failure_probability_range):
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
		lomns.append(lo.mean())
		lostd.append(lo.std())
		cpmns.append(cp.mean())
		cpstd.append(cp.std())
		camns.append(ca.mean())
		castd.append(ca.std())
	return [lomns, lostd, cpmns, cpstd, camns, castd]

def plot(path, args, color, mrk, fcc, lw, ls, oluct_lo_mea):
	fp_range        = args[0]
	fp_range_val = args[1]
	take_log        = args[2]
	ax1             = args[3]
	ax2             = args[4]
	ax3             = args[5]
	ax4             = args[6]
	[lo_mea, lo_std, cp_mea, cp_std, ca_mea, ca_std] = extract(path,fp_range)
	l = plot_error_bar(ax1, fp_range_val, lo_mea, lo_std, color, mrk, fcc, lw, ls, take_log)
	l = plot_relative_means(ax2, fp_range_val, lo_mea, lo_std, color, mrk, fcc, lw, ls, oluct_lo_mea)
	l = plot_error_bar(ax3, fp_range_val, cp_mea, cp_std, color, mrk, fcc, lw, ls, take_log)
	l = plot_error_bar(ax4, fp_range_val, ca_mea, ca_std, color, mrk, fcc, lw, ls, take_log)
	return l

def apply_label(ax, xlab, ylab, font, per):
	ax.set_xlabel(xlab, fontproperties=font)
	ax.set_ylabel(ylab, fontproperties=font)
	if per:
		ax.yaxis.set_major_formatter(FuncFormatter(lambda y, _: '{:.0%}'.format(y))) 

# Variables --------------------------------------------------------------------

# Take the log of the data or not
take_log = False
discrete = False

# Different failure probabilities (wrt the files names)
fp_range = ["0", "5", "10", "15", "20", "25", "30", "35", "40", "45", "50"]

# Same with real values for plotting
fp_range_val = [0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5]

# Plot -------------------------------------------------------------------------

plt.close('all')
f, ((ax1, ax3), (ax2, ax4)) = plt.subplots(2, 2, sharex='col', figsize=(8.0,8.0))
ax1.set_xlim(0.0, 0.5)

if discrete:
	root_path = 'data/backup/olta/discrete/'
else:
	root_path = 'data/backup/olta/continuous/'
[oluct_lo_mea, oluct_lo_std, oluct_cp_mea, oluct_cp_std, oluct_ca_mea, oluct_ca_std] = extract(root_path + 'oluct',fp_range)
lw = 2.3
args = [fp_range, fp_range_val, take_log, ax1, ax2, ax3, ax4]
l1 = plot(root_path + 'olta0', args, PL1, 's', PL1,    lw, '-', oluct_lo_mea)
if discrete:
	l2 = plot(root_path + 'olta1', args, PL2, 's', PL2,    lw, '-', oluct_lo_mea)
l3 = plot(root_path + 'olta2', args, PL3, '^', 'none', lw, '-.', oluct_lo_mea)
l4 = plot(root_path + 'olta3', args, PL4, 'o', 'none', lw, '--', oluct_lo_mea)
l5 = plot(root_path + 'olta4', args, PL5, 's', 'none', lw, '-', oluct_lo_mea)
l0 = plot(root_path + 'oluct',    args, PL0, 'o', PL0,    lw, '-', oluct_lo_mea)


# Label ------------------------------------------------------------------------

font = FontProperties()
font.set_family('serif')
font.set_style('normal')
font.set_size('x-large')
font.set_weight('light')

labsz = 13
xlab = '$p$ = transition misstep probability'
ylab1 = 'Loss (time steps to\nreach all the waypoints)'
ylab2 = 'Mean loss relatively\nto OLUCT loss'
ylab3 = 'Computational cost (ms)'
ylab4 = 'Number of calls'

apply_label(ax1,xlab,ylab1,font,False)
apply_label(ax2,xlab,ylab2,font,True)
apply_label(ax3,xlab,ylab3,font,False)
apply_label(ax4,xlab,ylab4,font,False)

# Legend -----------------------------------------------------------------------
if discrete:
	l = [l0,l1,l2,l3,l4,l5]
	lg = ['OLUCT', 'Plain OLTA', 'SDM-OLTA', 'SDV-OLTA', 'SDSD-OLTA', 'RDV-OLTA']
else:
	l = [l0,l1,l3,l4,l5]
	lg = ['OLUCT', 'Plain OLTA', 'SDV-OLTA', 'SDSD-OLTA', 'RDV-OLTA']

#ax4.legend(l, lg, numpoints=1, prop=font, bbox_to_anchor=(0., -0.8, 1., .102), loc='lower left', ncol=3, mode="expand", borderaxespad=0., frameon=False)

plt.figlegend( l, lg, prop=font, loc='lower center', ncol=5, labelspacing=0. )

plt.show()
