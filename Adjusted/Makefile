parser = ../parser2/parser
slope = -.95942
resistance = .81
roomTemp = 295.094
movingAvg = 2

graph: clean generate
	echo temp5V =  $(shell ../parser2/resist2temp $(resistance) $(roomTemp) 5 0.51) >> temp.gpi
	echo temp6V =  $(shell ../parser2/resist2temp $(resistance) $(roomTemp) 6 0.56) >> temp.gpi
	echo temp7V =  $(shell ../parser2/resist2temp $(resistance) $(roomTemp) 7 0.61) >> temp.gpi
	echo temp8V =  $(shell ../parser2/resist2temp $(resistance) $(roomTemp) 8 0.66) >> temp.gpi
	echo temp9V =  $(shell ../parser2/resist2temp $(resistance) $(roomTemp) 9 0.70) >> temp.gpi
	echo temp10V = $(shell ../parser2/resist2temp $(resistance) $(roomTemp) 10 0.74) >> temp.gpi
	cat generate.gpi >> temp.gpi
	gnuplot temp.gpi
	rm temp.gpi
	mv *.pdf graphs

generate: 10Vtrial9.csv 9Vtrial8.csv 8Vtrial7.csv 7Vtrial6.csv 5Vtrial4.csv 5Vtrial2.csv 5Vtrial2.csv 6Vtrial3.csv 5Vtrial1.csv

10Vtrial9.csv:
	$(parser) ../10Vtrial9.csv  10Vtrial9.csv $(movingAvg) 10 10 .74 $(resistance) $(roomTemp) $(slope)
9Vtrial8.csv:
	$(parser) ../9Vtrial8.csv  9Vtrial8.csv $(movingAvg)   10 9   .7 $(resistance) $(roomTemp) $(slope)
8Vtrial7.csv:
	$(parser) ../8Vtrial7.csv  8Vtrial7.csv $(movingAvg)   10 8  .66 $(resistance) $(roomTemp) $(slope)
7Vtrial6.csv:
	$(parser) ../7Vtrial6.csv  7Vtrial6.csv $(movingAvg)   10 7 0.61 $(resistance) $(roomTemp) $(slope)
6Vtrial3.csv:
	$(parser) ../6Vtrial3.csv  6Vtrial3.csv $(movingAvg)   10 6 0.56 $(resistance) $(roomTemp) $(slope)
5Vtrial4.csv:
	$(parser) ../5Vtrial4.csv  5Vtrial4.csv $(movingAvg)   10 5 0.51 $(resistance) $(roomTemp) $(slope)
5Vtrial2.csv:
	$(parser) ../5Vtrial2.csv  5Vtrial2.csv $(movingAvg)   10 5 0.51 $(resistance) $(roomTemp) $(slope)
5Vtrial1.csv:
	$(parser) ../5Vtrial1.csv  5Vtrial1.csv $(movingAvg)   10 5 0.51 $(resistance) $(roomTemp) $(slope)

clean:
	touch something.csv
	touch graphs/something.pdf
	rm *.csv
	rm graphs/*.pdf

view: 
	evince graphs/*.pdf

merge: graph
	pdfmerge graphs/*.pdf merged.pdf
