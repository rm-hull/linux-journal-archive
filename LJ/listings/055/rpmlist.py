#!/usr/bin/python
import string, os, HTMLgen

allrpms = "rpm -q -a --queryformat '%{group} \
%{name} %{summary}\n'" 

inpipe  = os.popen(allrpms, "r")
rpmlist = inpipe.readlines(); inpipe.close()
rpmlist.sort()

indexfile="rpm.html"
mainfile="rpmlist.html"

idoc = HTMLgen.SeriesDocument("rpmstyle.rc")
mdoc = HTMLgen.SeriesDocument("rpmstyle.rc")

ilist = HTMLgen.List(style="compact", columns=3)
idoc.append(ilist)

lastgroup = ""
for rpm in rpmlist:
    fields = string.split(rpm)
    group, name = (fields[0], fields[1])
    summary = string.join(fields[2:], " ")
    if group != lastgroup:
	lastgroup = group
        title = HTMLgen.Text(group)
        href  = HTMLgen.Href(mainfile+"#"+ group,
                             title)
        ilist.append(href)
        anchor = HTMLgen.Name(group, title)
        mdoc.append(HTMLgen.Heading(2, anchor))
        grplist = HTMLgen.DefinitionList()
        mdoc.append(HTMLgen.Blockquote(grplist))
    grplist.append(
       (HTMLgen.Text(name),HTMLgen.Text(summary)))

idoc.write(indexfile)
mdoc.write(mainfile)

