#!/usr/bin/python
import string, os, HTMLgen, barchart

inpipe = os.popen("ps vax", "r");
colnames = string.split(inpipe.readline())

chart = barchart.StackedBarChart()
chart.title = "Text/Data Memory per Process"
chart.datalist = barchart.DataList()
chart.datalist.segment_names = colnames[5:7]
data = chart.datalist

for line in inpipe.readlines():
    cols    = string.split(line)
    barname = string.join(cols[10:], " ")
    tsize   = string.atoi(cols[6])
    dsize   = string.atoi(cols[7])
    data.load_tuple(( barname, tsize, dsize ))

data.sort(key=colnames[5], direction="decreasing")

doc = HTMLgen.SimpleDocument(title='Memory')
doc.append(chart)
doc.write("psv.html")
