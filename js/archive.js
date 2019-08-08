// archive.js

var toc_urls       = Array();
var cover_urls     = Array();
var cover_objs     = Array();
var search_enabled = -1;

// Fix this function
function issue_array()
{
	var issues = [];
	var issue  = 1;
	while ( issue <= last_issue ) {
		issues[issue] = issue;
		issue++;
	}
	for ( issue in extra_issues ) {
		issues.push(extra_issues[issue]);
	}
	return issues;
}

// Show a cover image for the specified year and issue number.
function show_cover(year, inum)
{
	var anchor = document.getElementById('toc-' + year);
	var img    = document.getElementById('cover-' + year);
	var i;

	if ( img ) {
		if ( cover_urls.length == 0 ) {
			// Create cover image urls and image objects.
			var  issues = issue_array();
		    for ( ix in issues ) {
		    	issue = issues[ix];
		    	if      ( issue < 10  ) zero = '00';
		    	else if ( issue < 100 ) zero = '0';
		    	else                    zero = '';
				toc_urls[issue-1]   = '' + zero + issue + '/toc' + zero + issue + '.html';
		    	cover_urls[issue-1] = '' + zero + issue + '/cover' + issue + '.jpg';
		    	cover_objs[issue-1] = new Image();
		    }
		}

		i = inum - 1;
		if ( !cover_objs[i].src ) {
			cover_objs[i].src = cover_urls[i];
		}
		img.src = cover_objs[i].src;
		img.alt = 'Issue ' + inum;
		if ( anchor ) {
			anchor.href = toc_urls[i];
		}
	}
}

// Show a cover image if the auto show covers check box is checked.
function auto_show_cover(year, inum)
{
	var cbox = document.getElementById('auto_show_covers_cbox');

	if ( cbox  &&  cbox.checked ) {
		show_cover(year, inum);
	}
}

function check_search()
{
	var tsearch = document.getElementById('top_search');
	var bsearch = document.getElementById('bottom_search');
	var websvr  = /127\.0\.0\.1/.test(window.location);;

	if ( websvr ) {
		if ( tsearch ) {
			tsearch.style.display    = "block";
			tsearch.style.visibility = "visible";
		}
		if ( bsearch ) {
			bsearch.style.display    = "block";
			bsearch.style.visibility = "visible";
		}
	}
	else {
		if ( tsearch ) {
			tsearch.style.display    = "none";
			tsearch.style.visibility = "hidden";
		}
		if ( bsearch ) {
			bsearch.style.display    = "none";
			bsearch.style.visibility = "hidden";
		}
	}
	return websvr;
}

function search_highlight()
{
	if ( check_search() ) {
		highlight();
	}
}


//# vim: tabstop=4: shiftwidth=4: noexpandtab:
//# kate: tab-width 4; indent-width 4; replace-tabs false;


