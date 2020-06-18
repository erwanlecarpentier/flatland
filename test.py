from matplotlib.font_manager import FontProperties
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys

def test(p):
	data = pd.read_csv(p,sep = ',')
	r = data["achieved_return"]
	r = r.as_matrix()
	m = np.mean(r, dtype=np.float)
	s = np.std(r, dtype=np.float)
	print(p)
	print("mean    : %f" % m)
	print("std dev : %f\n" % s)

test("data/mcts_dyn.csv")
test("data/mcts_sta.csv")
test("data/uct_dyn.csv")
test("data/uct_sta.csv")

'''
test("data/tuct100_sta.csv")
test("data/tuct10_sta.csv")
test("data/tuct1_sta.csv")
test("data/tuct01_sta.csv")
'''
'''
test("data/tuct001_sta_nbvis.csv")
test("data/tuct100_sta_nbvis.csv")
test("data/tuct10_sta_nbvis.csv")
test("data/tuct1_sta_nbvis.csv")
test("data/tuct01_sta_nbvis.csv")
test("data/tuct001_sta_nbvis.csv")
'''

test("data/tuct1_sta_randeval.csv")

