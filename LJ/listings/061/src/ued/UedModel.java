import java.util.*;

/** UedModel is the list of users and groups. */
public class UedModel extends Observable {
	/** The list of users */
	Vector u;
	/** The list of groups */
	Vector g;

	UedModel() {
		u = new Vector();
		PWReader p = new PWFileReader("./passwd.txt");
		PW user;
		while ((user=p.getpwent()) != null)
			u.addElement(user);
		// same for groups!
		// while ((group=...)) != null)
		//	v.addItem(group);
	}

	Vector getUserVector() {
		return u;
	}
}
