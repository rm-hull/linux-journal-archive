#!/usr/bin/python
import sys, regex, HTMLgen

def parse_lsm(text):
    fields = {}  # New dictionary, ie hash table.
    order = []   # New list of field names.
    if text[0][:6] == "Begin3":     # Valid LSM
	for line in text[1:]:
	    n = regex.match("[^\t\n ]+:", line)
	    if n > 0:               # New field
		name,value=(line[:n-1],line[n+1:])
		order.append(name)  # Save order
		fields[name] = value# Store value
	    elif line != "End\n":   # Append value
		fields[name] = fields[name] + line
    return (order, fields)

def html_lsm(keys, data):
    doc = HTMLgen.SeriesDocument("lsmstyle.rc")
    doc.title = HTMLgen.Text(data["Title"])
    table = HTMLgen.TableLite() 
    for key in keys:          # Process in order
	name  = HTMLgen.Text(key + ":")
	value = HTMLgen.Text(data[key])
	row = HTMLgen.TR()    # TRow and col TData
	row.append(HTMLgen.TD(HTMLgen.Bold(name),
			      valign="top"))
	row.append(HTMLgen.TD(value))
	table.append(row)
    doc.append(table)         # Finished
    return doc

for filename in sys.argv[1:]:
    lsmfile = open(filename, "r")
    keys, data = parse_lsm(lsmfile.readlines())
    html_lsm(keys, data).write(filename + ".html")
    lsmfile.close()
