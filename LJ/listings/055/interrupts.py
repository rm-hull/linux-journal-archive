import regsub, string, HTMLgen
                      # New HTML document.
doc = HTMLgen.SimpleDocument(title='Interrupts')
                      # New HTML table.
table = HTMLgen.Table(
    tabletitle='Interrupts', 
    border=2, width=100, cell_align="right",
    heading=[ "Description", "IRQ", "Count" ])
table.body = []       # Empty list.
doc.append(table)     # Add table to document.

interrupts_file = open('/proc/interrupts')
for line in interrupts_file.readlines():
    data=regsub.split(string.strip(line),'[ :+]+')
    table.body.append(
	[ HTMLgen.Text(data[2]),data[0],data[1] ])

doc.write("interrupts.html")


    
