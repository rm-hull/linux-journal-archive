
var illuminate = {

    caps : {
    		"a" : "102.PNG", "b" : "103.PNG",
    		"c" : "104.PNG", "d" : "105.PNG",
    		"e" : "106.PNG", "f" : "107.PNG",
    		"g" : "108.PNG", "h" : "109.PNG",
    		"i" : "110.PNG", "j" : null,
    		"k" : "111.PNG", "l" : "112.PNG",
    		"m" : "113.PNG", "n" : "114.PNG",
    		"o" : "115.PNG", "p" : "116.PNG",
    		"q" : "117.PNG", "r" : "118.PNG",
    		"s" : "119.PNG", "t" : "120.PNG",
    		"u" : null,
    		"v" : "121.PNG",
    		"w" : null,
    		"x" : "122.PNG",
    		"y" : null,
    		"z" : "123.PNG"
    },

    load : function () {
      this.mask();
      this.insert();
    },

    image : function(text) {
      var a = text.substring(0,1).toLowerCase();
      var link = "";
      if (a && this.caps[a]) {
        link = 'http://rubens.anu.edu.au/htdocs/' +
	       'bytype/prints/ornament/0001/' +
      	        this.caps[a];
      }
      return link;
    },

    mask : function () {
      var head = document.getElementsByTagName(
      			'head')[0];
      var rules = document.createElement('style');
      var text = document.createTextNode(
      	'div.node > div.content > img[ill] : ' +
	'{ display: inline; float:left; }\n' +

      	'div.node > div.links : ' +
	'{ clear : left; }\n' +

      	'img[ill] : { display : none; }\n'
      );
      rules.appendChild(text);
      head.appendChild(rules);
    },

    insert : function () {
      var list = this.getElements(window.document);
      var img;
      var text;
      for (var i=0; i<list.length; i++) {
	text = list[i].firstChild;
	if (text.nodeType == 3 ) {
	  img = document.createElement('img');
	  img.setAttribute('ill','true');
	  img.setAttribute('width','64px');
	  img.setAttribute('height','64px');
	  img.setAttribute('src',
		this.image(text.nodeValue));
	  text.nodeValue =
	  	text.nodeValue.substring(1);
	  list[i].insertBefore(img, text);
	}
      }
    },
	  
    getElements : function (node) {
      var rv = [];
      this.getElementsRecursive(rv, node);
      return rv;
    },

    getElementsRecursive : function (list, node) {
      for (var i=node.childNodes.length-1;i>=0;i--)
      {
        var child = node.childNodes.item(i);
        var klass = null;
        if ( child.nodeType == 1) {
          klass = child.getAttribute("class");
          if ( klass && klass == "content") {
            list.push(child);
          }
          this.getElementsRecursive(list, child);
      }
    }
  }
}

window.onload = function () { illuminate.load(); }
