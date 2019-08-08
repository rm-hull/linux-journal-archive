/* Import these libraries */

using System;
using System.IO;
using System.Collections;
using System.Configuration;
using Gtk;
using Nwc.XmlRpc;
using Glade;
using GLib;
using GtkSharp;

public class MonoBlog
{

    /** Configuration details for the user's weblog */

    private String ServerURL;
    private String ServerUser;
    private String ServerPass;
    private String BlogID;
    private String  NumberOfPosts;
    
    

    
    
    
    /* Internal flags that tell the program if the user is editing 
	an old post */

    private bool EditingOldPost = false;
    private string  OldPostID;

    /* The widgets from the Glade file that we manipulate in this
       program */

    [Glade.Widget] TextView textview1;
    [Glade.Widget] Entry entry1;
    [Glade.Widget] TreeView treeview1;

    [Glade.Widget] Dialog prefs;
    [Glade.Widget] Entry URLEntry;
    [Glade.Widget] Entry UserEntry;
    [Glade.Widget] Entry PassEntry;
    [Glade.Widget] Entry IDEntry;
    [Glade.Widget] Entry PostEntry;
    [Glade.Widget] Button okbutton1;
    [Glade.Widget] Window MonoBlogWindow;


    public static void Main (string[] args) {
	new MonoBlog(args);
    }



    /** Get the N most recent posts from the weblog */
	  
    private void getRecentPosts() {
	

	/* Build XML-RPC request using MetaWeblog API */

	Hashtable request = new Hashtable();

     	XmlRpcRequest client = new XmlRpcRequest();
	client.MethodName = "metaWeblog.getRecentPosts";
	client.Params.Add(BlogID);
	client.Params.Add(ServerUser);
	client.Params.Add(ServerPass);
	client.Params.Add(NumberOfPosts);
	
	

	try {

	    XmlRpcResponse response = client.Send(ServerURL);		
	    
	    /* Get the array of posts */


	    ArrayList results = (ArrayList) response.Value;
	    
	    /* Build a new model for the GTKTreeView control, and
	       populate it with the posts that have just been downloaded */

	    System.Type[] ListTypes = new System.Type[3];

	    ListTypes[0] = typeof(string);
	    ListTypes[1] = typeof(string);
	    ListTypes[2] = typeof(string);
	    
	    ListStore store = new ListStore(ListTypes);

	    treeview1.Model = store;

	    TreeIter iter = new TreeIter ();

	
	    foreach (Hashtable post in results) {
	 
		String title = (String) post ["title"];
		String postid = (String) post ["postid"];
		String description = (String) post ["description"];
  	 
		store.Append (out iter);
		store.SetValue (iter, 0, new GLib.Value(title));
		store.SetValue (iter, 1, new GLib.Value(postid));
		store.SetValue (iter, 2, new GLib.Value(description));
	    
	    }
	}
	catch(Exception problem) {
	    MessageDialog md = new MessageDialog(MonoBlogWindow,
						 DialogFlags.DestroyWithParent,
						 MessageType.Error,
						 ButtonsType.Close,problem.ToString());

	 
	    
	    md.Run();
	    md.Destroy();
	}
    }
	

    /** Read in the configuration information, returning true if success,
	false if failure */

    private bool getConfig() {

	try {
	    
	    AppSettingsReader config = new AppSettingsReader();
	    ServerURL = (string) config.GetValue("ServerURL", typeof(string));
	    ServerUser =(string)  config.GetValue("ServerUser", typeof(string));
	    ServerPass = (string) config.GetValue("ServerPass", typeof(string));
	    BlogID = (string) config.GetValue("BlogID", typeof(string));
	    NumberOfPosts =  (string) config.GetValue("NumberOfPosts", typeof(string));
	    
	}
	
	catch(Exception problem) {
	    return false;
	}
	
	return true;

    }

    /** Save the configuration details back out to disk, in a very simple
	manner */

    private void saveConfig() {
	FileStream fs = new FileStream("monoblog.exe.config",
				       FileMode.OpenOrCreate,
				       FileAccess.Write);
	StreamWriter ts = new  StreamWriter(fs);
	
	ts.WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
	ts.WriteLine("<configuration>");
	ts.WriteLine("<appSettings>");
	ts.Write("<add key=\"ServerURL\" value=\"");
	ts.Write(ServerURL);
	ts.Write("\"/>\n");

	ts.Write("<add key=\"ServerUser\" value=\"");
	ts.Write(ServerUser);
	ts.Write("\"/>\n");

	ts.Write("<add key=\"ServerPass\" value=\"");
	ts.Write(ServerPass);
	ts.Write("\"/>\n");

	ts.Write("<add key=\"BlogID\" value=\"");
	ts.Write(BlogID);
	ts.Write("\"/>\n");

	ts.Write("<add key=\"NumberOfPosts\" value=\"");
	ts.Write(NumberOfPosts);
	ts.Write("\"/>\n");

	ts.WriteLine("</appSettings>");
	ts.WriteLine("</configuration>");

	ts.Flush();
	fs.Close();

    }
	

