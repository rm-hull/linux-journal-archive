import java.io.*;
import java.util.*;

public class PW {
	/** The name of the standard password file */
	public static final String PWFILE = "/etc/passwd";
	/** The number of fields in a pwent */
	public static final int NF = 7;
	/** The username (uid name) */
	public String pw_nam;
	/** The encrypted password (may be x or * if shadow passwords in use) */
	public String pw_passwd;
	/** The user number (uid number) */
	public int pw_uid;
	/** The group number (gid number) */
	public int pw_gid;
	/** The "GECOS" (accounting data, normally user name) */
	public String pw_gecos;
	/** The user's home directory */
	public String pw_home;
	/** The shell program to use, or null if /bin/sh is to be defaulted */
	public String pw_shell;
	/** The list of names for the elements, in the same order */
	public static final String colNames[] = {
		"User", "Passwd", "UID", "GID", "Accounting", "Home", "Shell"
	};

	/** Make a printable representation of this PW entry */
	public String toString() {
		if (pw_nam == null)
			return "PW: [invalid]";
		return "PW: [name="+pw_nam+"]";
	}
}
