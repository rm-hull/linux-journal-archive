/** Simple program to test the PW class */
public class PWTest {
	public static void main(String av[]) {
		PWReader pr = new PWFileReader("./passwd.txt");

		PW root = pr.getpwnam("root");
		if (root == null)
			System.out.println("Root missing");
		else
			System.out.println("Root = " + root);

		PW me = pr.getpwuid(100);
		if (me == null)
			System.out.println("Self missing");
		else
			System.out.println("Self = " + me);

	}
} 