    public MonoBlog (string[] args) {
        	
	Application.Init();

	/* Loading the MonoBlog.glade file, and start building from 
	   the top node, so we have access to the main window
	   and the rpeferences window */
	
	
         
	Glade.XML gxml = new Glade.XML ("monoblog.glade", null, null);
	
	/* Binding this object's signal handlers to the Glade object */

	gxml.Autoconnect (this);


	/* Create the view into the TreeView control */

	TreeViewColumn TitleCol = new TreeViewColumn ();
	CellRenderer NameRenderer = new CellRendererText ();
	TitleCol.PackStart (NameRenderer, true); 

	/* Add attribute to column, rendering the first column in
	   the model (the title field) */

	TitleCol.AddAttribute (NameRenderer, "text", 0); 
	treeview1.AppendColumn (TitleCol); 
       

	/* Only get recent posts if configuration details were loaded */

	if(getConfig())
	    getRecentPosts();

	Application.Run();
    }


    /* Send the contents of the text forms as a new weblog entry */

    public void OnUpdateClicked(System.Object obj, EventArgs e) {
	
	
		/* Get text data from the GTK controls */

		TextBuffer buffer = textview1.Buffer;

		/* Get text from TextEntry, without hidden characters */

		String text = buffer.GetText(buffer.StartIter, buffer.EndIter, false);
		String title = entry1.Text;


		/* Package for transport */

		Hashtable entryStruct = new Hashtable();	   
		entryStruct.Add("title", title);
		entryStruct.Add("description", text);


		/* Send XML-RPC message over Internet */


		try {

		XmlRpcRequest client = new XmlRpcRequest();
		

		/* Work out whether we're updating a new post
		   or an old one, and use the appropriate MW API 
		   method */

		if(EditingOldPost) {
		    client.MethodName = "metaWeblog.editPost";
		    client.Params.Add(OldPostID);
		}
		else {
		    client.MethodName = "metaWeblog.newPost";
		    client.Params.Add(BlogID);
		}

		client.Params.Add(ServerUser);
		client.Params.Add(ServerPass);
		client.Params.Add(entryStruct);
		client.Params.Add(true);
		
		XmlRpcResponse response = client.Send(ServerURL);
		
		/* If successful, clear the text forms */
		

		if(!response.IsFault) 
		    {
			entry1.Text="";
			buffer.Delete(buffer.StartIter, buffer.EndIter);
			EditingOldPost = false;
			getRecentPosts();
		    }
		}
		catch(Exception problem) {
		    MessageDialog md = 
			new MessageDialog(MonoBlogWindow, DialogFlags.DestroyWithParent,
					  MessageType.Error,
					  ButtonsType.Close,
					  problem.ToString());
		    
		    md.Run();
		    md.Destroy();
		    
		}
		

    }


    /* Quit the program if the main window is closed */
	    
    public void OnWindowDeleteEvent (System.Object obj, 
				     DeleteEventArgs args) {
	Application.Quit ();
    }

    /* Quit if user clicks on the quit button */

    public void OnButtonQuitClicked (System.Object obj, EventArgs e) {
	Application.Quit ();
    }

    /* Clear the forms when the New Post button is clicked,
       and clear the EditingOldPost flag */

    public void OnNewPostClicked(System.Object obj, EventArgs e) {
	TextBuffer buffer = textview1.Buffer;
	entry1.Text="";
	buffer.Delete(buffer.StartIter, buffer.EndIter);
	EditingOldPost = false;
    }

    /** When a user clicks on an entry in the Tree, get the selected
	entry, and fill in the text fields with the old entry */

    public void SelectOldPost(System.Object obj, EventArgs e) {

	TreeSelection currentSelection = treeview1.Selection;
	TreeIter iter;
	TreeModel model = treeview1.Model;
	currentSelection.GetSelected (out model, out iter);
	String selected = (string) model.GetValue (iter, 1);
	String oldTitle = (string) model.GetValue(iter,0);
	String oldEntry = (string) model.GetValue(iter,2);
	
	TextBuffer buffer = textview1.Buffer;
	entry1.Text = oldTitle;
	buffer.SetText(oldEntry);

	OldPostID = selected;
	EditingOldPost = true;
    }

    /** Load the preferences dialog and fill in the current configuration 
	details */
    
    public void OnPrefsActivate(System.Object obj, EventArgs e) {
	
	URLEntry.Text = ServerURL;
	UserEntry.Text = ServerUser;
	PassEntry.Text = ServerPass;
        IDEntry.Text = BlogID;
	PostEntry.Text = NumberOfPosts;
	prefs.Run();
	

    }

    /** When the user clicks "OK" on the prefs panel, update the
     current settings, write them out to disk, and refresh the 
     recent posts again */

    public void OnUpdatePrefs(System.Object obj, EventArgs e) {
	
     
	ServerURL = URLEntry.Text;
        ServerUser = UserEntry.Text;
	ServerPass = PassEntry.Text;
	BlogID = IDEntry.Text;
	NumberOfPosts = PostEntry.Text;  
	

	prefs.Hide();
	saveConfig();
	getRecentPosts();
    }

    /** If a user clicks "Cancel", then simply close the dialog */

    public void OnPrefsCancel(System.Object obj, EventArgs e) {
       	prefs.Hide();
	
    }

}

