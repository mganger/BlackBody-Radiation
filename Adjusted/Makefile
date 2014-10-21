generate: 10Vtrial9.csv 9Vtrial8.csv 8Vtrial7.csv 7Vtrial6.csv 5Vtrial4.csv 5Vtrial2.csv 5Vtrial2.csv 6Vtrial3.csv 5Vtrial1.csv
parser = ../parser2/parser

10Vtrial9.csv:
	$(parser) ../10Vtrial9.csv  10Vtrial9.csv 2 10 10 .74 1.62 293 -.95942
9Vtrial8.csv:
	$(parser) ../9Vtrial8.csv  9Vtrial8.csv 2 10 9 .7 1.62 293 -.95942
8Vtrial7.csv:
	$(parser) ../8Vtrial7.csv  8Vtrial7.csv 2 10 8 .66 1.62 293 -.95942
7Vtrial6.csv:
	$(parser) ../7Vtrial6.csv  7Vtrial6.csv 2 10 7 0.61 1.62 293 -.95942
6Vtrial3.csv:
	$(parser) ../6Vtrial3.csv  6Vtrial3.csv 2 10 6 0.56 1.62 293 -.95942
5Vtrial4.csv:
	$(parser) ../5Vtrial4.csv  5Vtrial4.csv 2 10 5 0.51 1.62 293 -.95942
5Vtrial2.csv:
	$(parser) ../5Vtrial2.csv  5Vtrial2.csv 2 10 5 0.51 1.62 293 -.95942
5Vtrial1.csv:
	$(parser) ../5Vtrial1.csv  5Vtrial1.csv 2 10 5 0.51 1.62 293 -.95942

graph: generate
	gnuplot generate.gpi
	mv *.pdf graphs
