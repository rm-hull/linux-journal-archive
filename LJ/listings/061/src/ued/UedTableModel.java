import javax.swing.table.*;
import java.util.*;

public class UedTableModel extends AbstractTableModel {
	/** The vector of users, each a PW object */
	Vector u = null;

	/** Construct a UedTableModel given the list of PW's */
	UedTableModel(Vector users) {
		u = users;
	}

	/** Get the name of a given column */
	public String getColumnName(int n){
		return PW.colNames[n];
	}

	/** Returns the number of users in the list. */
	public int getRowCount()  {
		return u.size();
	}

	/** Return the width of the table */
	public int getColumnCount() {
		return PW.NF;
	}

	/** Returns a data value for the cell at columnIndex and rowIndex. */
	public Object getValueAt(int row, int col)  {
		PW user = (PW)u.elementAt(row);
		switch(col) {
			case 0: return user.pw_nam;
			case 1: return user.pw_passwd;
			case 2: return ""+user.pw_uid;
			case 3: return ""+user.pw_gid;
			case 4: return user.pw_gecos;
			case 5: return user.pw_home;
			case 6: return user.pw_shell;
		}
		return "Unknown in getValueAt";
	}

	/** All cells are considered editable */
	public boolean isCellEditable(int r, int c) {
		return true;
	}

	/** Set a value in a cell. */
	public void setValueAt(Object val, int row, int col)  {
		PW user = (PW)u.elementAt(row);
		switch(col) {
			case 0: user.pw_nam = (String)val; return;
			case 1: user.pw_passwd = (String)val; return;
			case 2: user.pw_uid = Integer.parseInt((String)val); return;
			case 3: user.pw_gid = Integer.parseInt((String)val); return;
			case 4: user.pw_gecos = (String)val; return;
			case 5: user.pw_home = (String)val; return;
			case 6: user.pw_shell = (String)val; return;
		}
		System.err.println("Unknown column " + col + " in setValueAt");
	}
}
