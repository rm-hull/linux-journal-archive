import java.io.*;
import java.util.*;

public class PWFileReader extends PWReader {
	public static final String SEP = ":";
	public static final char SEPC = ':';

	// Constructors
	PWFileReader() {
		super();
	}
	PWFileReader(String fn) {
		super(fn);
	}

	// READING METHODS
	private final int INVAL = 0, NNAM = 1, NPW = 2, NUID = 3, NGID = 4,
		NGECOS = 5, NHOME = 6, NSH = 7;
	private PW getNext() {
		String line = null;
		StringBuffer buf = null;

		// get a line
		try {
			line = pwf.readLine();
		} catch (IOException e) {
			return null;		// if the read failed
		}
		if (line == null)
			return null;		// if we got EOF

		PW p = new PW();

		buf = new StringBuffer();
		int state = NNAM;
		for (int i=0; i<line.length(); i++) {
			char c;
			if ((c=line.charAt(i)) == SEPC) {
				switch(state) {
				default:
				case INVAL:
					throw new IllegalArgumentException("Bad state "+state +
						" in " + line);
				case NNAM:
					p.pw_nam = buf.toString(); buf=new StringBuffer(); break;
				case NPW:
					p.pw_passwd = buf.toString();
					buf=new StringBuffer();
					break;
				case NUID:
					p.pw_uid = Integer.parseInt(buf.toString());
					buf = new StringBuffer();
					break;
				case  NGID:
					p.pw_gid = Integer.parseInt(buf.toString());
					buf = new StringBuffer();
					break;
				case  NGECOS:
					p.pw_gecos = buf.toString();
					buf=new StringBuffer();
					break;
				case  NHOME:
					p.pw_home = buf.toString();
					buf=new StringBuffer();
					break;
				case  NSH:
					throw new IllegalArgumentException(
						"Should not reach NSH here");
				}
				state++;
			}
				else buf.append(c);
		}
		// At this point, the shell field, buf should contain the shell
		// (or null, since the last field=="" means use the default sh).
		if (state != NSH)
			throw new IllegalArgumentException(
						"Line incomplete: " + line);
		if (buf.length() != 0)
			p.pw_shell = buf.toString();
		return p;
	}

	public PW getpwent() {
		return getNext();
	}

	public PW getpwnam(String name) {
		PW p;
		do {
			p = getNext();
		} while (p != null && (!p.pw_nam.equals(name)));
		return p;
	}

	public PW getpwuid(int u) {
		PW p;
		do {
			p = getNext();
		} while (p != null && (p.pw_uid != u));
		return p;
	}
}
